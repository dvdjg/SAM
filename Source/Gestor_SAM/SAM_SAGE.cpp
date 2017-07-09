#include <QRegExp>
#include <QUrl>
#include <QDateTime>
#include <QTimer>

#include "SAM_JsonUtil.h"
#include "SAM_Telnet.h"
#include "SAM_Log.h"
#include "SAM_CommandObjectPrivate.h"
#include "SAM_QEnumToString.h"
#include "SAM_SAGE.h"

namespace SAM
{
    class SAGEPrivate : public CommandObjectPrivate
    {
        friend class SAGE;

    protected:
        SAGEPrivate(QObject * parent) : m_telnet(new Telnet(parent))//, m_maxStrings(300)
        {
        }

        Telnet *m_telnet;

        /**
         * @brief Almacena una secuencia de mensajes que se pretende enviar al SAGE.
         *  Las secuencias tendrán un formato:
         *  [
         *    {"exec":"intfc", "parse":[{"name":"Initial State", "regexp":"(.*)"}]},
         *    {"wait":10000}
         *  ]
         *
         *  Es un array de órdenes que pueden ser una de las siguientes:
         *  - exec.- Envía el comando especificado al SAGE. En éste caso `intfc`. La respuesta se
         *      parseará con el array de expresiones regulares que se indican dentro de `parse`.
         *      > name.- Nombre que se dará al resultado de la expresión regular.
         *      > regexp.- Expresión regular que extraerá los datos requeridos del resultado de la
         *          ejecución de la orden en el SAGE.
         *  - wait.- Espera la cantidad de milisegundos especificada.
         *
         *  El resultado se almacenará como un mapa en `m_measures`.
         */
        JVariant m_sequence;
        int m_idxSequence; ///< Índice a cada uno de los elementos del array `m_sequence`.

        /**
         * @brief Almacena el resultado de mediciones realizadas sobre el SAGE en forma de
         *  `QVariantMap` en el que la clave es el nombre que se proporcionó en "parse"[]."name"
         *  del archivo de configuración y el valor es el resultado de aplicar la expresión
         *  regular "parse"[]."regexp".
         */
        JVariant m_measures;
    };

    SAGE::SAGE(QObject * parent, int i) : CommandObject(parent, i)
    {
    }

    SAGE::SAGE(QObject * parent) : CommandObject(parent, -1)
    {
        TRACE(TELNET);
        SAGEPrivate * d = new SAGEPrivate(this);
        m_ptr.reset(d);

        // Establecer el inductor de órdenes que usa el SAGE (compatibilizado con el de RHEL)
        d->m_telnet->setPromptPattern(QRegExp("(?:[^\n]*\\\\>|\\[[^\n]*\\][\\$#])\\s*$"));

        connect(d->m_telnet, SIGNAL(message(const QString &)),
                this, SLOT(telnetRawMessage(const QString &)));
        connect(d->m_telnet, SIGNAL(loginRequired()),
                this, SLOT(telnetLoginRequired()));
        connect(d->m_telnet, SIGNAL(loginFailed()),
                this, SLOT(telnetLoginFailed()));
        connect(d->m_telnet, SIGNAL(loggedOut()),
                this, SLOT(telnetLoggedOut()));
        connect(d->m_telnet, SIGNAL(loggedIn()),
                this, SLOT(telnetLoggedIn()));
        connect(d->m_telnet, SIGNAL(commandResponse(QString, QString)),
                this, SLOT(onCommandResponse(QString,QString)));
        connect(d->m_telnet, SIGNAL(socketError(QAbstractSocket::SocketError)),
                this, SLOT(telnetSocketError(QAbstractSocket::SocketError)));
    }

    QVariant SAGE::measures() const
    {
        Q_D(const SAGE);

        return d->m_measures;
    }

    void SAGE::clearMeasures()
    {
        Q_D(SAGE);

        d->m_measures.clear();
    }

    void SAGE::setAppSettings(QVariant settings, QStringList path)
    {
        TRACE(TELNET);
        CommandObject::setAppSettings(settings, path);

        // Siempre que se actualice la configuración reintentar conectar al anfitrión.
        static const QString strThisPath("LOCAL/SAGE");
        QString strPath = path.join("/");

        if(strThisPath.startsWith(strPath)) {
            connectToHost();
        }
    }

    qint64 SAGE::sendCommand(const QString & str)
    {
        TRACE(TELNET);
        Q_D(SAGE);

        return d->m_telnet->sendCommand(str);
    }

    void SAGE::telnetRawMessage(const QString &msg)
    {
        Q_UNUSED(msg)
        //LOG_DEBUG(TELNET) << "Recibido mensaje en crudo:" << SAM::JsonUtil::stringify(msg);
    }

    void SAGE::onCommandResponse(QString command, QString response)
    {
        Q_D(SAGE);

        response = Telnet::stripCR(response);
        LOG_INFO(TELNET) << "Comando y respuesta {" << SAM::JsonUtil::stringify(command)
                         << ": " << SAM::JsonUtil::stringify(response) << "}.";

        // Si hay una secuencia de análisis activa, se intenta parsear esta respuesta
        int sequenceSize = d->m_sequence.size();
        if(sequenceSize > d->m_idxSequence)
        {
            JVariant element = d->m_sequence[d->m_idxSequence];
            QString exec = element["exec"];
            if(!exec.isEmpty())
            {
                if(exec == command)
                {
                    // Se ha recibido la respuesta a las mediciones que se pidieron
                    JVariant parsers = element["parse"];
                    int nParse = parsers.size();
                    for(int i = 0; i < nParse; ++i)
                    {
                        // Aplicar todas las lecturas de expresiones regulares que se hayan definido
                        // para esta consulta
                        JVariant parse = parsers[i];
                        QString name = parse["name"];
                        if(name.isEmpty()) {
                            // Si no se proporcionó una etiqueta para ésta medida
                            // toma el nombre del comando que la originó.
                            name = command;
                        }

                        QString regexp = parse["regexp"];
                        if(!regexp.isEmpty())
                        {
                            QRegExp rx(regexp);
                            if(rx.indexIn(response) >= 0)
                            {
                                QString captured = rx.cap(1);
                                // Añadir una marca temporal para saber cuándo se hizo la última lectura
                                d->m_measures["timeStamp"] = QDateTime::currentDateTime();
                                d->m_measures[name] = captured;

                                // Notifica que se ha realizado una lectura de un valor del SAGE
                                emit measure(name, captured);

                                LOG_INFO(PARSNG) << "Analizado el parámetro del SAGE {"
                                                 << SAM::JsonUtil::stringify(name) << ": "
                                                 << SAM::JsonUtil::stringify(captured) << "}.";
                            }
                            else
                            {
                                LOG_ALERT(PARSNG) << "No se ha casado el parámetro"
                                                  << SAM::JsonUtil::stringify(name)
                                                  << "con la respuesta del SAGE";
                            }
                        }
                        else
                        {
                            LOG_WARN(PARSNG) << "No hay una expresión regular asociada a"
                                             << SAM::JsonUtil::stringify(name);
                        }
                    }
                }
                else
                {
                    LOG_WARN(PARSNG) << "Se ha recibido la respuesta que no se esperaba"
                                     >> command << "ante un comando" >> exec;
                }
            }

            // Se intenta analizar el elemento siguiente
            d->m_idxSequence++;
            processSequenceElement();
        }
    }

    void SAGE::connectToHost(QString strURL)
    {
        TRACE(TELNET);
        Q_D(SAGE);

        if(strURL.isEmpty()) {
            const JVariant & appSettings = appSettingsConst();
            strURL = appSettings["LOCAL"]["SAGE"]["SAGE_TELNET_URL"].toString();
        }

        QUrl urlSAGE(strURL);
        QString ip = urlSAGE.host();
        quint16 port = urlSAGE.port();

        // Configurar un nombre descriptivo para esta conexión de Telnet al SAGE.
        QString name(QString("tcp://%1:%2#SAGE_TELNET").arg(ip).arg(port));

        if(d->m_telnet->isValid() && name != objectName()) {
            // Se ha cambiado de anfitrión
            d->m_telnet->close();
        }

        if(!d->m_telnet->isValid())
        {
            if(name != objectName()) {
                setObjectName(name);
                d->m_telnet->setObjectName(name+"_CLIENT");
            }

            LOG_INFO(TELNET) << "Conectando al servidor" << name;

            d->m_telnet->setWindowSize(80, 50);

            // Si no hay respuesta, reintentar la operación cada 20 segundos
            d->m_telnet->setTimeToRetry(20);

            d->m_telnet->connectToHost(ip, port);
        }
        else
        {
            LOG_DEBUG(TELNET) << "Se evita la reconexión a un anfitrión al que ya se está conectado: "
                              << objectName();
        }
    }

    void SAGE::telnetLoginRequired()
    {
        TRACE(TELNET);
        Q_D(SAGE);

        const JVariant & appSettings = appSettingsConst();
        QString strURL = appSettings["LOCAL"]["SAGE"]["SAGE_TELNET_URL"];
        QUrl urlSAGE(strURL);
        QString login = urlSAGE.userName();
        QString password = urlSAGE.password();
        //QString login = appSettings["LOCAL"]["SAGE"]["SAGE_TELNET_USER"];
        //QString password = appSettings["LOCAL"]["SAGE"]["SAGE_TELNET_PASWORD"];

        LOG_INFO(TELNET) << "Credenciales requeridas por" << objectName();

        d->m_telnet->login(login, password);
    }

    void SAGE::telnetLoginFailed()
    {
        TRACE(TELNET);

        LOG_WARN(TELNET) << "No se ha aceptado el Login" << objectName();
    }

    void SAGE::telnetLoggedOut()
    {
        TRACE(TELNET);

        LOG_ALERT(TELNET) << "Deslogado" << objectName();
    }

    void SAGE::telnetLoggedIn()
    {
        TRACE(TELNET);
        //Q_D(SAGE);

        LOG_INFO(TELNET) << "Logado" << objectName();

        // Enviar la secuencia de inicio que se encargará de configurar al SAGE
        // a un estado conocido y seguro para las posteriores mediciones.
        JVariant sageInit = appSettingsConst()["LOCAL"]["SAGE"]["SAGE_INIT"];
        LOG_DEBUG(TELNET) << "SAGE_INIT" << sageInit.toString();
        processSequence(sageInit);
    }

    void SAGE::processSequence(const JVariant & sequence)
    {
        Q_D(SAGE);

        d->m_sequence = sequence;
        d->m_idxSequence = 0;

        processSequenceElement();
    }

    bool SAGE::processSequenceElement()
    {
        TRACE(TELNET);
        Q_D(SAGE);

        int sequenceSize = d->m_sequence.size();
        while(sequenceSize > d->m_idxSequence)
        {
            JVariant element = d->m_sequence[d->m_idxSequence];
            QString exec = element["exec"];

            if(!exec.isEmpty())
            {
                qint64 nWritten = sendCommand(exec);
                if(nWritten <= 0) {
                    LOG_WARN(TELNET) << "No se ha podido enviar el comando" >> exec
                                     << "al SAGE por lo que se interrumpe la secuencia de medidas.";
                    emit measureSequenceFinished(false);
                    return false;
                }
                return true;
            }

            JVariant wait = element["wait"];

            if(wait)
            {
                ++d->m_idxSequence;
                int msec = wait;
                if(msec > 0) {
                    // Programar una espera. Esto sirve para que el SAGE disponga de
                    // tiempo suficiente para realizar sus medidas.
                    QTimer::singleShot(msec, this, SLOT(processSequenceElement()));
                }
                return true;
            }

            // No debería llegarse a éste punto
            LOG_ERROR_LN(TELNET) << "No se ha reconocido una orden válida para el SAGE:"
                                 << element.toJson();

            ++d->m_idxSequence;
        }

        emit measureSequenceFinished(true);

        return false;
    }

    void SAGE::telnetSocketError(QAbstractSocket::SocketError error)
    {
        TRACE(TELNET);

        QEnumToString<QAbstractSocket> enumToString("SocketError");
        const char * szSocketError = enumToString(error);
        LOG_WARN(TELNET) << "Error de socket en" << objectName()
                         << ":" << szSocketError;
    }

    void SAGE::telnetResponseTimeout()
    {
        TRACE(TELNET);
        Q_D(SAGE);

        int sequenceSize = d->m_sequence.size();
        if(sequenceSize > d->m_idxSequence) {
            LOG_ALERT(TELNET) << "Se cancela la secuencia de mediciones "
                                 "porque se ha recibido un timeout del Socket.";
            emit measureSequenceFinished(false);
        }
    }

    void SAGE::suspend()
    {
        TRACE(TELNET);
        Q_D(SAGE);

        LOG_DEBUG(TELNET) << "Suspend" << objectName();

        d->m_telnet->sendControl(Telnet::Suspend);
    }

    void SAGE::quit()
    {
        TRACE(TELNET);
        Q_D(SAGE);

        d->m_telnet->sendControl(Telnet::InterruptProcess);

        CommandObject::quit();
    }


    void SAGE::logout()
    {
        TRACE(TELNET);
        Q_D(SAGE);

        d->m_telnet->logout();
    }

}
