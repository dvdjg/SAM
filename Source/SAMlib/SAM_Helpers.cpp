#include <QVariant>
#include <QFile>
#include <QDir>
#include <QIODevice>
#include <QTemporaryFile>
#include <QDateTime>
#include <QMetaProperty>
#if QT_VERSION >= 0x050000
#include <QtConcurrent/QtConcurrentRun>
#else
#include <QtConcurrentRun>
#endif
#include "qtiocompressor.h"
#include "SAM_Log.h"
#include "SAM_Helpers.h"

namespace SAM
{
    QTextStream cin(stdin, QIODevice::ReadOnly);
    QTextStream cout(stdout, QIODevice::WriteOnly);
    QTextStream cerr(stderr, QIODevice::WriteOnly);

    QString g_dateTimeFormat("yyyy-MM-ddThh:mm:ss.zzz");

    QTextStream& operator<<(QTextStream& os, const QStringList& lst)
    {
        TRACE(LOCAL);

        QStringListIterator it(lst);

        os << "[";

        while (it.hasNext())
        {
            os << '\"' << it.next() << '\"';

            if(it.hasNext())
                os << ',';
        }

        os << "]";

        return os;
    }

    Debug& operator<<(Debug& os, const QStringList& lst)
    {
        TRACE(LOCAL);

        QStringListIterator it(lst);

        os << "[";

        while (it.hasNext())
        {
            os << it.next();

            if(it.hasNext())
                os << ',';
        }

        os << "]";

        return os;
    }

    QTextStream& operator<<(QTextStream& os, const QVariant& var)
    {
        if(var.type() == QVariant::StringList)
            os << var.toStringList();
        else
            os << var.toString();

        return os;
    }

    Debug& operator<<(Debug& os, const QVariant& var)
    {
        if(var.type() == QVariant::StringList)
            os << var.toStringList();
        else
            os << var.toString();

        return os;
    }

    QTextStream& operator<<(QTextStream& os, const char * sz)
    {
        return os << QString(sz);
    }

    void deleteOldFilesAsync(QString path, const QDateTime & timeThreshold,  QStringList wildcard)
    {
        // Ejecuta en un hilo trabajador el borrado de archivos anteriores a timeThreshold
        QtConcurrent::run(deleteOldFiles, path, timeThreshold, wildcard);
    }

    int deleteOldFiles(QString path, const QDateTime & timeThreshold,  QStringList wildcard)
    {
        TRACE(LOCAL);

        int nDeleted = 0;
        QDir dir(path);

        LOG_INFO(LOCAL) << "Borrando los archivos de tipo" << wildcard
                       << "de la ruta" >> path << "más antiguos que"
                       << timeThreshold.toString(g_dateTimeFormat);

        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

        QFileInfoList list = dir.entryInfoList(wildcard);

        for (int i = 0; i < list.size(); ++i)
        {
            QFileInfo fileInfo = list.at(i);
            QDateTime lastModified = fileInfo.lastModified();

            if(lastModified < timeThreshold)
            {
                QString fileName = fileInfo.filePath();

                if(QFile::remove(fileName))
                {
                    ++nDeleted;
                    LOG_INFO(LOCAL) << "Borrado el archivo" >> fileName
                                   << "por ser más antiguo que"
                                   << timeThreshold.toString();
                }
            }
        }

        if(!nDeleted) {
            LOG_INFO(LOCAL) << "No se han encontrado archivos para borrar de tipo"
                           << wildcard << "en la ruta" >> path;
        }

        return nDeleted;
    }

    quint64 deviceCopy(QIODevice & src, QIODevice &dst)
    {
        qint64 totalRead = 0;

        bool bSrc = src.open(QIODevice::ReadOnly);
        bool bDst = dst.open(QIODevice::WriteOnly);

        if(!bSrc) {
            Debug(QtDebugMsg) << "No se abrir para lectura:" >> src.errorString() LOCATION_STREAM;
        } else if(!bDst) {
            Debug(QtDebugMsg) << "No se abrir para escritura:" >> src.errorString() LOCATION_STREAM;
        }
        else
        {
            char block[4096];

            while(!src.atEnd())
            {
                qint64 in = src.read(block, sizeof(block));

                if (in <= 0)
                    break;

                qint64 out = dst.write(block, in);

                totalRead += in;
                if(in != out)
                {
                    Debug(QtDebugMsg) << "No se puede copiar un bloque" LOCATION_STREAM;

                    break;
                }
            }
        }

        return totalRead;
    }

    quint64 deviceCopyCompress(QIODevice & src, QIODevice &dst)
    {
        TRACE(LOCAL);

        quint64 totalCompressed = 0;

        QtIOCompressor compressor(&dst);
        compressor.setStreamFormat(QtIOCompressor::GzipFormat);

        totalCompressed = deviceCopy(src, compressor);

        compressor.close();

        return totalCompressed;
    }

    bool fileCopyCompress(QString srcName, QString dstName, bool remove)
    {
        // Se puede TRAZAR si pertenece a su propipo hilo
        TRACE(LOCAL);

        bool bRet = false;
        QFile srcFile(srcName);
        QFile dstFile(dstName);

        quint64 originalSize = srcFile.size();

        quint64 totalCompressed = deviceCopyCompress(srcFile, dstFile);

        quint64 finalSize = dstFile.size();

        srcFile.close();
        dstFile.close();

        if(originalSize != totalCompressed)
        {
            LOG_ERROR(SERV) << "Ha ocurrido un error al comprimir el archivo" >> dstName << "de" << originalSize
                           << "a" << finalSize << "bytes ("
                           << qPrintable(QString::number(100.0 * double(totalCompressed)/double(originalSize), 'f', 1))
                           << "% )";
        }
        else
        {
            if(remove)
                srcFile.remove();

            LOG_INFO(LOCAL) << "Se ha comprimido el archivo" >> dstName << "de" << originalSize
                           << "a" << finalSize << "bytes ("
                           << qPrintable(QString::number(100.0 * double(finalSize)/double(originalSize), 'f', 1))
                           << "% )";
            bRet = true;
        }

        return bRet;
    }

    QFuture<bool> compressFileAsync(QFile & srcFile, bool remove)
    {
        // No usar TRACE() porque puede ser usada por la biblioteca de LOG
        if(srcFile.isOpen())
            srcFile.close();

        QString fileName = srcFile.fileName();
        QString tempFileName = fileName;

        // Si se va a borrar el antiguo archivo, renombrarlo para que no aparezca
        // con su nombre cuando esta función regrese.
        if(remove)
        {
            QTemporaryFile tempFile(fileName);
            if(tempFile.open()) {
                tempFileName = tempFile.fileName();
            } else {
                tempFileName = fileName + ".tmp";
            }

            if(tempFile.remove()) // Libera el antiguo nombre del sistema de archivos
                srcFile.rename(tempFileName);
        }

        // Ejecuta la compresión en un hilo trabajador
        return QtConcurrent::run(fileCopyCompress, tempFileName, fileName + ".gz", remove);
    }

    void compressFileAsync(QString fileName)
    {
        QFile srcFile(fileName);

        compressFileAsync(srcFile, true);
    }


    bool store(QString fileName, QString newName, QString strStamp)
    {
        if(!QDir().exists(fileName))
            return false;

        if(newName.isEmpty()) {
            newName = fileName;
        }

        if(strStamp.isEmpty()) {
            strStamp = QDateTime::currentDateTime().toString("-(yyyy-MM-dd_hh.mm.ss)");
        }

        QFileInfo fi(newName);
        QString storedName = QString("%1/%2%3.%4")
                .arg(fi.path())
                .arg(fi.baseName())
                .arg(strStamp)
                .arg(fi.completeSuffix());

        QString path = QFileInfo(storedName).path();
        if(!QDir().mkpath(path))// Asegurarse de que la ruta exista
            Debug(QtDebugMsg) << "Creado el directorio" >> path << "para almacenar los logs.";

        bool bRet = QFile::rename(fileName, storedName);

        if(!bRet){
            Debug(QtDebugMsg) << "No se ha podido renombrar el archivo de log" >> fileName
                     << "a" >> storedName LOCATION_STREAM;
        } else {
            // Si el archivo no estaba comprimido con .gz, lo comprime
            if(!fileName.endsWith(".gz"))
                compressFileAsync(storedName);
        }

        return bRet;
    }

    char * mini_strcpy(char * p, const char * q)
    {
        while((*p++=*q++));
        return p;
    }

    QVariantMap toVariant(const QObject * t)
    {
        TRACE(LOCAL);

        QVariantMap inf;

        const QMetaObject *metaobject = t->metaObject();
        inf["className"] = metaobject->className();

        // Muestra el valor de todas las propiedades legibles definidas con `Q_PROPERTY`.
        int count = metaobject->propertyCount();
        for (int i = 0; i < count; ++i)
        {
            QMetaProperty metaproperty = metaobject->property(i);

            if(metaproperty.isReadable())
            {
                const char *name = metaproperty.name();
                QVariant value = t->property(name);
                QString strName(name);

                inf[strName] = value;
            }
        }

        // Busca también en todos los hijos
        QVariantMap list;
        QObjectList children = t->children();
        int nChild = 0;
        foreach(QObject* pObj, children)
        {
            QVariant var = toVariant(pObj); // Llamada recursiva
            QString name = pObj->objectName();
            ++nChild;
            if(name.isEmpty()) {
                name = QString("InstanceOf_%1").arg(pObj->metaObject()->className());
            }

            if(inf.contains(name)) {
                // Si el objeto estaba en la lista de propiedades del padre, no sacarlo de nuevo
                var = "<PROPERTY>";
            }

            if(list.contains(name)) {
                // No repetir nombres
                name = QString("%1_%2").arg(name).arg(nChild);
            }

            list[name] = var;
        }

        if(!list.isEmpty()) {
            inf["children"] = list;
        }

        return inf;
    }
}
