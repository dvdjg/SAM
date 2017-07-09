#include <QStringList>
#include <QRegExp>
#include <QDateTime>
#include <QThread>
#include <QAtomicInt>

#include "getCPUTime.h"
#include "SAM_Log.h"
#include "SAM_JVariant.h"
#include "SAM_CommandObject.h"
#include "SAM_CommandObjectPrivate.h"
#include "SAM_Helpers.h"

namespace SAM
{
    static QAtomicInt s_instanceCount;

    CommandObject::CommandObject(QObject *parent, int)
        : QObject(parent)
    {
        TRACE(LOCAL);
        construct();
    }

    CommandObject::CommandObject(QObject *parent)
        : QObject(parent), m_ptr(new CommandObjectPrivate)
    {
        TRACE(LOCAL);
        construct();
    }

    void CommandObject::construct()
    {
        TRACE(LOCAL);

        // Cuenta el número de instancias
        int nInstances = 1 + s_instanceCount.fetchAndAddRelaxed(1);

        // Elige un nombre genérico para el objeto basado en el nombre de su clase
        QString name = QString("%1[%2]")
                .arg(metaObject()->className())
                .arg(nInstances);

        setObjectName(name);

        // Programar la emisión de la señal de creación
        QMetaObject::invokeMethod( this, "info",
                                   Qt::QueuedConnection,
                                   Q_ARG( QString, objectName() ),
                                   Q_ARG( QString, "created" ),
                                   Q_ARG( QVariant, QVariant() ) );
    }

    CommandObject::~CommandObject()
    {
        TRACE(LOCAL);
        m_ptr.reset();
    }

    int CommandObject::createdCommandObjectCount()
    {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        return s_instanceCount;
#else
        return s_instanceCount.load();
#endif
    }

    const JVariant & CommandObject::appSettingsConst() const
    {
        Q_D(const CommandObject);

        return d->m_appSettings;
    }

    JVariant & CommandObject::appSettings()
    {
        Q_D(CommandObject);

        return d->m_appSettings;
    }

    void CommandObject::setObjectName(const QString &name)
    {
        TRACE(LOCAL);

        QString objectName = QObject::objectName();

        if(objectName != name)
        {
            if(!objectName.isEmpty()) {
                // Enviar el mensaje de `rename` si previamente
                // ya se tenía un nombre válido.
                QMetaObject::invokeMethod( this, "info",
                                           Qt::QueuedConnection,
                                           Q_ARG( QString, objectName),
                                           Q_ARG( QString, "rename" ),
                                           Q_ARG( QVariant, name ) );
            }

            QObject::setObjectName(name);
        }
    }

    void CommandObject::processCommand(QStringList command)
    {
        TRACE(LOCAL);

        if(command.count() > 0)
        {
            QString comm(command[0].toLower());

            if(comm == "quit") // Mensaje de terminación
            {
                emit quit();
            }
            else if(comm == "info") // Se requiere información interna del objeto
            {
                JVariant inf(getInfo());

                emit info(objectName(), "info", inf);
            }
        }
    }

    void CommandObject::processCommand(QString strCommand, QString namePattern)
    {
        TRACE(LOCAL);

        QRegExp regExp(namePattern);
        QString objName(objectName());
        QStringList commandList = strCommand.split(' ', QString::SkipEmptyParts);

        if(commandList.count() > 0 && regExp.exactMatch(objName)) {
            processCommand(commandList);
        }
    }

    QVariantMap CommandObject::getInfo() const
    {
        QVariantMap inf;

        inf = SAM::toVariant(this);

        inf["dateTime"] = QDateTime::currentDateTime()
                .toString(g_dateTimeFormat);

        double thread_cpu_time;
        if(calling_thread_cpu_time(thread_cpu_time) == 0) {
            inf["threadTime"] = thread_cpu_time;
        }

        inf["threadID"] = (long long) QThread::currentThreadId();

        return inf;
    }

    void CommandObject::init()
    {
        LOG_DEBUG(LOCAL) << "Recibida la señal 'init()' en el objeto" << objectName();
    }

    void CommandObject::quit()
    {
        TRACE(LOCAL);

        emit info(objectName(), "finished", QVariant());

        LOG_DEBUG(LOCAL) << "Recibida la señal 'quit()' en el objeto" << objectName()
                         << ". Enviando 'finished()'.";

        emit finished();
    }

    void CommandObject::setAppSettings(QVariant settings, QStringList path)
    {
        TRACE(LOCAL);

        Q_D(CommandObject);

        d->m_appSettings[path] = settings;
    }
}

