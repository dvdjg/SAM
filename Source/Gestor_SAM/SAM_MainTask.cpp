#include <QtCore/QCoreApplication>
#include <QStringList>
#include <QThread>
#include <QSharedMemory>
#include <QTimer>
#include <QFileInfo>
#include <QDir>
#include <QNetworkProxy>
#include <QNetworkInterface>
#include <QDir>
#include <QDateTime>
#include <QUrl>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QUrlQuery>
#endif

#include "SAM_Log.h"
#include "SAM_JVariant.h"
#include "SAM_JsonUtil.h"
#include "SAM_TcpServerCommandProcessor.h"
#include "SAM_UdpServerMessageProcessor.h"
#include "SAM_UdpServerSniffer.h"
#include "SAM_UdpServer.h"
#include "SAM_TcpServer.h"
#include "SAM_TcpUdpClient.h"
#include "SAM_MessageProcessor.h"
#include "SAM_Sniffer.h"
#include "SAM_Helpers.h"
#include "SAM_Injector.h"
#include "SAM_Testing.h"
#include "SAM_MainTask.h"
#include "SAM_CPUInfo.h"
#include "SAM_Ping.h"
#include "SAM_SleeperThread.h"
#include "memoryStats.h"
#include "getCPUTime.h"

namespace SAM
{
    QNetworkProxy proxyFromUrl(const QUrl & url)
    {
        QNetworkProxy::ProxyType type = QNetworkProxy::DefaultProxy;

        if(url.scheme() == "socks" || url.scheme() == "socks5")
            type = QNetworkProxy::Socks5Proxy;
        else if(url.scheme() == "http")
            type = QNetworkProxy::HttpProxy;
        else if(url.scheme() == "httpcaching")
            type = QNetworkProxy::HttpCachingProxy;
        else if(url.scheme() == "ftpcaching")
            type = QNetworkProxy::FtpCachingProxy;

        QNetworkProxy proxy;
        proxy.setType(type);
        proxy.setHostName(url.host());
        proxy.setPort(url.port());
        proxy.setUser(url.userName());
        proxy.setPassword(url.password());

        return proxy;
    }


    MainTask * g_pMainTask; /**< La única instancia principal permitida */

    MainTask::MainTask(QObject *parent) :
        CommandObject(parent),
        m_datetimeFormat(g_dateTimeFormat),
        m_suitableIdEq(1),
        m_pMessageProcessor(NULL),
        m_pSniffer(NULL),
        m_pPingToHosts(NULL)
    {
        //int idPacket =
        qRegisterMetaType<Packet>("Packet");
        setObjectName("SAM_MainTask");
    }

    void MainTask::initSettings()
    {
        TRACE(LOCAL);

        // Obtener el nodo 'local' de configuraciones
        JVariant settings = appSettings();
        JVariant & LOCAL = settings[CONF_LOCAL];

        // Establecer algunos parámetros por defecto
        LOCAL["SETTINGS"]["STORAGE_PATH"] = "storage";
        LOCAL["SETTINGS"]["INI_PATH"] = QCoreApplication::applicationName() + ".ini";
        LOCAL["TRAZAS"]["FICH_TRAZAS"] = "log/" + QCoreApplication::applicationName() + ".log";
        LOCAL["TRAZAS"]["LIVING_DAYS"] = 90;
        LOCAL["SETTINGS"]["TCP_COMMANDS_PORT"] = (quint16) 7030;
        LOCAL["SETTINGS"]["UDP_MESSAGES_PORT"] = (quint16) 7040;

        const int defaultTimeout = 1000;
        JVariant & GENERAL = LOCAL["GENERAL"];

        // Timeout a la espera de que se conmute el estado del patrón
        GENERAL["TIMEOUT_SET_POS_PATRON"] = defaultTimeout;

        // Timeout a la espera de que se conmute el estado de medición del puesto
        GENERAL["TIMEOUT_SET_POS_MEDICION"] = defaultTimeout;

        // Timeout a la espera de que se conmute el estado de medición del patrón
        GENERAL["TIMEOUT_SET_LLAMADA_SAM"] = defaultTimeout;

        // Timeout a la espera de que se conmute el estado del bucle
        GENERAL["TIMEOUT_SET_BUCLE_UGA"] = defaultTimeout;

        GENERAL["TIMEOUT_SET_INT_MEDICION"] = defaultTimeout;
        GENERAL["TIMEOUT_SET_CANAL_UGA"] = defaultTimeout;

        //        // Escribir unos parámetros de conexión por defecto
        //        Settings & T_SUPER = settings()["CONFIG"]["HW"]["T_SUPER"][1];
        //        T_SUPER["MODO_MULTICAST"] = false;
        //        T_SUPER["MCAST_VOIP1"] = "225.31.0.1";
        //        T_SUPER["PUERTO_VOIP_CONTROL"] = 7003;
        //        T_SUPER["PUERTO_VOIP_SUPERVISION"] = 7002;
        //        T_SUPER["INTERVALO_PRESENCIAS"] = 1000;
        //        T_SUPER["PUERTO_WATCHDOG"] = 4656;

        setAppSettings(LOCAL, QStringList() << CONF_LOCAL);
    }

    void MainTask::parseArguments()
    {
        TRACE(LOCAL);

        QStringList arguments = QCoreApplication::arguments();
        QStringListIterator argsIterator(arguments);

        while (argsIterator.hasNext())
        {
            QString arg = argsIterator.next();
            QString key = arg.section('=', 0, 0);
            QString value = arg.section('=', 1);
            QStringList path;

            if(key == "--log")
            {
                // Log path and filename
                path << CONF_LOCAL << "TRAZAS" << "FICH_TRAZAS";
                appSettings()[path] = value;
            }
            else if(key == "--storage")
            {
                // Storage path
                path << CONF_LOCAL << "SETTINGS" << "STORAGE_PATH";
                appSettings()[path] = value;
            }
            else if(key == "--ini")
            {
                // Log path and filename
                path << CONF_LOCAL << "SETTINGS" << "INI_PATH";
                appSettings()[path] = value;
            }
            else if(key == "--sniffer") // --sniffer=udp://0.0.0.0:7003 --ini=nada
            {
                JVariant sniffer;
                sniffer["URL"] = value;

                // Configura el modo sniffer en el que se hace una copia de todos los mensajes reconocidos
                path << CONF_LOCAL << "SETTINGS" << "SNIFFERS";
                appSettings()[path].asList().append(sniffer);
            }
            else if(key == "--snifferegrep")
            {
                path << CONF_LOCAL << "SETTINGS" << "SNIFFERS";
                JVariant & sniffer = appSettings()[path].last();

                sniffer["egrep"] = value;
            }
            else if(key == "--snifferpath") // --snifferpath=sniffer/messages.sniff.json
            {
                path << CONF_LOCAL << "SETTINGS" << "SNIFFERS";
                JVariant & sniffer = appSettings()[path].last();

                sniffer["path"] = value;
            }
            else if(key == "--inject") // --inject=sniffer/messages.sniff.json.gz
            {
                JVariant injector;
                injector["path"] = value;

                // Configura el modo inyector en el que se vuelcan a la red los mensajes
                path << CONF_LOCAL << "SETTINGS" << "INJECTORS";
                appSettings()[path].asList().append(injector);
            }
//            else if(key == "--injectpath")
//            {
//                // Configura el modo inyector en el que se vuelcan a la red los mensajes
//                JVariant & injector = settings()[CONF_LOCAL]["SETTINGS"]["INJECTORS"].last();

//                injector["path"] = value;
//            }
            else if(key == "--injectegrep")
            {
                // Configura el modo inyector en el que se vuelcan a la red los mensajes
                path << CONF_LOCAL << "SETTINGS" << "INJECTORS";
                JVariant & injector = appSettings()[path].last();

                injector["egrep"] = value;
            }
            else if(key.startsWith("--settings"))
            {
                QString ruta = key.section(':', 1);

                // Log path and filename
                path = ruta.split('/', QString::SkipEmptyParts);
                appSettings()[path] = value;
            }

            if(!path.isEmpty()) {
                emit appSettingsChanged(appSettings()[path], path);
            }
        }

    }

    void MainTask::configInjectors(JVariant INJECTORS)
    {
        TRACE(LOCAL);

        for(int i = 0; i < INJECTORS.size(); ++i)
        {
            QString fileName = INJECTORS[i]["path"];

            if(fileName.isEmpty()) {
                LOG_WARN(COMS) << "No se ha especificado correctamente el nombre de un inyector.";
                return;
            }

            // Escucha las conexiones entrantes UDP de mensajes SCV
            Injector * injector = new Injector;

            injector->setObjectName(QString("%1#INJECTOR_%2").arg(fileName).arg(i));
            injector->setFileName(fileName);

            // Por ahora no hacer nada cuando se haya terminado con el archivo
            connect(injector, SIGNAL(fileInjected()), injector, SLOT(quit()));

            registerCommandObject(injector)->start();

            QMetaObject::invokeMethod( injector, "injectFile", Qt::QueuedConnection );

            LOG_INFO(COMS) << "Configurada la inyección de mensajes de" >> fileName;
        }
    }

    void MainTask::configSniffers(JVariant SNIFFERS)
    {
        TRACE(LOCAL);

//        if(SNIFFERS.size() > 0) {
//            LOG_WARN(COMS) << "Sniffing:" << SNIFFERS.size();
//        }

        for(int i = 0; i < SNIFFERS.size(); ++i)
        {
            QString strSniffer = SNIFFERS[i]["URL"];
            QString strPath = SNIFFERS[i]["path"];
            QUrl url(strSniffer);

            if(url.scheme().toLower() == "tcp") {
                LOG_WARN(COMS) << "No se soportan sniffers en TCP:" << url.toString();
                return;
            }

            if(!strPath.isEmpty()) {
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
                url.addQueryItem("path", strPath);
#else
                QUrlQuery urlQuery(url);
                urlQuery.addQueryItem("path", strPath);
                url.setQuery(urlQuery);
#endif
            }

            url.setFragment(QString("SNIFFER_%1").arg(i));

            // Escucha las conexiones entrantes UDP de mensajes SCV
            UdpServer * udpServer = new UdpServer;

            udpServer->setUrl(url);
            udpServer->setObjectName(url.toString());

            // Establecer la clase que escuchará los mensajes que lleguen
            udpServer->setRunnableFactory<UdpServerSniffer>();

            registerCommandObject(udpServer)->start();

            QMetaObject::invokeMethod( udpServer, "listen", Qt::QueuedConnection );

            LOG_INFO(COMS) << "SNIFFER_" << i << ": Configurada la escucha de mensajes "
                              "en" << url.toString();
        }
    }

    void MainTask::configCommunications(JVariant T_SUPER, const char* szType)
    {
        TRACE(LOCAL);

        if(!T_SUPER)
        {
            LOG_ERROR(CONF) << "No se han especificado los parámetros [T_SUPER] por lo "
                               "que no se configurarán los servicios de" >> szType <<
                               "para la lectura/escritura correspondientes.";

            return;
        }

        // Escucha en UDP desde cualquier interfaz para procesar
        // las comunicaciones de control y supervisión
        quint16 port = T_SUPER[QString("PUERTO_VOIP_%1").arg(szType)];

        if(port > 0)
        {
            QUrl url;

            url.setScheme("udp");
            url.setHost("0.0.0.0");
            url.setPort(port);
            url.setFragment(QString("%1_%2").arg(szType).arg("SERVER"));

            /** 5.2.6 RED
              El servidor SAM debe subscribirse en el arranque al grupo multicast
              MCAST_VOIP1 (parámetro de configuración del sistema) para comunicarse
              con el resto del sistema.
            */
            if(T_SUPER["MODO_MULTICAST"] && T_SUPER["MCAST_VOIP1"])
            {
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
                url.addQueryItem("multicast", T_SUPER["MCAST_VOIP1"]);
#else
                QUrlQuery urlQuery(url);
                urlQuery.addQueryItem("multicast", T_SUPER["MCAST_VOIP1"]);
                url.setQuery(urlQuery);
#endif
            }

            // Escucha las conexiones entrantes UDP de mensajes SAM
            UdpServer * udpServer = new UdpServer;

            udpServer->setUrl(url);
            udpServer->setObjectName(url.toString());

            // Establecer la clase que escuchará los mensajes que lleguen
            udpServer->setRunnableFactory<UdpServerMessageProcessor>();

            registerCommandObject(udpServer)->start();

            QMetaObject::invokeMethod( udpServer, "listen", Qt::QueuedConnection );

            LOG_INFO(COMS) << "Configurada la escucha de mensajes de"
                           << szType << "en" << url.toString();
        }
        else
        {
            LOG_ERROR(CONF) << "No se ha especificado el puerto de" << szType
                            << "en la configuración por lo que no se configurará "
                               "el servicio de escucha.";
        }

        if(T_SUPER["MCAST_VOIP1"] && port )
        {
            // Configura también la escritura al TMCS
            QUrl url;

            /** 5.2.6 RED
              El servidor SAM debe enviar las peticiones de solicitud de configuración
                al TMCS utilizando el puerto PUERTO_VOIP_SUPERVISION (parámetro de
                configuración del sistema), siendo el puerto destino también el
                PUERTO_VOIP_CONTROL.
              El servidor SAM debe enviar las peticiones de solicitud de configuración
                al TMCS utilizando el puerto PUERTO_VOIP_CONTROL (parámetro de
                configuración del sistema), siendo el puerto destino también el
                PUERTO_VOIP_CONTROL.
             @note Se sobreentiende que lo que quiere decir el SRD es
                que se use MCAST_VOIP1 como dirección de red.
            */
            url.setScheme("udp"); // Implícitamente la comunicación con el TMCS es UDP
            url.setHost(T_SUPER["MCAST_VOIP1"]);
            url.setPort(port);

            /** 5.2.6 RED
              El servidor SAM debe activar o desactivar el modo multicast según
                el valor del parámetro MODO_MULTICAST para las comunicaciones de
                control, configuración y mensajes de presencia.
            */
            if(T_SUPER["MODO_MULTICAST"]) {
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
                url.addQueryItem("ttl", "20");
#else
                QUrlQuery urlQuery(url);
                urlQuery.addQueryItem("ttl", "20");
                url.setQuery(urlQuery);
#endif
            } else {
                url.setHost(T_SUPER["IFZ_VOIP1"]);
            }

            registerRemoteServer(szType, url);
        }
        else
        {
            LOG_ERROR(CONF) << "No se ha especificado bien la dirección y puerto de"
                            << szType << "por lo que no se configurará el servicio "
                               "de escritura al TMCS.";
        }
    }

    void MainTask::configCommandsListener(quint16 commandsPort)
    {
        TRACE(LOCAL);

        QUrl urlSamServer;

        if(commandsPort > 0)
        {
            urlSamServer.setScheme("tcp");

            // Escucha en TCP desde cualquier interfaz para procesar comandos
            urlSamServer.setHost("0.0.0.0");
            urlSamServer.setPort(commandsPort);
            urlSamServer.setFragment("COMMANDS_SERVER");

            // Escucha las conexiones entrantes TCP de comandos
            TcpServer * tcpSamServerCommands = new TcpServer;

            tcpSamServerCommands->setUrl(urlSamServer);
            tcpSamServerCommands->setObjectName(urlSamServer.toString());

            // Establecer la clase que escuchará los mensajes que lleguen
            tcpSamServerCommands->setRunnableFactory<TcpServerCommandProcessor>();

            registerCommandObject(tcpSamServerCommands)->start();

            // Inicia la escucha
            QMetaObject::invokeMethod( tcpSamServerCommands, "listen", Qt::QueuedConnection );

        }
    }

    void MainTask::configTraces(const JVariant& trazas)
    {
        TRACE(LOCAL);

        QString logFilePath = trazas["FICH_TRAZAS"];

        Log::instance().setLogPath(logFilePath);

        QList<QVariant> nivelWarning = trazas["NIVEL"];

        QListIterator<QVariant> itNivel(nivelWarning);

        Logger::Level level = Logger::FatalLevel;

        // Deshabilitar el nivel de log de todos los módulos.
        Log::instance().setModuleLevel(level);
        LOG_INFO(LOCAL) << "Se establece el nivel de log de todos los módulos a"
                        >> Logger::levelToText(level);

        // Habilita el nivel de log de SERV
        Log::instance().getLogger("SERV").setLoggingLevel(Logger::InfoLevel);

        QList<QVariant> hilos = trazas["HILOS_TRAZA"];

        QListIterator<QVariant> itTraza(hilos);

        while(itTraza.hasNext())
        {
            if(itNivel.hasNext())
            {
                QString strLevel(itNivel.next().toString());

                level = Logger::textToLevel(strLevel);
            }

            // Habilita los niveles de log que se indiquen en settings
            QString traza(itTraza.next().toString());

            Log::instance().getLogger(traza).setLoggingLevel(level);

            LOG_INFO(LOCAL) <<  "Se establece el nivel de log del módulo"
                             << traza << "a" << Logger::levelToText(level);
        }
    }

    void MainTask::applySettings(const JVariant & settings)
    {
        TRACE(LOCAL);

        const JVariant & local = settings[CONF_LOCAL];

        configTraces(local["TRAZAS"]);

        if(local["SETTINGS"]["GLOBAL_PROXY"])
        {
            QUrl url(local["SETTINGS"]["GLOBAL_PROXY"]);

            if(!url.host().isEmpty() && url.port() > 0)
            {
                QNetworkProxy proxy = proxyFromUrl(url);

                QNetworkProxy::setApplicationProxy(proxy);

                LOG_INFO(COMS) << "Proxy" >> url.toString() << "Establecido globalmente.";
            }
        }

        quint16 commandsPort = local["SETTINGS"]["TCP_COMMANDS_PORT"];
        if(commandsPort) {
            configCommandsListener(commandsPort);
        } else {
            LOG_INFO(COMS) << "No se ha configurado el puerto para un Servidor "
                              "de comandos TCP mediante TCP_COMMANDS_PORT.";
        }

        JVariant T_SUPER = settings["CONFIG"]["HW"]["T_SUPER"][1];

        /**
         * 5.2.6 RED
         *  El servidor SAM debe escuchar en el puerto PUERTO_VOIP_CONTROL para
         *  recibir las configuraciones del sistema procedentes del TMCS.
         */
        configCommunications(T_SUPER, "CONTROL");

        /**
         * 5.2.6 RED
         *  El servidor SAM debe escuchar en el puerto PUERTO_VOIP_SUPERVISION
         *  para recibir las presencias de CWPs y GWPs.
         */
        configCommunications(T_SUPER, "SUPERVISION");

        /**
         *  5.2.2.5 Posición Patrón SAM
         *  El servidor SAM debe tener un parámetro de configuración local
         *  que indique la IP del puesto que va a funcionar como posición
         *  Patrón (parámetro de configuración, IP_POS_PATRON).
         */
//        JVariant GENERAL = local["GENERAL"];
//        QString strPatron(QString("udp://%1:%2")
//                          .arg(GENERAL["IP_POS_PATRON"].toString())
//                          .arg(T_SUPER["PUERTO_VOIP_SUPERVISION"].toString()));

        // registerRemoteServer("PATRON", strPatron);

        configSniffers(settings[CONF_LOCAL]["SETTINGS"]["SNIFFERS"]);

        configInjectors(settings[CONF_LOCAL]["SETTINGS"]["INJECTORS"]);

    }

    void MainTask::configureHostsPings()
    {
        JVariant PING_INTERVAL = appSettings()["LOCAL"]["SETTINGS"]["PING_INTERVAL"];

        if(!PING_INTERVAL)
            return;

        int pingInterval = appSettings()["LOCAL"]["PING_INTERVAL"];
        // Comprueba que los equipos existen
        if(!m_pPingToHosts) {
            m_pPingToHosts = new Ping(this);
            m_pPingToHosts->setObjectName("SAM_Ping");
        } else {
            m_pPingToHosts->clear();
        }

        const JVariant & T_EQIP = appSettings()["CONFIG"]["HW"]["T_EQIP"];

        QMapIterator<QString, QVariant> it(T_EQIP.toMap());
        while(it.hasNext()) {
            it.next();
            m_pPingToHosts->watch(it.key());
        }

        m_pPingToHosts->start(pingInterval);
    }

    QThread *MainTask::registerCommandObject(CommandObject * commandObject)
    {
        TRACE(LOCAL);

        QThread* thread;

        if(commandObject == this)
        {
            thread = this->thread();
        }
        else
        {
            thread = new QThread;
            commandObject->moveToThread(thread);

            connect(this, SIGNAL(finished()), thread, SLOT(quit()));
            connect(commandObject, SIGNAL(finished()), thread, SLOT(quit()));
            connect(commandObject, SIGNAL(finished()), commandObject, SLOT(deleteLater()));
            connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

            connect(thread, SIGNAL(started()), commandObject, SLOT(init()));
        }

        registerLocalCommandObject(commandObject);

        // Los objetos que cuenten con hilo propio responderán ante el mensaje de 'info'
        // Otros objetos secundarios aparecerán como hijos.
        connect(commandObject, SIGNAL(info(QString, QString, QVariant)),
                this, SLOT(info(QString, QString, QVariant)));

        return thread;
    }

    void MainTask::registerLocalCommandObject(CommandObject * commandObject)
    {
        TRACE(LOCAL);

        if(commandObject != this)
        {
            QObject * pParent = commandObject->parent();

            if(pParent) {
                // Si el objeto tiene padre, enganchar su señal de finalización
                connect(pParent, SIGNAL(finished()), commandObject, SLOT(quit()));

                // Provocar una iniciación incondicional para que pueda iniciarse
                QMetaObject::invokeMethod( commandObject, "init", Qt::QueuedConnection );
            } else {
                // Enganchar la señal de terminación principal para objetos sin padre
                connect(this, SIGNAL(terminated()), commandObject, SLOT(quit()));
            }
            connect(this, SIGNAL(appSettingsChanged(QVariant, QStringList)),
                    commandObject, SLOT(setAppSettings(QVariant, QStringList)));

            QMetaObject::invokeMethod( commandObject, "setAppSettings", Qt::QueuedConnection,
                                       Q_ARG( QVariant, appSettings() ) );
        }

        // Conectar el mecanismo para que el objeto informe a la aplicación
        connect(this, SIGNAL(sendCommandToObjects(QString, QString)),
                commandObject, SLOT(processCommand(QString, QString)));
    }

    QObject * MainTask::registerRemoteServer(QString serverType, QUrl serverUrl)
    {
        TRACE(LOCAL);

        // Se complementa el nombre con el tipo de servidor
        serverUrl.setFragment(serverType + "_CLIENT");

        TcpUdpClient * worker = new TcpUdpClient(serverUrl);

        QThread* thread = registerCommandObject(worker);

        connect(worker, SIGNAL(error(QString)), this, SLOT(error(QString)));

        serverType = serverType.toUpper();

        // Conectar las señales dedicadas que corresponda a cada tipo
        //  de cliente a servidor remoto.
        if(serverType == "CONTROL")
            connect(this, SIGNAL(sendPacketToControlServers(Packet)),
                    worker, SLOT(sendPacket(Packet)),
                    Qt::QueuedConnection);
        else if(serverType == "SUPERVISION")
            connect(this, SIGNAL(sendPacketToSupervisionServers(Packet)),
                    worker, SLOT(sendPacket(Packet)),
                    Qt::QueuedConnection);
        else if(serverType == "PATRON")
            connect(this, SIGNAL(sendPacketToPatronServers(Packet)),
                    worker, SLOT(sendPacket(Packet)),
                    Qt::QueuedConnection);
        else if(serverType == "ESTAD")
            connect(this, SIGNAL(sendPacketToEstadServers(Packet)),
                    worker, SLOT(sendPacket(Packet)),
                    Qt::QueuedConnection);
        else if(serverType == "SAGE")
            connect(this, SIGNAL(sendPacketToSageServers(Packet)),
                    worker, SLOT(sendPacket(Packet)),
                    Qt::QueuedConnection);
        else if(serverType == "LOCAL")
            connect(this, SIGNAL(sendPacketToSageServers(Packet)),
                    worker, SLOT(sendPacket(Packet)),
                    Qt::QueuedConnection);

        LOG_INFO(COMS) << "Configurada la escritura de mensajes de" << serverType
                       << "en" << worker->objectName();

        thread->start();

        return worker;
    }


    /**
     * @brief
     *
     * @param signal
     */
    void MainTask::handleSignal(int signal)
    {
        TRACE(LOCAL);

        const char * szSignal = SignalHandler::enumName((SignalHandler::SIGNALS)signal);

        if(signal
                & (SignalHandler::SIG_INT
                  |SignalHandler::SIG_TERM
                  |SignalHandler::SIG_CLOSE))
        {
            LOG_WARN(LOCAL) << "Recibida la señal de terminación" << szSignal;

            quit(TERMINATE_DELAY);
        }
        else
        {
            LOG_WARN(LOCAL) << "Recibida la señal" << szSignal << "que será ignorada.";
        }
    }

    QStringList MainTask::arguments() const
    {
        return QCoreApplication::arguments();
    }

    QString infoString()
    {
        TRACE(LOCAL);
        return QString(SAM_TARGET " v"SAM_VERSION". Construido con Qt v" QT_VERSION_STR
                       " corriendo en Qt v%1. Compilado el día "__DATE__" con \""SAM_COMPILER_VERSION
                       "\" en " SAM_HOST_OS " " SAM_HOST_ARCH).arg(qVersion());
    }

    QString MainTask::infoString() const
    {
        return SAM::infoString();
    }


    void MainTask::loadHwConfigFile(QString filePath)
    {
        TRACE(LOCAL);

        // Comprobar que el nombre del archivo corresponda con lo que se espera
        if(!filePath.endsWith("config_sdcxxi_hw.txt.zip")) {
            return;
        }

        JVariant configHw = JsonUtil::parseScvFile(filePath);

        // Si se puede leer la configuración desde el archivo .zip,
        //  combinarla (selectivamente) con la que se especificó en el .ini
        if(configHw)
        {
            JVariant & HW = appSettings()["CONFIG"]["HW"];

            // Combinar los parámetros de T_SUPER
            HW["T_SUPER"][1] |= configHw["T_SUPER"][1];

            // Sobreescribir la tabla de configuración
            HW["T_CONFIG"] = configHw["T_CONFIG"]; // Sustituir la configuración

            /** 5.2.5.2 Posiciones del Sistema

               El servidor SAM debe obtener los datos de las posiciones de tipo "PUESTO" de la
                tabla [T_PUES] del fichero de configuración del sistema "config_sdcxxi_hw.txt":
               - "IP": dirección IP de la posición.
               - "NOMBRE": nombre del puesto.
               - "ID": identificador del puesto.

               El servidor SAM debe obtener los datos de las UCSs de la tabla [T_UCS]
                del fichero de configuración del sistema "config_sdcxxi_hw.txt":
               - "NOMBRE": nombre de la UCS.
               - "ID": identificador de la UCS.
               - "ID_PUE_EJ": identificador del puesto ejecutivo (de la tabla [T_PUES]).
               - "ID_PUE_PL": identificador del puesto planificador (de la tabla [T_PUES]).

               El servidor SAM debe diferenciar las posiciones que forman parte de una UCS,
                ya que sobre estas posiciones se podrán activar más bucles de la UGA
               (para los Jacks 2 , del coordinador).
               [(parámetro de diseño), SAMDP_NumBuclesUGAActivados]
             */
            HW["T_PUES"] = configHw["T_PUES"];

            HW["T_UCS"] = configHw["T_UCS"];

            /**
             *  5.2.5.3 Interfaces del Sistema

                Las medidas de audio realizadas por el servidor SAM sobre las
                interfaces deben ser independientes de la configuración activa,
                por lo que para identificar a las distintas interfaces se utilizará
                los parámetros:

                - "ID_GW_USU": identificador del Gateway en el que se encuentra la interfaz.
                - "HUECO": Slot que ocupa la tarjeta en el gateway.
                - "TIPO": tipo de protocolo de la tarjeta.



                El servidor SAM solamente debe obtener datos de la configuración sobre
                gateways de tipo "GW_INDRA" y con el campo "IP" no vacío (gateways de la
                tabla [T_GATE] con el campo "TIPO" con valor "GW_INDRA" y con el campo
                "IP" no vacío, del fichero de configuración config_sdcxxi_hw.txt).

                El campo "IP" estará vacío si se trata de un gateway simulado, por eso el
                SAM no tiene en cuenta dichos gateways.

                El servidor SAM debe obtener los datos de los gateways de tipo "GW_INDRA"
                de la tabla [T_GATE] del fichero de configuración del sistema
                "config_sdcxxi_hw.txt":

                - "ID_GW_USU": identificador del Gateway en el que se encuentra la interfaz.
                - "IP": dirección IP del gateway.
                - Listado de tarjetas: para cada tarjeta que contenga el gateway:

                - "ID_ENL": identificador del enlace (de la tabla [T_ENL]).
                - "HUECO": slot que ocupa en el gateway.


                El servidor SAM debe obtener los datos de las tarjetas de los gateways
                de la tabla [T_ENL] del fichero de configuración del sistema
                "config_sdcxxi_hw.txt":

                - "ID": identificador del enlace.
                - "NOMBRE": nombre del enlace.
                - "TIPO": tipo del enlace.
            */

            HW["T_ENL"] = configHw["T_ENL"];

            HW["T_GATE"] = configHw["T_GATE"];

            int nPues = HW["T_PUES"].size();

            /**
             *  5.2.5.2 Posiciones del Sistema
             *
             *  El servidor SAM debe diferenciar las posiciones que forman
             *  parte de una UCS, ya que sobre estas posiciones se podrán activar
             *  más bucles de la UGA (para los Jacks 2 , del coordinador).
             *  [(parámetro de diseño), SAMDP_NumBuclesUGAActivados]
             */

            // Añadir la información de la UCS que corresponde a cada puesto
            for(int i = 1; i < HW["T_UCS"].size(); ++i)
            {
                JVariant ucs = HW["T_UCS"][i];
                if(ucs)
                {
                    int ej = ucs["ID_PUE_EJ"];
                    int pl = ucs["ID_PUE_PL"];

                    if(ej > 0 && ej < nPues) {
                        HW["T_PUES"][ej]["UCS_EJ"] = i;
                    }
                    if(pl > 0 && pl < nPues) {
                        HW["T_PUES"][pl]["UCS_PL"] = i;
                    }
                }
            }

            static QString strPUES("PUES");

            // Almacenar los equipos ordenados por IP
            for(int i = 1; i < HW["T_PUES"].size(); ++i)
            {
                JVariant pues = HW["T_PUES"][i];
                if(pues && pues["IP"])
                {
                    JVariant & eq = HW["T_EQIP"][pues["IP"]];
                    eq["ID"] = i;
                    eq["TIPO"] = strPUES;
                }
            }

            static QString strGATE("GATE");

            for(int i = 1; i < HW["T_GATE"].size(); ++i)
            {
                JVariant gate = HW["T_GATE"][i];
                if(gate && gate["IP"])
                {
                    JVariant & eq = HW["T_EQIP"][gate["IP"]];
                    eq["ID"] = i;
                    eq["TIPO"] = strGATE;
                }
            }

            static QString strSUPER("SUPER");

            for(int i = 1; i < HW["T_SUPER"].size(); ++i)
            {
                JVariant super = HW["T_SUPER"][i];
                if(super && super["IFZ_VOIP1"])
                {
                    JVariant & eq = HW["T_EQIP"][super["IFZ_VOIP1"]];
                    eq["ID"] = i;
                    eq["TIPO"] = strSUPER;
                }
            }

            emit appSettingsChanged(HW, QStringList() << "CONFIG" << "HW");

            configureHostsPings();
        }
    }

    void MainTask::loadConfigFiles()
    {
        TRACE(LOCAL);
        QString configPath = appSettings()[CONF_LOCAL]["SETTINGS"]["CONFIG_PATH"];
        if(!configPath.isEmpty())
        {
            QString configHwPath(QString("%1/config_sdcxxi_hw.txt.zip")
                                 .arg(configPath));

            loadHwConfigFile(configHwPath);
        }
    }

    void MainTask::findSuitableIpAddress()
    {
        TRACE(COMS);

        const JVariant & T_SUPER = appSettings()["CONFIG"]["HW"]["T_SUPER"][1];
        QString IFZ_VOIP1 = T_SUPER["IFZ_VOIP1"]; // Obtener la IP del TMCS
        QPair<QHostAddress, int> subnetTMCS = QHostAddress::parseSubnet(IFZ_VOIP1 + "/24");
        QString ipAddress, bcAddress;

        QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

        bool bDone = false;
        for(int nInterface = 0; nInterface < interfaces.size() && !bDone; ++nInterface)
        {
            const QNetworkInterface & iface = interfaces.at(nInterface);

            QList<QNetworkAddressEntry>	addresses = iface.addressEntries();

            // Utiliza la primera dirección IPv4 que no sea localhost
            for (int nAddress = 0; nAddress < addresses.size() && !bDone; ++nAddress)
            {
                const QNetworkAddressEntry & address = addresses.at(nAddress);

                QHostAddress addressIp = address.ip();

                if (addressIp != QHostAddress::LocalHost && addressIp.toIPv4Address())
                {
                    ipAddress = addressIp.toString();
                    bcAddress = address.broadcast().toString();

                    if(addressIp.isInSubnet(subnetTMCS)) {
                        bDone = true;
                    }
                }
            }
        }

        // Si no se encontró ninguna, usa el localhost IPv4
        if (ipAddress.isEmpty()) {
            ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
            bcAddress = QHostAddress(QHostAddress::Broadcast).toString();
        }

        m_suitableIpAddress = ipAddress;
        m_broadcastIpAddress = bcAddress;

        LOG_INFO(COMS) << "El Servidor SAM se ejecuta en" >> m_suitableIpAddress << "con broadcast"
                       >> m_broadcastIpAddress;
    }

    void MainTask::run()
    {
        TRACE(LOCAL);

        initCurrentlyUsedCPU();
        initCurrentlyUsedCPUByCurrentProcess();

        initSettings();

        parseArguments();

        cerr << QString("«" SAM_TARGET " v" SAM_VERSION "»") << endl;

        JVariant local = appSettings()[CONF_LOCAL]; // Hacer una copia de solo lectura.

        QString iniPath = local["SETTINGS"]["INI_PATH"];

        if(!iniPath.isEmpty())
        {
            // Se analiza el archivo de configuración y se mezcla con el actual
            JVariant iniFile = JsonUtil::parseScvFile(iniPath);
            JVariant settings = appSettings();
            settings |= iniFile; // Mezcla los objetos sobreescribiendo los nuevos
            local = settings[CONF_LOCAL];
            setAppSettings(settings);
        }

        // Comprueba que esta es la única instancia del programa en ejecución.
        // http://www.qtcentre.org/wiki/index.php?title=SingleApplication
        QSharedMemory sharedMemory;

        sharedMemory.setKey(SAM_TARGET);

        if(!sharedMemory.create(1))
        {
            cerr << QString("Ya se encuentra en ejecución otra instancia de " SAM_TARGET) << endl;

            if(local["SETTINGS"]["UNLIMITED_INSTANCES"].toBool() == false)
            {
                // No se ha podido crear la memoria compartida porque otro proceso ya lo hizo
                LOG_ERROR(SERV)  << "No se puede iniciar otra instancia de este programa: "
                                 << sharedMemory.errorString();

                quit(TERMINATE_DELAY);

                return;
            }
        }

        loadConfigFiles();

        findSuitableIpAddress();

        applySettings(appSettings());

        // Se registra a sí mismo
        registerCommandObject(this);

        Testing test;
        if(!test.test())
        {
            emit quit();
            return;
        }

        // Crea el objeto que procesará los mensajes de SAM
        m_pMessageProcessor = new MessageProcessor;
        m_pMessageProcessor->setObjectName("SAM_MessageProcessor");

        connect(m_pMessageProcessor, SIGNAL(fileWritten(QString)),
                this, SLOT(loadHwConfigFile(QString)));

        registerCommandObject(m_pMessageProcessor)->start();

        // Crea el objeto que Esnifará mensajes de la Red
        m_pSniffer = new Sniffer;
        m_pSniffer->setObjectName("SAM_Sniffer");

        registerCommandObject(m_pSniffer)->start();

        // Establece el puerto real de comunicaciones
        ushort portEstad = local["SETTINGS"]["PUERTO_ESTADISTICAS_LOCAL"];

        if(portEstad) {
            registerRemoteServer("ESTAD", QUrl(QString("udp://127.0.0.1:%1").arg(portEstad)));
        } else {
            LOG_ALERT(SERV) << "No se ha configurado un puerto de estadísticas válido.";
        }

        sendCommandToObjects("maxbuffercount 1000", "..p://.*");

        // Realiza labores de mantenimiento de inicio
        maintenance();

        // Petición para recibir los archivos de configuración
        QMetaObject::invokeMethod( m_pMessageProcessor, "requestTMCSConfigurations", Qt::QueuedConnection );
    }

    void MainTask::sendPacketToRemoteEstadServers(Packet packet)
    {
        emit sendPacketToEstadServers(packet);
    }

    void MainTask::sendPacketToRemoteSageServers(Packet packet)
    {
        emit sendPacketToSageServers(packet);
    }

    void MainTask::sendPacketToRemoteControlServers(Packet packet)
    {
        emit sendPacketToControlServers(packet);
    }

    void MainTask::sendPacketToRemoteSupervisionServers(Packet packet)
    {
        emit sendPacketToSupervisionServers(packet);
    }

    void MainTask::sendPacketToRemotePatronServers(Packet packet)
    {
        emit sendPacketToPatronServers(packet);
    }

    void MainTask::delayedQuit(int msec)
    {
        TRACE(LOCAL);

        // Programar un quit() al futuro
        QTimer::singleShot(msec, this, SLOT(quit()));
    }

    void MainTask::quit(int msec)
    {
        TRACE(LOCAL);

        if(msec <= 0) {
            LOG_INFO(COMS) <<  "Quit - Finalizando " SAM_TARGET " ahora.";
        } else {
            LOG_INFO(COMS) <<  "Quit - Programando la finalización de "
                               SAM_TARGET " en" << msec << "ms.";
        }

        m_pSniffer = NULL; // No permitir usar el sniffer a partir de aquí

        if(m_pMessageProcessor)
        {
            QThread * pMessageProcessorThread = m_pMessageProcessor->thread();
            m_pMessageProcessor = NULL; // No permitir su uso a partir de aquí

            emit terminated();

            // Esperar al hilo MessageProcessor que suele ser el más "lento" en terminar.
            if(!pMessageProcessorThread->wait(200)) {
                LOG_TRACE(LOCAL) << "El hilo SAM_MessageProcessor no devuelve el control para terminar "
                                    "la aplicación por lo que el comportamiento será indeterminado.";
            }
        }
        else
        {
            emit terminated();
        }

        if(msec <= 0) {
            emit finished();
        } else {
            // Espera msec a que otros hilos terminen
            QTimer::singleShot(msec, this, SLOT(quit()));
        }
    }

    void MainTask::maintenance()
    {
        TRACE(LOCAL);

        QString logDir;
        int livingDays = -30;

        JVariant trazas = appSettings()["LOCAL"]["TRAZAS"];
        QString fichTrazas = trazas["FICH_TRAZAS"];

        QFileInfo logInfo(fichTrazas);

        logDir = logInfo.dir().path();

        livingDays = -trazas["LIVING_DAYS"];

        QDateTime deleteThreshold(QDateTime::currentDateTime().addDays(livingDays));

        // Borra los archivos de log viejos que hayan sido comprimidos
        deleteOldFilesAsync(logDir,
                            deleteThreshold,
                            QStringList() << "*.log.gz" << "*.log.bz" << "*.log.zip");

        // Calcula los milisegundos hasta mañana a las 03:33AM
        //  y programa un nuevo mantenimiento.
        QDateTime dt(QDateTime::currentDateTime());
        dt = dt.addDays(1);
        dt.setTime(QTime(3,33,33,333)); // Una hora de madrugada
        qint64 msecs = qAbs(dt.msecsTo(QDateTime::currentDateTime()));

        QTimer::singleShot(msecs, this, SLOT(maintenance()));

        QTime span(0, 0);
        span = span.addMSecs((int)msecs);

        LOG_INFO(LOCAL) << "Realizadas las labores de mantenimiento. "
                           "Han sido reprogramadas para" >> dt.toString(g_dateTimeFormat)
                        << ". Faltan" >> span.toString() << "horas.";
    }

    void MainTask::info(QString who, QString what, QVariant data)
    {
        TRACE(LOCAL);

        if(who.isEmpty() || what.isEmpty()) {
            LOG_ALERT_LN(LOCAL) << "Se ha recibido una petición de información "
                                   "con parámetros incorrectos." << who << ':' << what;
            return;
        }

        QStringList path;
        if(what == "created")
        {
            path << "INFO" << "runningObjects" << who << "info" << "createdDateTime";

            // Registrar éste objeto en la lista de objetos supervisados
            appSettings()[path] =
                    QDateTime::currentDateTime().toString(datetimeFormat());
        }
        else if(what == "finished")
        {
            path << "INFO" << "runningObjects";

            // Elimina éste objeto en la lista de objetos supervisados
            JVariant & runningObjects = appSettings()[path];
            runningObjects.remove(who);
        }
        else if(what == "rename")
        {
            path << "INFO" << "runningObjects";

            // Renombra éste objeto en la lista de objetos supervisados
            JVariant & runningObjects = appSettings()[path];
            JVariant old = runningObjects[who];
            runningObjects.remove(who);
            runningObjects[data.toString()] = old;
        }
        else if(what == "info")
        {
            path << "INFO" << "runningObjects" << who << what;

            // Actualiza la información del objeto
            appSettings()[path] = data;
        }

        if(!path.isEmpty()) {
            emit appSettingsChanged(appSettings()[path], path);
        }
    }

    void MainTask::error(QString err)
    {
        LOG_ERROR(COMS) <<  err;
    }

    bool MainTask::processCommand(Packet packetCommand, QByteArray & response)
    {
        TRACE(LOCAL);

        QString fullCommand(packetCommand.byteArray());
        fullCommand = fullCommand.trimmed();

        QString command(fullCommand.section(' ',0,0, QString::SectionSkipEmpty));
        QString args(fullCommand.section(' ',1).trimmed());

        LOG_INFO(COMS) << "Recibida la orden" >> command << "desde "
                       >> packetCommand.origin() << ".";

        QTextStream stResponse(&response, QIODevice::WriteOnly);

        if(command == "quit")
        {
            int delay = 0;

            if(!args.isEmpty())
            {
                bool ok;
                double secs = args.toDouble(&ok);

                if(ok)
                    delay = int(secs*1000.0);
            }

            stResponse << "Cerrando " SAM_TARGET;

            if(delay <= 0)
            {
                stResponse << " ahora ...\n"; // …
                delay = 0;

                // Llamar a quit() forzando que se realice una conexión en cola ya que
                // `emit quit(delay);` Realiza una llamada directa y eso no interesa.
                QMetaObject::invokeMethod( this, "quit", Qt::QueuedConnection,
                                           Q_ARG( int, TERMINATE_DELAY ) );
            }
            else
            {
                stResponse << " en " << delay << "ms ...\n";

                // Llamar a delayedQuit() forzando que se realice una conexión en cola.
                QMetaObject::invokeMethod( this, "delayedQuit", Qt::QueuedConnection,
                                           Q_ARG( int, delay ) );
            }
        }
        else if(command == "info")
        {
            sendCommandToObjects("info", ".*");
        }
        else if(command == "settings")
        {
            QStringList path; ///< Ruta dentro de la base de datos de configuraciones.
            QStringList list(args.split('=', QString::SkipEmptyParts));

            if(list.size() > 0)
                path = list[0].trimmed().split('/', QString::SkipEmptyParts);

            if(list.size() > 1) // Se ha especificado una asignación
            {
                QString strValue(list[1].trimmed());

                JVariant value(JsonUtil::parseJson(strValue));

                appSettings()[path] = value;

                stResponse << value.toJson();

                emit appSettingsChanged(appSettings()[path], path);
            }
            else
            {
                // Tan solo se requiere una lectura de la configuración
                sendCommandToObjects("info", ".*");

                // Espera a que se reciban los resultados
                //SleeperThread::msleep(400);

                stResponse << appSettings()[path].toJson();
            }
        }
        else if(command == "requestconfig")
        {
            // Petición para recibir los archivos de configuración
            if(m_pMessageProcessor) {
                QMetaObject::invokeMethod( m_pMessageProcessor,
                                           "requestTMCSConfigurations", Qt::QueuedConnection );
                stResponse << "Requeridos los archivos de configuración al TMCS "
                              "(Configuration files requested to TMCS)...";
            }
        }

        // Añadir una entrada al log interno de comandos
        JVariant log;

        log["DATETIME"] = packetCommand.dateTime().toString(datetimeFormat());
        log["ORIGIN"] = packetCommand.origin();
        log["COMAND"] = fullCommand;

        QStringList path;
        path << "LOGS" << "COMMANDS";
        QList<QVariant> & logs = appSettings()[path];

        logs << log;

        // Limitar el crecimiento a 10 elementos
        while(logs.size() > 10)
            logs.removeFirst();

        emit appSettingsChanged(logs, path);

        return true;
    }

    QVariant MainTask::memoryInfo() const
    {
        JVariant info;

        sg_mem_stats mem_stat;
        sg_get_mem_stats(&mem_stat);

        //long long totalSystemMemory = getTotalSystemMemory();

        long long totalVirtualMem, virtualMemUsed, totalPhysMem, physMemUsed;
        GlobalMemory(totalVirtualMem, virtualMemUsed, totalPhysMem, physMemUsed);

        long long virtualMemUsedByMe, physMemUsedByMe;
        ProcessMemory(virtualMemUsedByMe, physMemUsedByMe);

        //info["memoryStats"]["totalSystemMemoryMB"] =  B_TO_MB(totalSystemMemory);
        //info["globalMemory"]["totalMB"] = B_TO_MB(mem_stat.total);
        //info["globalMemory"]["usedMB"] = B_TO_MB(mem_stat.used);
        info["globalMemory"]["freeMB"] = B_TO_MB(mem_stat.free);
        info["globalMemory"]["cacheMB"] = B_TO_MB(mem_stat.cache);

        info["globalMemory"]["totalVirtualMemMB"] = B_TO_MB(totalVirtualMem);
        info["globalMemory"]["virtualMemUsedMB"] = B_TO_MB(virtualMemUsed);
        info["globalMemory"]["totalPhysMemMB"] = B_TO_MB(totalPhysMem);
        info["globalMemory"]["physMemUsedMB"] = B_TO_MB(physMemUsed);

        info["processMemory"]["virtualMemUsedMB"] = B_TO_MB(virtualMemUsedByMe);
        info["processMemory"]["physMemUsedMB"] = B_TO_MB(physMemUsedByMe);

        return info;
    }

    QVariant MainTask::cpuInfo() const
    {
        JVariant info;

        static CPUInfo cpuInfo;

        info["cpuInfo"] = cpuInfo.toVariant();
        info["cpuInfo"]["numProcessors"] = getNumProcessors();
        info["cpuInfo"]["idealThreadCount"] = QThread::idealThreadCount(); // Normalmente es igual a numProcessors

        double dUserTime, dKernelTime;
        getCPUTime(dUserTime, dKernelTime);

        //double cpu;
        //calling_thread_cpu_time(cpu);

        double currentlyUsedCPUPercentage = getCurrentlyUsedCPUPercentage();
        double currentlyUsedCPUByCurrentProcessPercentage = getCurrentlyUsedCPUByCurrentProcessPercentage();

        // Todos los valores están en segundos
        info["cpuTimes"]["userTimeInSeconds"] = dUserTime;
        info["cpuTimes"]["kernelTimeInSeconds"] = dKernelTime;
        //info["cpuTimes"]["mainThreadTimeInSeconds"] = cpu;
        info["cpuTimes"]["currentlyUsedCPUPercentage"] = currentlyUsedCPUPercentage;
        info["cpuTimes"]["currentlyUsedCPUByCurrentProcessPercentage"] = currentlyUsedCPUByCurrentProcessPercentage;

        return info;
    }
}
