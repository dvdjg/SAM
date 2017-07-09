#ifndef SAM_LOGGERDEST_H
#define SAM_LOGGERDEST_H

#include <QSharedPointer>
#include "SAM_ExplicitlySharedDataPointer.h"
#include "basename.h"


#define LOCATION_STREAM << __FILE__ << '@' << __LINE__ << ':' << Q_FUNC_INFO
//#define LOCATION_STREAM << SAM::thisFile.base << '@' << __LINE__ << ':' << Q_FUNC_INFO


class QTextStream;
class QString;

namespace SAM
{

    /**
     * @brief Clase auxiliar para extraer el nombre del archivo que se
     *  está compilando a partir de la macro `__FILE__` del preprocesador.
     *  Se usa en la generación de trazas de entrada y salida de funciones.
     */
    class SThisFile
    {
    public:
        const char * base;
        SThisFile(const char file[]) : base(SAM::basename(file)) {}
    };

    /** Variable estática a cada módulo objeto con un puntero al nombre base
     *  del archivo c/c++ que se está compilando en ese momento.
     * Se usa en la generación de trazas de entrada y salida de funciones.
     */
    static SThisFile thisFile(__FILE__);

    class AppenderPrivate;

    /**
     * @brief Clase base para todos los _appenders_ del log.
     *  Los _appenders_ son los sumideros donde el log escribe.
     */
    class Appender
    {
        J_DECLARE_PRIVATE(Appender)

    protected:
        Appender(AppenderPrivate * p);

    public:
        Appender();
        Appender(const Appender & o);
        ~Appender();
        bool operator == (const Appender & other) const;
        bool operator != (const Appender & other) const;
        Appender & operator = (const Appender & other);
        operator bool() const;

        void write(const QString& message);
    protected:
        ExplicitlySharedDataPointer<AppenderPrivate> m_ptr;
    };

    class FileAppenderPrivate;
    /**
     * @brief Especialización del Appender para escribir en archivos.
     *
     */
    class FileAppender : public Appender
    {
        J_DECLARE_PRIVATE(FileAppender)

    public:
        FileAppender(const QString& filePath);
        FileAppender(const FileAppender& o) : Appender(o) {}
        QString fileName() const;
        void setFileName(const QString& newFileName);
        bool rename(const QString& filePath);
        void setMaxFileSize(long long maxFileSize);
        long long maxFileSize() const;
        QString fmtDateTime() const;
        void setFmtDateTime(QString fmtDateTime);
    };

    class DebugOutputAppenderPrivate;
    /**
     * @brief Especialización del Appender para escribir en la salida de depuración del sistema.
     */
    class DebugOutputAppender : public Appender
    {
        J_DECLARE_PRIVATE(DebugOutputAppender)

    public:
        DebugOutputAppender();
    };

    class StreamOutputAppenderPrivate;
    /**
     * @brief Especialización del Appender para escribir en QTextStream.
     */
    class StreamOutputAppender : public Appender
    {
        J_DECLARE_PRIVATE(StreamOutputAppender)

    public:
        StreamOutputAppender(QTextStream & ts);
    };

} // end namespace

#endif // SAM_LOGGERDEST_H
