
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>
#include <QString>
#include <QMutexLocker>
//#include <QDebug>

#include "SAM_LoggerAppender.h"
#include "SAM_Helpers.h"
#include "SAM_Debug.h"


#if defined(Q_OS_WIN)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/**
 * @brief Depuración a través de los servicios del depurador de Windows
 *  mediante `OutputDebugStringW()` que puede ser capturada por el depurador
 *  del entorno de desarrollo.
 * @note En la compilación Unix se hace uso de los servicios del depurador
 *  de Qt, normalmente `stderr`.
 * @param message
 */
static void DebugOutput( const QString& message )
{
    // OutputDebugString no es threadsafe.
    static QMutex outputDebugStringMutex;
    QMutexLocker locker(&outputDebugStringMutex);
    int pos = 0;
    do
    {
        // Se trocea la cadena en bloques de 1000 bytes debido
        // a una limitación interna
        QString ref = message.midRef(pos, 1000).toString();

        const WCHAR* szMessage = reinterpret_cast<const WCHAR*>(ref.utf16());
        OutputDebugStringW(szMessage);
        pos += 1000;
    } while(pos < message.size());

    OutputDebugStringW(L"\n");

    //OutputDebugStringA(message.toLocal8Bit().data());
    //OutputDebugStringA("\n");
}
#else
/**
 * @brief Depuración a través de los servicios del depurador de Qt, normalmente `stderr`.
 * @note En la compilación Windows se hace uso de los servicios del depurador
 *  de Windows mediante `OutputDebugStringW()`.
 * @param message
 */
static void DebugOutput( const QString& message )
{
    SAM::Debug(QtDebugMsg) << message;
}
#endif


namespace SAM
{

    /**
     * @brief Implementación privada de SAM::Appender.
     */
    class AppenderPrivate : public QSharedData
    {
        Q_DISABLE_COPY(AppenderPrivate)

    public:
        AppenderPrivate() {}

        virtual ~AppenderPrivate() {}

        /**
         * @brief Función para ser sobrecargada en las clases derivadas.
         * Es donde se realiza la escritura al dispositivo final.
         *
         * @param message Mensaje de log a escribir.
         */
        virtual void write(const QString& message) { Q_UNUSED(message); }

        QMutex m_mutex; /**< La escritura en cada `appender` se realiza en exclusión mutua */
    };

    Appender::Appender() : m_ptr(new AppenderPrivate) {}
    Appender::Appender(AppenderPrivate * p) : m_ptr(p) {}
    Appender::Appender(const Appender & o) : m_ptr(o.m_ptr) {}
    Appender::~Appender() {}
    void Appender::write(const QString& message) { m_ptr->write(message);}
    bool Appender::operator == (const Appender & other) const { return m_ptr == other.m_ptr; }
    bool Appender::operator != (const Appender & other) const { return m_ptr != other.m_ptr; }
    Appender::operator bool() const { return m_ptr; }
    Appender & Appender::operator = (const Appender & other) { m_ptr = other.m_ptr; return *this; }

    /**
     * @brief Implementación privada de SAM::FileAppender.
     */
    class FileAppenderPrivate : public AppenderPrivate
    {
        Q_DISABLE_COPY(FileAppenderPrivate)

    public:
        FileAppenderPrivate() : m_maxFileSize(1 << 20) {}
        FileAppenderPrivate(const QString& filePath)
            : m_fmtDateTime("-(yyyy-MM-dd_hh.mm.ss)"), m_maxFileSize(1 << 20)
        {
            m_logFile.setFileName(filePath);
        }

        /**
         * @brief Escribe el mensaje de log al archivo.
         *
         * @param message Mensaje de log a escribir.
         */
        virtual void write(const QString& message);

        /**
         * @brief Abre el archivo de log en solo escritura.
         *
         * @return bool true si el archivo está escribible.
         */
        bool openFile();

        QFile m_logFile; /**< Archivo donde se escribe el log */
        QTextStream m_logStream; /**< Envoltorio de tipo QTextStream del archivo FileAppenderPrivate::m_logFile */
        QString m_fmtDateTime; ///< Formato de la marca de fecha que será postpuesta al nombre de los archivos de log viejos.
        long long m_maxFileSize; /**< Tamaño máximo que se permite al archivo de log antes de ser almacenado mediante store() */
    };



    void FileAppenderPrivate::write(const QString& message)
    {
        QMutexLocker lock(&m_mutex);

        if(openFile())
        {
            m_logStream << message << endl;

            long long size = m_logFile.size();

            if(size > m_maxFileSize)
            {
                m_logFile.close();

                store(m_logFile.fileName(), QString(),
                      QDateTime::currentDateTime().toString(m_fmtDateTime));
            }
        }
    }

    FileAppender::FileAppender(const QString& filePath)
        : Appender(new FileAppenderPrivate(filePath))
    {
    }

    void FileAppender::setMaxFileSize(long long maxFileSize)
    {
        if(maxFileSize > 1)
        {
            J_D(FileAppender);
            d->m_maxFileSize = maxFileSize;
        }
    }

    long long FileAppender::maxFileSize() const
    {
        J_D(const FileAppender);
        return d->m_maxFileSize;
    }

    QString FileAppender::fmtDateTime() const
    {
        J_D(const FileAppender);
        return d->m_fmtDateTime;
    }

    void FileAppender::setFmtDateTime(QString fmtDateTime)
    {
        J_D(FileAppender);
        d->m_fmtDateTime = fmtDateTime;
    }

    bool FileAppender::rename(const QString& filePath)
    {
        J_D(FileAppender);

        return d->m_logFile.rename(filePath);
    }

    QString FileAppender::fileName() const
    {
        J_D(const FileAppender);

        QString filename = d->m_logFile.fileName();

        return filename;
    }

    void FileAppender::setFileName(const QString& newFileName)
    {
        J_D(FileAppender);

        QMutexLocker lock(&d->m_mutex);

        if (d->m_logFile.isOpen())
            d->m_logFile.close();

        QString oldFilename = d->m_logFile.fileName();

        // Si ese archivo ya existiera, lo renombra
        store(newFileName, QString(),
                 QDateTime::currentDateTime().toString(d->m_fmtDateTime));

        // Lo mueve a la nueva ubicación
        QFile::rename(oldFilename, newFileName);

        d->m_logFile.setFileName(newFileName);
    }


    bool FileAppenderPrivate::openFile()
    {
        bool isOpen = m_logFile.isOpen();

        if (!isOpen)
        {
            QString path = QFileInfo(m_logFile.fileName()).path();
            if(!QDir().mkpath(path))// Asegurarse de que la ruta exista
                Debug(QtDebugMsg) << "Creado el directorio" << path << "para almacenar los logs.";

            if (m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
            {
                m_logStream.setDevice(&m_logFile);
                isOpen = true;
            }
            else
            {
                Debug(QtDebugMsg) << "> No se puede abrir el archivo" >> m_logFile.fileName()
                        << "para escribir el log:" >> m_logFile.errorString() LOCATION_STREAM;
            }
        }

        return isOpen;
    }


    /**
     * @brief Implementación privada de SAM::DebugOutputAppender.
     */
    class DebugOutputAppenderPrivate : public AppenderPrivate
    {
        Q_DISABLE_COPY(DebugOutputAppenderPrivate)

    public:
        DebugOutputAppenderPrivate() {}

        /**
         * @brief Escribe el log hacia la salida de depuración del sistema.
         *
         * @param message Mensaje de log a mostrar.
         */
        virtual void write(const QString& message);
    };

    void DebugOutputAppenderPrivate::write(const QString& message)
    {
        QMutexLocker lock(&m_mutex);

        DebugOutput(message);
    }

    DebugOutputAppender::DebugOutputAppender()
        : Appender(new DebugOutputAppenderPrivate())
    {
    }

    /**
     * @brief Implementación privada de SAM::StreamOutputAppender.
     */
    class StreamOutputAppenderPrivate : public AppenderPrivate
    {
        Q_DISABLE_COPY(StreamOutputAppenderPrivate)

    public:
        StreamOutputAppenderPrivate(QTextStream & logStream) : m_logStream(logStream) {}

        /**
         * @brief Escribe al `stream` que se haya configurado.
         *
         * @param message
         */
        virtual void write(const QString& message);

        QTextStream & m_logStream;
    };

    void StreamOutputAppenderPrivate::write(const QString& message)
    {
        QMutexLocker lock(&m_mutex);

        m_logStream << message << endl;
    }


    StreamOutputAppender::StreamOutputAppender(QTextStream & ts)
        : Appender(new StreamOutputAppenderPrivate(ts))
    {
    }

} // end namespace
