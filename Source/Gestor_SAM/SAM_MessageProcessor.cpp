#include <QHostAddress>
#include <QMapIterator>
#include <QStateMachine>
#include <QUrl>
#include <QDir>
#include <QTimer>

#include "qtiocompressor.h"
#include "SAM_NetworkOrder.h"
#include "SAM_Declaration.h"
#include "SAM_Helpers.h"
#include "SAM_DataBase.h"
#include "SAM_Archive.h"
#include "SAM_Log.h"
#include "SAM_MessageProcessorPrivate.h"
#include "SAM_MessageProcessor.h"
#include "SAM_Declaration.h"
#include "SAM_TcpUdpClient.h"
#include "SAM_MainTask.h"


namespace SAM
{

    using namespace SAM_Declaration;

    MessageProcessor::MessageProcessor(QObject *parent)
        : CommandObject(parent, -1)
    {
        TRACE(LOCAL);

        MessageProcessorPrivate * const d = new MessageProcessorPrivate();
        m_ptr.reset(d);

        d->m_stateMachine = new StateMachine(d->m_settings, this);
        d->m_stateMachine->setObjectName("SAM_StateMachine");
        g_pMainTask->registerLocalCommandObject(d->m_stateMachine);

        d->m_timerPresWatchdog = new QTimer(this);
        d->m_timerPresEstad = new QTimer(this);

        connect(d->m_timerPresWatchdog, SIGNAL(timeout()), this, SLOT(sendWatchdogPingOrStop()));
        connect(d->m_timerPresEstad, SIGNAL(timeout()), this, SLOT(sendStatisticsPres()));

        // Crea entradas vacías para lo que va a contener la configuración del objeto
        settings()["numMessagesSent"].asMap();
        settings()["mensajes"].asMap(); // Copia de los últimos mensajes recibidos ordenados por IP
    }

    void MessageProcessor::setAppSettings(QVariant settings, QStringList path)
    {
        TRACE(LOCAL);

        CommandObject::setAppSettings(settings, path);

        static const QString strThisPath("CONFIG/HW/T_SUPER");
        QString strPath = path.join("/");

        if(strThisPath.startsWith(strPath)) {
            parseAppSettingsData();
        }
    }

    void MessageProcessor::init()
    {
        TRACE(LOCAL);
    }

    void MessageProcessor::requestTMCSConfigurations(QString origin)
    {
        TRACE(LOCAL);

        if(origin.isEmpty()) {
            origin = objectName();
        }

        QString suitableIpAddress = g_pMainTask->suitableIpAddress();

        JVariant T_SUPER = appSettings()["CONFIG"]["HW"]["T_SUPER"][1];
        QString IFZ_VOIP1 = T_SUPER["IFZ_VOIP1"]; // Obtener la IP del TMCS
        quint16 PUERTO_VOIP_CONTROL = T_SUPER["PUERTO_VOIP_CONTROL"];

        unsigned short lng = sizeof(CAB_MSG_TH) + sizeof(MSG_CONFIG_SESION);

        uint & nMessage = settings()["numMessagesSent"]["M_INICIO_SESION"].asUInt();
        ++nMessage;

        // Crea el mensaje en BigEndian
        MSG_TH msg;
        memset(&msg, 0, sizeof(msg));

        msg.cab.lng = hton(lng);
        msg.cab.tipoMsg = PETI; // (E_TIP_MSG)
        msg.cab.idMsg = M_INICIO_SESION; // (E_ID_MSG)
        msg.cab.idOrigen.id = hton(1U);
        msg.cab.idOrigen.tipoEq = EQ_EQUI_GEN;
        msg.cab.dirOrigen.s_addr = inet_addr(suitableIpAddress);
        msg.cab.idDestino.tipoEq = EQ_SUPERV;
        msg.cab.dirDestino.s_addr = inet_addr(IFZ_VOIP1);
        msg.cab.portDestino = hton((unsigned short)PUERTO_VOIP_CONTROL);
        msg.cab.idDestino.id = 0;
        msg.cab.numMensaje = hton(nMessage);
        msg.sesion.numSesion = 0;
        msg.sesion.infCrc.supFichConfHw.id = 0;
        msg.sesion.infCrc.supFichConfHw.subId = 0;
        msg.sesion.infCrc.supFichConfLog.id = 0;
        msg.sesion.infCrc.supFichConfLog.subId = 0;
        msg.sesion.infCrc.supFichConfLogSectA.id = 0;
        msg.sesion.infCrc.supFichConfLogSectA.subId = 0;
        msg.sesion.infCrc.supFichConfLogSectB.id = 0;
        msg.sesion.infCrc.supFichConfLogSectB.subId = 0;

        // Empaqueta el mensaje
        Packet packet(QByteArray((const char *)&msg, lng), origin);

        LOG_INFO(COMS) << "Envío del mensaje M_INICIO_SESION #" << nMessage << "a CONTROL";

        // Enviar el mensaje a Control (normalmente udp://225.31.0.1:7003)
        g_pMainTask->sendPacketToRemoteControlServers(packet);

        while(false)
        {
            msg.cab.idMsg = M_MODIFICA_PARAMETROS;
            msg.mModificaParametros.idSesion = hton(1234U);
            msg.mModificaParametros.numParametros = 3;
            msg.mModificaParametros.parametros[0] = 61;
            msg.mModificaParametros.parametros[1] = 62;
            msg.mModificaParametros.parametros[2] = 63;
            msg.mModificaParametros.parametros[3] = 64;
            msg.mModificaParametros.parametros[4] = 65;

            OutJsonArchive outBinJson;
            serialize(outBinJson, msg);

            LOG_INFO(COMS) << "MSG_TH =" << outBinJson.data().toJson();

            MSG_TH msg2;
            memset(&msg2, 0, sizeof(msg2));

            InJsonArchive inBinJson(outBinJson.data());
            serialize(inBinJson, msg2);

            continue;
        }
    }

    MessageProcessor::~MessageProcessor()
    {
        TRACE(LOCAL);
    }

    JVariant & MessageProcessor::settings()
    {
        Q_D(MessageProcessor);

        return d->settings();
    }

    const JVariant & MessageProcessor::constSettings() const
    {
        Q_D(const MessageProcessor);

        return d->constSettings();
    }

    Battery * MessageProcessor::getBattery(uint idSesion, uint idBateria)
    {
        return samDataBase().getBattery(idSesion, idBateria);
    }

    const SAM_DataBase & MessageProcessor::samDataBase() const
    {
        TRACE(LOCAL);

        Q_D(const MessageProcessor);

        return d->m_stateMachine->samDataBase();
    }

    SAM_DataBase & MessageProcessor::samDataBase()
    {
        TRACE(LOCAL);

        Q_D(MessageProcessor);

        return d->m_stateMachine->samDataBase();
    }

    QVariant MessageProcessor::samStateMachineInfo() const
    {
        Q_D(const MessageProcessor);

        return d->m_stateMachine->samStateMachineInfo();
    }

    void MessageProcessor::parseAppSettingsData()
    {
        TRACE(LOCAL);

        Q_D(MessageProcessor);

        // Preparar los eventos de envío de `ping`.
        JVariant T_SUPER = appSettingsConst()["CONFIG"]["HW"]["T_SUPER"][1];

        if(T_SUPER)
        {
            // Configurar el temporizador del Watchdog
            /**
             *  5.2.5.6 Parámetros del Watchdog
             *
             *  El servidor SAM debe obtener la los siguientes parámetros del Watchdog
             *  de la tabla [T_SUPER] del fichero de configuración config_sdcxxi_hw.txt
             *  - ENVIO_PING_WATCHDOG: SI / NO
             *  - TMP_PING_WATCHDOG: tiempo entre mensajes de ping (ms)
             *  - PUERTO_WATCHDOG: puerto local al que se envían los mensajes de ping.
             */
            ushort puertoWatchdog = T_SUPER["PUERTO_WATCHDOG"];
            bool bPingWatchdog = T_SUPER["ENVIO_PING_WATCHDOG"];
            int msWatchdog = T_SUPER["TMP_PING_WATCHDOG"];
            msWatchdog *= 1000;

            if(bPingWatchdog && puertoWatchdog && msWatchdog > 0) {
                if(d->m_timerPresWatchdog->interval() != msWatchdog){
                    d->m_timerPresWatchdog->start(msWatchdog);

                    LOG_INFO(LOCAL) << "Configurado el temporizador para las presencias del Watchdog cada"
                                    << msWatchdog << "ms.";
                }
            } else {
                // En caso de que algo no sea propicio, deshabilita la notificación de presencia
                d->m_timerPresWatchdog->stop();

                LOG_INFO(LOCAL) << "Detenido el temporizador para las presencias del Watchdog: "
                                   "ENVIO_PING_WATCHDOG =" << bPingWatchdog << ", PUERTO_WATCHDOG ="
                                << puertoWatchdog << ", TMP_PING_WATCHDOG =" << msWatchdog / 1000;
            }

            // Configurar el temporizador de Estadísticas
            /**
             *  5.2.1 REQUISITOS GENERALES DEL SERVIDOR SAM
             *
             *  El servidor SAM debe enviar presencias (EvtSAM_Presencia) a la dirección
             *  multicast MCAST_VOIP1 de manera periódica según el intervalo definido en la
             *  configuración del sistema (parámetro INTERVALO_PRESENCIAS, en milisegundos).
             */
            QString ipPres = T_SUPER["MCAST_VOIP1"];
            int msEstad = T_SUPER["INTERVALO_PRESENCIAS"];

            if(!ipPres.isEmpty() && msEstad > 0) {
                if(d->m_timerPresEstad->interval() != msEstad) {
                    d->m_timerPresEstad->start(msEstad);

                    LOG_INFO(LOCAL) << "Configurado el temporizador para las presencias de Estadísticas cada"
                                    << msEstad << "ms.";
                }
            } else {
                // En caso de que algo no sea propicio, deshabilita la notificación de presencia
                d->m_timerPresEstad->stop();

                LOG_INFO(LOCAL) << "Detenido el temporizador para las presencias de Estadísticas: "
                                   "MCAST_VOIP1 =" << ipPres << ", INTERVALO_PRESENCIAS =" << msEstad;
            }
        }
    }

    void MessageProcessor::sendWatchdogPingOrStop(bool bSendStop)
    {
        TRACE(LOCAL);

        Q_D(MessageProcessor);

        // Obtener la configuración del Watchdog desde la tabla T_SUPER
        JVariant T_SUPER = appSettingsConst()["CONFIG"]["HW"]["T_SUPER"][1];
        ushort puertoWatchdog = T_SUPER["PUERTO_WATCHDOG"];

        /////////////////////////////////////////////////////////////////////////////////
        // Envía el mensaje de presencia al watchdog
        //
        short lng = sizeof(CAB_MSG_TH) + sizeof(MSG_PRES_WATCHDOG_SAM);

        // Crea el mensaje directamente en BigEndian
        MSG_TH msg, msgME;
        msgME.cab.lng = lng;
        msgME.cab.idMsg = M_PRES_WATCHDOG_SAM;
        msgME.cab.portDestino = 0;
        initResponseMessageBE(msg, msgME);

        msg.cab.tipoMsg = NOTIF;
        msg.cab.dirOrigen.s_addr = inet_addr("127.0.0.1"); // Por especificación del SRD
        msg.cab.idDestino.tipoEq = EQ_WATCHDOG;
        msg.cab.dirDestino.s_addr = inet_addr("127.0.0.1"); // Por especificación del SRD
        msg.cab.portDestino = hton(puertoWatchdog);
        msg.cab.idDestino.id = hton(1UL);

        if(bSendStop) {
            msg.mPresenciaWatchdogSAM.subtipo = M_STOP;
            msg.mPresenciaWatchdogSAM.longId = hton(7UL);
        } else {
            msg.mPresenciaWatchdogSAM.subtipo = M_PING;
            msg.mPresenciaWatchdogSAM.longId = hton(8UL);
        }

        mini_strcpy(msg.mPresenciaWatchdogSAM.proceso, "SAM_SRV");

        // Si hubiera cambiado cambiado el puerto del watchdog, recrear el objeto socket
        if(d->m_pWatchdogClient && d->m_pWatchdogClient->url().port() != puertoWatchdog)
        {
            delete d->m_pWatchdogClient;
            d->m_pWatchdogClient = NULL;
        }

        if(!d->m_pWatchdogClient)
        {
            QString url(QString("udp://127.0.0.1:%1#WATCHOG_CLIENT").arg(puertoWatchdog));
            d->m_pWatchdogClient = new TcpUdpClient(url, this);
            LOG_INFO(LOCAL) << "Creado socket" >> url << "para informar de la presencia al Watchdog.";
            g_pMainTask->registerLocalCommandObject(d->m_pWatchdogClient);
        }

        QByteArray ba((const char *)&msg, ntoh(msg.cab.lng));

        // Escribe el paquete directamente al puerto (sin buffers)
        d->m_pWatchdogClient->sendPacket(ba);

        LOG_INFO(LOCAL) << "Enviada la presencia #" << hton(msg.cab.numMensaje) << "al Watchdog.";
    }

    void MessageProcessor::sendStatisticsPres()
    {
        /**
         *  5.2.1 REQUISITOS GENERALES DEL SERVIDOR SAM
         *
         *  El servidor SAM debe enviar presencias (6.1.1 EvtSAM_Presencia) a la dirección
         *  multicast MCAST_VOIP1 de manera periódica según el intervalo definido en la
         *  configuración del sistema (parámetro INTERVALO_PRESENCIAS, en milisegundos).
         */
        /**
         *  5.2.6 RED
         *
         *  El servidor SAM debe enviar los mensajes de presencia del SAM y los informes
         *  de pruebas al TMCS utilizando el puerto PUERTO_VOIP_SUPERVISION
         *  (parámetro de configuración del sistema), siendo el puerto destino
         *  también el PUERTO_VOIP_SUPERVISION.
         */
        /**
         *  9 REQUISITOS DE DISEÑO
         *
         *  El puerto PUERTO_ESTADISTICAS_LOCAL de comunicación con el agente de estadísticas
         *  debe ser el 7010 y la dirección IP la de loopback (127.0.0.1).
         */

        TRACE(LOCAL);

        Q_D(MessageProcessor);

        /////////////////////////////////////////////////////////////////////////////////
        // Envía el mensaje de presencia a ESTAD tanto a MCAST_VOIP1 como a la loopback 127.0.0.1
        //
        short lng = sizeof(CAB_MSG_TH) + sizeof(MSG_PRES_SAM);

        // Crea el mensaje directamente en BigEndian
        MSG_TH msg, msgME;
        msgME.cab.lng = lng;
        msgME.cab.idMsg = M_PRES_SAM;
        msgME.cab.portDestino = 0;
        initResponseMessageBE(msg, msgME);

        msg.cab.tipoMsg = NOTIF;
        msg.cab.idDestino.tipoEq = EQ_ESTAD;
        msg.cab.dirDestino.s_addr = inet_addr("127.0.0.1"); // Por especificación del SRD
        msg.cab.idDestino.id = msg.cab.idOrigen.id;

        // Buscar el modo de SAM en el nombre del estado seleccionado
        EnumHolder<E_MODO_SAM> modoSAM(d->modoSAM());

        msg.mPresenciaSAM.modoSAM = ntoh(modoSAM);

        /**
         *  6.1.1 EvtSAM_Presencia
         *
         *  El mensaje de presencia del SAM (EvtSAM_Presencia).
         */
        sendToRemoteSupervisionServers(msg, objectName());

        LOG_INFO(LOCAL) << "Enviada la presencia #" << hton(msg.cab.numMensaje)
                        << "a supervisión.";

        /**
         *  6.1.1 EvtSAM_Presencia
         *
         *  El servidor SAM debe enviar también el mensaje de presencia
         *  al agente de estadísticas.
         */
        sendToEstadServers(msg, objectName());

        LOG_INFO(LOCAL) << "Enviada la presencia #" << hton(msg.cab.numMensaje)
                        << "a estadísticas.";
    }

    void MessageProcessor::sendMeasureAdvance()
    {
        /**
         *  6.1.7 EvtSAM_PorcentajeAvanceMedida
         *  El servidor SAM envía un mensaje con el porcentage de avance en la
         *  realización de una batería de pruebas o medidas de red.
         */
        TRACE(LOCAL);

        Q_D(MessageProcessor);

        /////////////////////////////////////////////////////////////////////////////////
        // Envía el mensaje para indicar el porcentaje de avance en la realiación
        // de las medidas de una batería de pruebas o de las medidas de red
        //
        short lng = sizeof(CAB_MSG_TH) + sizeof(uint8);

        // Crea el mensaje directamente en BigEndian
        MSG_TH msg, msgME;
        msgME.cab.lng = lng;
        msgME.cab.idMsg = M_PORCENTAJE_AVANCE_SAM;
        msgME.cab.portDestino = 0;
        initResponseMessageBE(msg, msgME);

        msg.cab.tipoMsg = NOTIF;
        msg.cab.idDestino.tipoEq = EQ_SUPERV;

        float percentage = d->m_stateMachine->getPercentageMeasuresAdvance() * 100.f;

        msg.mAvance = static_cast<unsigned char>(percentage);

        sendToRemoteSupervisionServers(msg, objectName());

        LOG_INFO(LOCAL) << "Enviado a supervisión el porcentaje de avance de las mediciones:"
                        << msg.mAvance << '%';
    }

    Packet MessageProcessor::packetMessageToBigEndian(const MSG_TH & th)
    {
        TRACE(LOCAL);

        // Hacer una copia local de la parte del mensaje que tiene datos válidos
        MSG_TH msgth;
        memcpy(&msgth, &th, qMin(th.cab.lng, (uint16) sizeof(MSG_TH)));

        std::stringstream ssMsgTh;
        OutBinInetArchive outBinAr(ssMsgTh);

        // Pone los endianness de Red (BigEndian)
        outBinAr & AR_SERIALIZATION(msgth);

        std::string str = ssMsgTh.str();
        int size = str.size();
        const MSG_TH * pData = (const MSG_TH *) str.data();

        // Empaqueta el mensaje
        Packet packet(QByteArray((const char *)pData, size));

        return packet;
    }

    void MessageProcessor::parseInterfazBateriaSAM(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        /* Tipo de mensaje para el alta, la modificacion o el borrado de las pruebas
         *  de un interfaz en una bateria de pruebas
         *  [Usado cuando el miembro cab.idMsg es M_INTERFAZ_BATERIA_SAM]
         */
        uint32 idSesion = pMsg->mInterfazBateria.idSesion;
        uint16 idBateria = pMsg->mInterfazBateria.idBateria;

        Battery * battery = getBattery(idSesion, idBateria);

        // Comprueba que la sesión de usuario y la batería existen
        if(!battery)
        {
            LOG_WARN(AUDIO) << "No se puede modificar la batería de pruebas de la "
                               "interfaz porque no existe una sesión de usuario "
                               "con el identificador" << idSesion
                            << "o bien porque su identificador de batería"
                            << idBateria << "no está activo.";
            return;
        }

        JVariant T_EQIP = appSettingsConst()["CONFIG"]["HW"]["T_EQIP"];
        QString ipGateway = T_EQIP[pMsg->mInterfazBateria.idGW]["ID"];

        if(ipGateway.isEmpty()) {
            LOG_WARN(STATE) << "No se puede crear/modificar los parámetros del gateway con ID="
                            << pMsg->mInterfazBateria.idGW
                            << "porque no se encuentra en la configuración actual.";
            return;
        }

        Gateway & gateway = battery->gateways[ipGateway];
        Link & interfaz = gateway.interfaces[pMsg->mInterfazBateria.slot];

        interfaz.id = pMsg->mInterfazBateria.idInterfaz;
        interfaz.slot = pMsg->mInterfazBateria.slot;
        interfaz.tipoInterfaz = pMsg->mInterfazBateria.tipo;

        int numCanales = qMin((int) pMsg->mInterfazBateria.numCanales, MAX_CANALES_INTERFAZ);

        interfaz.canales.clear();

        for(int i = 0; i < numCanales; ++ i)
        {
            const MEDIDAS_CANAL_BUCLE & canal = pMsg->mInterfazBateria.canales[i];

            int numMedidas = qMin((int) canal.numMedidas, MAX_MEDIDAS_CANAL_BUCLE);

            ChannelLoop & canalBucle = interfaz.canales[canal.tipoBucle];
            canalBucle.tipoCanalBucle = canal.tipoBucle;

            for(int m = 0; m < numMedidas; ++ m)
            {
                EnumHolder<E_TIPO_MEDIDA> tipoMedida = canal.medidas[m];

                if(canalBucle.aMedir.contains(tipoMedida)) {
                    LOG_WARN(AUDIO) << "Se ha intentado incorporar a la interfaz" << interfaz.slot
                                    << "del gateway" >> gateway.nombre << " (" << gateway.ip
                                    << ") el tipo de medida" << tipoMedida.toString()
                                    << "que ya existía.";
                } else {
                    canalBucle.aMedir << tipoMedida;
                }
            }
        }
    }

    void MessageProcessor::parseModificaParametros(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        uint32 idSesion = pMsg->mModificaParametros.idSesion;

        SAM_DataBase & estadoSAM = samDataBase();

        if(!estadoSAM.sesionesUsuario.contains(idSesion)) {
            LOG_WARN(AUDIO) << "No se pueden modificar los parámetros del usuario" << idSesion
                            << "porque no existe una sesión con ese identificador";
            return;
        }

        /// @todo Cambiar esta estructura para que se puedan modificar los parámetros.
        //const MSG_MODIFICA_PARAMETROS & parametros = pMsg->mModificaParametros;

        UserSession & sesion = estadoSAM.sesionesUsuario[idSesion];
        sesion.parametros.clear();
        sesion.parametros.reserve(pMsg->mModificaParametros.numParametros);
        qCopy(pMsg->mModificaParametros.parametros,
              pMsg->mModificaParametros.parametros + pMsg->mModificaParametros.numParametros,
              sesion.parametros.begin());
    }

    void MessageProcessor::parsePresencia(const MSG_TH *pMsg)
    {
        //TRACE(LOCAL);

        Q_UNUSED(pMsg)

        // No se hace nada especial porque el mensaje ya ha sido almacenado en settings()["mensajes"]
    }

    void MessageProcessor::parseConsultaParametros(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        //Q_D(MessageProcessor);

        uint32 idSesion = pMsg->mConsultaParametros.idSesion;

        const SAM_DataBase & estado = samDataBase();

        if(!estado.sesionesUsuario.contains(idSesion)) {
            LOG_WARN(AUDIO) << "No se pueden consultar los parámetros del usuario" << idSesion
                            << "porque no existe una sesión con ese identificador";
            return;
        }

        const UserSession & sesion = estado.sesionesUsuario[idSesion];

        /////////////////////////////////////////////////////////////////////////////////
        // Envía el mensaje de respuesta
        //
        short lng = sizeof(CAB_MSG_TH) + sizeof(MSG_MODIFICA_PARAMETROS);

        // Crea el mensaje directamente en BigEndian
        MSG_TH msg, msgME;
        msgME.cab = pMsg->cab;
        msgME.cab.lng = lng;
        msgME.cab.idMsg = M_MODIFICA_PARAMETROS;
        initResponseMessageBE(msg, msgME);

        qCopy(sesion.parametros.begin(),
              sesion.parametros.end(),
              msg.mModificaParametros.parametros);

        sendToRemoteSupervisionServers(msg, objectName());
    }

    void MessageProcessor::sendToEstadServers(const MSG_TH & th, QString origin)
    {
//        Q_D(MessageProcessor);

//        d->sendToEstadServers(th, objectName());
        TRACE(COMS);

        QByteArray ba((const char *)&th, ntoh(th.cab.lng));
        MSG_TH * pTh = (MSG_TH *) ba.data();

        JVariant SETTINGS = g_pMainTask->appSettingsConst()["LOCAL"]["SETTING"];

        // Establece el puerto real de comunicaciones
        uint16 portDestino = SETTINGS["PUERTO_ESTADISTICAS_LOCAL"];
        pTh->cab.portDestino = hton(portDestino);
        pTh->cab.dirOrigen.s_addr = inet_addr("127.0.0.1"); // Por especificación del SRD

        Packet packet(ba, origin);

        LOG_INFO(COMS) << "Envío del mensaje de" << getEnumElemName(th.cab.tipoMsg)
                        << getEnumElemName(th.cab.idMsg) << "a estadísticas.";

        // Enviar el mensaje a estadísticas (normalmente udp://127.0.0.1:7010)
        g_pMainTask->sendPacketToRemoteEstadServers(packet);
    }

    void MessageProcessor::sendToRemoteSupervisionServers(const MSG_TH & th, QString origin)
    {
        TRACE(COMS);

        QByteArray ba((const char *)&th, ntoh(th.cab.lng));
        MSG_TH * pTh = (MSG_TH *) ba.data();

        JVariant T_SUPER = g_pMainTask->appSettingsConst()["CONFIG"]["HW"]["T_SUPER"][1];

        pTh->cab.idDestino.id = 1;
        pTh->cab.idDestino.tipoEq = EQ_SUPERV;

        QString ipTMCS = T_SUPER["IFZ_VOIP1"]; // Obtener la IP del TMCS
        pTh->cab.dirDestino.s_addr = inet_addr(ipTMCS); // Se envía al TMCS

        // Establece el puerto real de comunicaciones
        uint16 portDestino = T_SUPER["PUERTO_VOIP_SUPERVISION"];
        pTh->cab.portDestino = ntoh(portDestino);

        Packet packet(ba, origin);

        LOG_INFO(COMS) << "Envío del mensaje de" << getEnumElemName(th.cab.tipoMsg)
                        << getEnumElemName(th.cab.idMsg) << "a supervisión.";

        // Enviar el mensaje a Supervisión (normalmente udp://225.31.0.1:7002)
        g_pMainTask->sendPacketToRemoteSupervisionServers(packet);
        ;
    }

    void MessageProcessor::sendToRemotePatronServers(const MSG_TH & th, QString origin)
    {
//        Q_D(MessageProcessor);

//        d->sendToRemotePatronServers(th, objectName());
        sendToRemoteSupervisionServers(th, origin);
    }

    QStateMachine & MessageProcessor::stateMachine()
    {
        Q_D(MessageProcessor);

        return d->m_stateMachine->stateMachineInternal();
    }

    void MessageProcessor::quit()
    {
        Q_D(MessageProcessor);

        /**
         *  5.4 WATCHDOG
         *
         *  Si el envío de mensajes de presencia del servidor SAM al watchdog
         *  está habilitado por configuración (parámetro ENVIO_PING_WATCHDOG)
         *  y el usuario cierra la aplicación manualmente, el servidor SAM debe
         *  enviar al watchdog un mensaje de notificación de parada al puerto
         *  UDP PUERTO_WATCHDOG de la máquina local.
         */
        if(d->m_timerPresWatchdog && d->m_timerPresWatchdog->isActive()) {
            sendWatchdogPingOrStop(false); // Envía la señal de stop al watchdog
        }

        CommandObject::quit();
    }

    void MessageProcessor::parseRespActivacionPosicionSAM(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        ResponseEvent::EventType type;
        QString ipEquipo(QHostAddress(pMsg->cab.dirOrigen.s_addr).toString());


        QString motivoRechazo =
                getEnumElemName(pMsg->mRespPosicionMedicion.motivoRechazo);

        JVariant mensaje;
        mensaje["ipEquipo"] = ipEquipo;

        if(pMsg->mRespPosicionMedicion.resultado != OK) {
            type = ResponseEvent::EstadoMedicionPuestoOff;
            mensaje["motivo"] = motivoRechazo;
        } else {
            type = ResponseEvent::EstadoMedicionPuestoOn;
        }

        stateMachine().postEvent(new ResponseEvent(type, mensaje));
    }

    void MessageProcessor::parseRespLlamadaSAM(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        ResponseEvent::EventType type;
        QString ipPatron(QHostAddress(pMsg->cab.dirOrigen.s_addr).toString());

        // Obtener la URL de la posición que ha consultado el Patrón.
        QString ipEquipo(QHostAddress(pMsg->mRespSetLlamadaSAM.destino.s_addr).toString());

        JVariant mensaje;
        mensaje["ipPatron"] = ipPatron;
        mensaje["ipEquipo"] = ipEquipo; // Sólo se usa para corroborar que es el mismo equipo al que se le pidió medir

        if(pMsg->mRespSetLlamadaSAM.estado != ON) {
            type = ResponseEvent::EstadoMedicionPatronOff;
        } else {
            type = ResponseEvent::EstadoMedicionPatronOn;
        }

        stateMachine().postEvent(new ResponseEvent(type, mensaje));
    }


    void MessageProcessor::parseSetBucleSAM(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        ResponseEvent::EventType type;

        // Obtener la URL de la posición que ha consultado el Patrón.
        QString ipEquipo(QHostAddress(pMsg->cab.dirOrigen.s_addr).toString());

        JVariant mensaje;
        mensaje["ipEquipo"] = ipEquipo; // Sólo se usa para corroborar que es el mismo equipo al que se le pidió medir

        QString tipoCanalBucle = getEnumElemName(pMsg->mSetBucle.nbucle);
        mensaje["tipoCanalBucle"] = tipoCanalBucle;

        if(pMsg->mSetBucle.estado != ON) {
            type = ResponseEvent::EstadoBucleOff;
        } else {
            type = ResponseEvent::EstadoBucleOn;
        }

        stateMachine().postEvent(new ResponseEvent(type, mensaje));
    }



    void MessageProcessor::parseRespActivacionPatron(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        ResponseEvent::EventType type;
        QString ipPatron(QHostAddress(pMsg->cab.dirOrigen.s_addr).toString());
        QString motivoRechazo =
                getEnumElemName(pMsg->mRespActivacionPatron.motivoRechazo);

        JVariant mensaje;
        mensaje["ipPatron"] = ipPatron;

        if(pMsg->mRespActivacionPatron.resultado != OK) {
            type = ResponseEvent::EstadoPatronOff;
            mensaje["motivo"] = motivoRechazo;
        } else {
            type = ResponseEvent::EstadoPatronOn;
        }

        stateMachine().postEvent(new ResponseEvent(type, mensaje));
    }

    void MessageProcessor::parseEjecutaBateriaSAM(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        uint32 idSesion = pMsg->mEjecutaBateria.idSesion;
        uint16 idBateria = pMsg->mEjecutaBateria.idBateria;

        JVariant mensaje;
        mensaje["idSesion"] = idSesion;
        mensaje["idBateria"] = idBateria;

        stateMachine().postEvent(new ResponseEvent(
                                     ResponseEvent::EjecutaBateria, mensaje));
    }

    void MessageProcessor::parsePosicionBateriaSAM(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        //JVariant & estado = settings()["estado"];

        uint32 idSesion = pMsg->mPosicionBateria.idSesion;
        uint16 idBateria = pMsg->mPosicionBateria.idBateria;

        Battery * pBattery = getBattery(idSesion, idBateria);

        // Comprueba que la sesión de usuario y la batería existen
        if(!pBattery)
        {
            LOG_WARN(AUDIO) << "No se puede modificar la batería de pruebas de "
                               "la posición porque no existe una sesión "
                               "de usuario con el identificador" << idSesion
                            << "o bien porque su identificador de batería"
                            << idBateria << "no está activo.";
            return;
        }

        uint32 idPosicion = pMsg->mPosicionBateria.idPosicion;
        // Tomar el archivo de configuraciones actual para obtener las IPs de las posiciones
        JVariant CONFIG = appSettingsConst()["CONFIG"];
        JVariant jPUES = CONFIG["HW"]["T_PUES"][idPosicion];

        // Se crea la nueva posición en la batería
        QString ip = jPUES["IP"];
        Position & posicion = pBattery->posiciones[ip];


        // Establecer la IP que le corresponde
        posicion.ip = ip;
        posicion.nombre = jPUES["NOMBRE"].toString();

        int numBucles = qMin((int) pMsg->mPosicionBateria.numBucles, MAX_BUCLES_POSICION);

        posicion.bucles.clear();

        for(int i = 0; i < numBucles; ++ i)
        {
            JVariant jbucle;
            const MEDIDAS_CANAL_BUCLE & bucle = pMsg->mPosicionBateria.bucles[i];
            ChannelLoop & canalBucle = posicion.bucles[bucle.tipoBucle];
            canalBucle.tipoCanalBucle = bucle.tipoBucle;

            canalBucle.aMedir.clear();
            int numMedidas = qMin((int) bucle.numMedidas, MAX_MEDIDAS_CANAL_BUCLE);

            for(int m = 0; m < numMedidas; ++ m)
            {
                EnumHolder<E_TIPO_MEDIDA> tipoMedida = bucle.medidas[m];

                if(!canalBucle.aMedir.contains(tipoMedida))
                    canalBucle.aMedir << tipoMedida;
            }
        }
    }

    void MessageProcessor::parseResultadoBateriaSAM(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        //Q_D(MessageProcessor);

        uint32 idSesion = pMsg->mResultadoBateria.idSesion;
        uint16 idBateria = pMsg->mResultadoBateria.idBateria;

        const Battery * pBattery = getBattery(idSesion, idBateria);

        // Comprueba que la sesión de usuario y la batería existen
        if(!pBattery)
        {
            LOG_WARN(AUDIO) << "No se puede consultar la batería de pruebas porque "
                               "no existe una sesión de usuario con el identificador"
                            << idSesion << "o bien porque su identificador de batería"
                            << idBateria << "no está activo.";
            return;
        }

        /////////////////////////////////////////////////////////////////////////////////
        // Envía los mensajes de respuesta: Uno por cada posición y por cada interfaz
        //

        // Primero envía cada uno de los mensajes de MSG_AUDIO_SAM_POSICION almacenados
        foreach(const Position & posicion, pBattery->posiciones)
        {
            //itPosicion.next();
            //const Position & posicion = itPosicion.value();
            uint16 idPosicion = ntoh(posicion.id);

            short lng = sizeof(CAB_MSG_TH) + sizeof(MSG_AUDIO_SAM_POSICION);

            MSG_TH msg, msgME;
            msgME.cab = pMsg->cab;
            msgME.cab.lng = lng;
            msgME.cab.idMsg = M_AUDIO_SAM_POSICION;
            initResponseMessageBE(msg, msgME);

            msg.mAudioSAMPosicion.id = ntoh(idBateria);
            msg.mAudioSAMPosicion.idConfig = ntoh(pBattery->idConfig);
            msg.mAudioSAMPosicion.subIdConfig = ntoh(pBattery->subIdConfig);
            msg.mAudioSAMPosicion.idCentro = ntoh(pBattery->idCentro);

            QListIterator<DeviceResults> itEjecucion(posicion.medidas);

            while(itEjecucion.hasNext())
            {
                const DeviceResults & ejecucion = itEjecucion.next();

                msg.mAudioSAMPosicion.fecha = toSAM_timebBE(ejecucion.fecha);

                MED_POSICION & medidasPosicion = msg.mAudioSAMPosicion.posicion;

                uint8 numBucles = 0;
                foreach(const ChannelLoopMeasure & medida, ejecucion.medidaBucles)
                {
                    MED_CANAL_BUCLE & medCanalBucle = medidasPosicion.bucle[numBucles++];

                    medCanalBucle.id = medida.tipoCanalBucle;

                    uint8 numMedida = 0;

                    foreach(const MeasureResult & resultado, medida.medidas)
                    {
                        RES_MEDIDA & resMedida = medCanalBucle.medida[numMedida++];

                        resMedida.id = resultado.tipoMedida;
                        resMedida.valor = resultado.valor;
                        resMedida.medida = resultado.clasificacion;
                    }

                    medCanalBucle.numMedidas = numMedida;
                }

                medidasPosicion.numBucles = numBucles;

                if(medidasPosicion.id != idPosicion) {
                    LOG_ALERT_LN(PARSNG) << "No se corresponden los identificadores "
                                         << "de la posiciones:" << ntoh(medidasPosicion.id)
                                         << "!=" << ntoh(idPosicion);
                }

                // Envía un mensaje por cada ejecución que haya tenido cada posición
                sendToRemoteSupervisionServers(msg, objectName()); // A supervisión
                /**
                 *  6.1.2 EvtSAM_ResultadoAudioPosicion
                 *
                 *  El servidor SAM debe enviar un mensaje al servidor TMCS con el
                 *  resultado de las pruebas de audio en una posición.
                 */
                sendToRemoteSupervisionServers(msg, objectName()); // A supervisión

                /**
                 *  6.1.2 EvtSAM_ResultadoAudioPosicion
                 *
                 *  El servidor SAM debe enviar el resultado de las pruebas de audio
                 *  realizadas en una posición al agente de Estadísticas.
                 */
                sendToEstadServers(msg, objectName()); // A estadísticas
            }
        }

        // Primero envía cada uno de los mensajes de MSG_AUDIO_SAM_INTERFAZ almacenados
        foreach(const Gateway & gateway, pBattery->gateways)
        {
            uint16 idGateway = ntoh(gateway.id);
            foreach(const Link & interfaz, gateway.interfaces)
            {
                uint16 idInterfaz = ntoh(interfaz.id);

                short lng = sizeof(CAB_MSG_TH) + sizeof(MSG_AUDIO_SAM_INTERFAZ);

                MSG_TH msg, msgME;
                msgME.cab = pMsg->cab;
                msgME.cab.lng = lng;
                msgME.cab.idMsg = M_AUDIO_SAM_INTERFAZ;
                initResponseMessageBE(msg, msgME);

                msg.mAudioSAMInterfaz.id = ntoh(idBateria);
                msg.mAudioSAMInterfaz.idConfig = ntoh(pBattery->idConfig);
                msg.mAudioSAMInterfaz.subIdConfig = ntoh(pBattery->subIdConfig);
                msg.mAudioSAMInterfaz.idCentro = ntoh(pBattery->idCentro);

                foreach(const DeviceResults & medida, interfaz.medidas)
                {
                    msg.mAudioSAMInterfaz.fecha = toSAM_timebBE(medida.fecha);

                    MED_INTERFAZ & medidasInterfaz = msg.mAudioSAMInterfaz.interfaz;
                    medidasInterfaz.idInterfaz = interfaz.id;
                    medidasInterfaz.idGW = gateway.id;
                    medidasInterfaz.slot = interfaz.slot;
                    medidasInterfaz.tipo = interfaz.tipoInterfaz;
                    medidasInterfaz.resultadoInt = E_RESULTADO_NO_RES; ///< @todo Computar resultadoInt

                    uint8 numCanales  = 0;
                    foreach(const ChannelLoopMeasure & medidaBucle, medida.medidaBucles)
                    {
                        MED_CANAL_BUCLE & medCanalBucle = medidasInterfaz.canal[numCanales];
                        medCanalBucle.id = medidaBucle.tipoCanalBucle;

                        uint8 numMedidas = 0;
                        foreach(const MeasureResult & medida, medidaBucle.medidas)
                        {
                            RES_MEDIDA & resMedida = medCanalBucle.medida[numMedidas++];
                            resMedida.id = medida.tipoMedida;
                            resMedida.valor = medida.valor;
                            resMedida.medida = medida.clasificacion;
                        }

                        medCanalBucle.numMedidas = numMedidas;
                    }
                    medidasInterfaz.numCanales = numCanales;

                    if(medidasInterfaz.idGW != idGateway) {
                        LOG_ALERT_LN(PARSNG) << "No se corresponden los identificadores de "
                                             << "los gateways:" << ntoh(medidasInterfaz.idGW)
                                             << "!=" << ntoh(idGateway);
                    }

                    if(medidasInterfaz.idInterfaz != idInterfaz) {
                        LOG_ALERT_LN(PARSNG) << "No se corresponden los identificadores de los "
                                             << "interfaces:" << ntoh(medidasInterfaz.idInterfaz)
                                             << "!=" << ntoh(idInterfaz);
                    }

                    // Envía un mensaje por cada ejecución que haya tenido cada posición
                    sendToRemoteSupervisionServers(msg, objectName()); // A supervisión
                    /**
                     *  6.1.3 EvtSAM_ResultadoAudioInterfaz
                     *
                     *  El servidor SAM debe enviar un mensaje al servidor TMCS con
                     *  el resultado de las pruebas de audio realizadas en una interfaz.
                     */
                    sendToRemoteSupervisionServers(msg, objectName()); // A supervisión

                    /**
                     *  6.1.3 EvtSAM_ResultadoAudioInterfaz
                     *
                     *  El servidor SAM debe enviar un mensaje al agente de Estadísticas
                     *  con el resultado de las pruebas de audio realizadas en una interfaz.
                     */
                    sendToEstadServers(msg, objectName()); // A estadísticas
                }
            }
        }
    }

    void MessageProcessor::parseConsultaBateriaSAM(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        //Q_D(MessageProcessor);

        uint32 idSesion = pMsg->mConsultaBateria.idSesion;
        uint16 idBateria = pMsg->mConsultaBateria.idBateria;

        const Battery * pBattery = getBattery(idSesion, idBateria);

        // Comprueba que la sesión de usuario y la batería existen
        if(!pBattery)
        {
            LOG_WARN(AUDIO) << "No se puede consultar la batería de pruebas porque no existe "
                               "una sesión de usuario con el identificador" << idSesion
                            << "o bien porque su identificador de batería"
                            << idBateria << "no está activo.";
            return;
        }

        /////////////////////////////////////////////////////////////////////////////////
        // Envía los mensajes de respuesta: Uno por cada posición y por cada interfaz
        //

        // Primero envía cada uno de los mensajes de MSG_POSICION_BATERIA almacenados
        foreach(const Position & position, pBattery->posiciones)
        {
            short lng = sizeof(CAB_MSG_TH) + sizeof(MSG_POSICION_BATERIA);

            MSG_TH msg, msgME;
            msgME.cab = pMsg->cab;
            msgME.cab.lng = lng;
            msgME.cab.idMsg = M_POSICION_BATERIA_SAM;
            initResponseMessageBE(msg, msgME);

            msg.mPosicionBateria.idSesion = ntoh(idSesion);
            msg.mPosicionBateria.idBateria = ntoh(idBateria);
            msg.mPosicionBateria.idPosicion = ntoh(position.id);

            int numBucles = 0;
            foreach(const ChannelLoop & bucle, position.bucles)
            {
                MEDIDAS_CANAL_BUCLE & medidasCanalBucle = msg.mPosicionBateria.bucles[numBucles++];

                medidasCanalBucle.tipoBucle = bucle.tipoCanalBucle;

                int numMedidas = 0;

                foreach(const EnumHolder<E_TIPO_MEDIDA> tipoMedida, bucle.aMedir) {
                    medidasCanalBucle.medidas[numMedidas++] = tipoMedida;
                }

                numMedidas = qMin(numMedidas, MAX_MEDIDAS_CANAL_BUCLE);
                medidasCanalBucle.numMedidas = (uint8) numMedidas;

                // Enviar un mensaje por cada posición
                sendToRemoteSupervisionServers(msg, objectName());
            }

            numBucles = qMin(numBucles, MAX_BUCLES_POSICION);

            msg.mPosicionBateria.numBucles = (uint8) numBucles;
        }

        foreach(const Gateway & gateway, pBattery->gateways)
        {
            foreach(const Link & interfaz, gateway.interfaces)
            {
                short lng = sizeof(CAB_MSG_TH) + sizeof(MSG_INTERFAZ_BATERIA);

                MSG_TH msg, msgME;
                msgME.cab = pMsg->cab;
                msgME.cab.lng = lng;
                msgME.cab.idMsg = M_INTERFAZ_BATERIA_SAM;
                initResponseMessageBE(msg, msgME);

                msg.mInterfazBateria.idSesion = ntoh(idSesion);
                msg.mInterfazBateria.idBateria = ntoh(idBateria);
                msg.mInterfazBateria.idGW = ntoh(gateway.id);
                msg.mInterfazBateria.idInterfaz = ntoh(interfaz.id);
                msg.mInterfazBateria.slot = (uint8) interfaz.slot;

                msg.mInterfazBateria.tipo.data = interfaz.tipoInterfaz;

                int iCanal = 0;
                foreach(const ChannelLoop & canalBucle, interfaz.canales)
                {
                    MEDIDAS_CANAL_BUCLE & medidasCanalBucle
                            = msg.mInterfazBateria.canales[iCanal++];

                    medidasCanalBucle.tipoBucle = canalBucle.tipoCanalBucle;

                    uint8 numMedidas = 0;
                    foreach(const EnumHolder<E_TIPO_MEDIDA> tipoMedida, canalBucle.aMedir) {
                        medidasCanalBucle.medidas[numMedidas++] = tipoMedida;
                    }

                    medidasCanalBucle.numMedidas = numMedidas;
                }

                iCanal = qMin(iCanal, MAX_CANALES_INTERFAZ);

                msg.mInterfazBateria.numCanales = (uint8) iCanal;

                // Enviar un paquete por cada interfaz
                sendToRemoteSupervisionServers(msg, objectName()); // A supervisión

                // sendToEstadServers(msg, objectName()); // A estadísticas
            }
        }
    }

    void MessageProcessor::parseBorraBateriaSAM(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        uint32 idSesion = pMsg->mBorraBateria.idSesion;
        uint16 idBateria = pMsg->mBorraBateria.idBateria;

        Battery * pBattery = getBattery(idSesion, idBateria);

        if(pBattery)
        {
            // Si la sesión de usuario y la batería existen, borra la batería
            QString nombre = pBattery->nombre;
            samDataBase().sesionesUsuario[idSesion].baterias.remove(idBateria);

            LOG_INFO(AUDIO) << "Borrada la batería de pruebas" << nombre
                            << '#' << idBateria;
        }
        else
        {
            LOG_WARN(AUDIO) << "No se puede borrar la batería de pruebas" << idBateria
                            << "porque no existe una sesión de usuario con el identificador"
                            << idSesion << "o bien no existe esa batería";
        }
    }

    void MessageProcessor::parseCreaBateriaSAM(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        Q_D(MessageProcessor);

        uint32 idSesion = pMsg->mCreaBateria.idSesion;

        /**
         * 5.2.2.1 Baterías de Pruebas
         *  El servidor SAM recibe del cliente SAM la petición de realizar las medidas
         *  de audio de una batería de pruebas (BdP).
         */

        if(!samDataBase().sesionesUsuario.contains(idSesion)) // Si la sesión de usuario existe, crea la batería
        {
            LOG_WARN(AUDIO) << "No se puede crear la batería de pruebas porque no existe una "
                               "sesión de usuario con el identificador"<< idSesion;
            return;
        }

        QString nombre = qPrintable(QString::fromLatin1(
                    pMsg->mCreaBateria.nombre, MAX_NOMBRE_BATERIA));

        /**
         * 5.2.2.1 Baterías de Pruebas
         *  El servidor SAM debe impedir que se creen baterías de pruebas
         *  con nombres repetidos.
         */
        UserSession & sesion = samDataBase().sesionesUsuario[idSesion];
        foreach(const Battery & battery, sesion.baterias)
        {
            if(battery.nombre == nombre)
            {
                LOG_WARN(AUDIO) << "No se puede crear una nueva batería de pruebas" >> battery.nombre
                                << "porque ese nombre está ya en uso en la batería" << battery.idBateria;
                return;
            }
        }

        uint32 idBattery = ++d->m_batteryCounter; ///< @todo Serializar este contador

        // Asegurarse de que el contador es mayor que el mayor de los IDs de batería
        if(!sesion.baterias.empty()) {
            uint lastID = sesion.baterias.constEnd().key();

            if(lastID <= idBattery) {
                idBattery = d->m_batteryCounter = lastID + 1;
            }
        }

        qint64 msecs = pMsg->mCreaBateria.fechaCreacion.time * 1000
                + pMsg->mCreaBateria.fechaCreacion.millitm;

        // Se rellena la nueva batería
        Battery & bateria = sesion.baterias[idBattery];

        bateria.idBateria = idBattery;
        bateria.nombre = nombre;
        bateria.creacion = QDateTime::fromMSecsSinceEpoch(msecs); ///< @note Se podría coger de esta misma máquina
        bateria.modificacion = bateria.creacion;
        bateria.idConfig = pMsg->mCreaBateria.idConfig;
        bateria.subIdConfig = pMsg->mCreaBateria.subIdConfig;
        bateria.idCentro = pMsg->mCreaBateria.idCentro;

        QHostAddress urlOrigen(pMsg->cab.dirOrigen.s_addr);
        LOG_INFO(AUDIO) << "El usuario" << sesion.rol << '#' << idSesion
                        << "ha creado la batería" << nombre << '#' << idBattery
                        << "desde" << urlOrigen.toString();

        /////////////////////////////////////////////////////////////////////////////////
        // Envía el mensaje de respuesta
        //
        short lng = sizeof(CAB_MSG_TH) + sizeof(MSG_ID_SESION);

        // Crea el mensaje directamente en BigEndian
        MSG_TH msg, msgME;
        msgME.cab = pMsg->cab;
        msgME.cab.lng = lng;
        msgME.cab.idMsg = M_RESULTADO_BATERIA_SAM;
        initResponseMessageBE(msg, msgME);

        msg.mResultadoBateria.idSesion = hton(idSesion);
        msg.mResultadoBateria.idBateria = hton(idBattery);

        sendToRemoteSupervisionServers(msg, objectName());
    }

    void MessageProcessor::parseFinSesionSAM(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        Q_D(MessageProcessor);

        if(pMsg->mIdSesion == d->m_sessionOpened) {
            LOG_INFO(AUDIO) << "Cerrada la sesión" << pMsg->mIdSesion;
        } else {
            LOG_WARN(AUDIO) << "No se puede cerrar la sesión" << pMsg->mIdSesion
                            << "porque no se encontraba abierta sino " << d->m_sessionOpened;
        }

        d->m_sessionOpened = 0;
    }


    void MessageProcessor::parseInicioSesionSAM(const MSG_TH *pMsg)
    {
        TRACE(COMS);

        Q_D(MessageProcessor);

        if(d->m_sessionOpened) {
            LOG_WARN(STATE) << "Se cierra automáticamente la sesión" << d->m_sessionOpened
                            << "previo a abrir una nueva sesión.";
        }

        /** @todo Elegir un mensaje para la respuesta del inicio de sesión SAM
         *  adecuado. Por ahora se usará el de M_CONSULTA_PARAMETROS.
         */

        QString rolUsuario = qPrintable(QString::fromLatin1(
                                            pMsg->mInicioSesionSAM.rolUsuario, MAX_NOMBRE_ABONADO));

        uint32 idSesion = ++d->m_sessionCounter;
        UserSession & sesion = samDataBase().sesionesUsuario[idSesion];

        sesion.idSesion = idSesion;
        sesion.rol = rolUsuario;
        sesion.creacion = QDateTime::currentDateTime();
        sesion.modificacion = sesion.creacion;

        QHostAddress urlOrigen(pMsg->cab.dirOrigen.s_addr);
        LOG_INFO(AUDIO) << "Creada la sesión de usuario" << idSesion << "de" >> rolUsuario
                        << "desde" >> urlOrigen.toString();

        /////////////////////////////////////////////////////////////////////////////////
        // Envía el mensaje de respuesta
        short lng = sizeof(CAB_MSG_TH) + sizeof(MSG_ID_SESION);

        // Crea en BigEndian el mensaje de respuesta
        MSG_TH msg, msgME;
        msgME.cab = pMsg->cab;
        msgME.cab.lng = lng;
        msgME.cab.idMsg = M_CONSULTA_PARAMETROS;
        initResponseMessageBE(msg, msgME);

        // El único dato que se devuelve en MSG_ID_SESION es el identificador de sesión
        msg.mConsultaParametros.idSesion = hton(idSesion);

        sendToRemoteSupervisionServers(msg, objectName());
    }

    void MessageProcessor::cancelBatteryExecution(const QString & causa)
    {
        TRACE(COMS);

        Q_D(MessageProcessor);

        // Si se está en medio de la ejecución de una batería, la interrumpe
        // lo que provocará que cuando se llegue a reposo se envíe el mensaje
        // 6.1.8 EvtSAM_MedicionCancelada al TMCS.

        d->m_stateMachine->cancelBatteryExecution(causa);
    }

    bool MessageProcessor::processMessage(Packet msgPacket)
    {
        TRACE(LOCAL);

        Q_D(MessageProcessor);

        bool bProcessed = false;

        QUrl url(msgPacket.origin());

        QString originType = url.fragment();

        // Extrae el mensaje SAM contenido en el paquete
        const MSG_TH *pMsg = getSamMessage(msgPacket);

        // Comprueba que contenga un mensaje válido antes de procesarlo
        if(pMsg)
        {
            const char *szIdMsg = "UNKNONW_ID";
            const char *szSrcEqType = "UNKNONW_ID";
            const char *szDstEqType = "UNKNONW_ID";
            E_ID_MSG idMsg = pMsg->cab.idMsg;

            bool bWellFormed = true;
            bWellFormed = bWellFormed && getEnumElemName(&szIdMsg, idMsg);
            bWellFormed = bWellFormed && getEnumElemName(&szSrcEqType, pMsg->cab.idOrigen.tipoEq);
            bWellFormed = bWellFormed && getEnumElemName(&szDstEqType, pMsg->cab.idDestino.tipoEq);

            if(bWellFormed)
            {
                bProcessed = true;
                IPv4 dirOrigen = pMsg->cab.dirOrigen;
                QHostAddress urlOrigen(dirOrigen.s_addr);

                LOG_INFO(COMS) << "Recibido el mensaje" << szIdMsg << "#" << pMsg->cab.numMensaje
                               << "desde" << urlOrigen.toString() << '('
                               << szSrcEqType << ", ID =" << pMsg->cab.idOrigen.id
                               << ") -> (" << szDstEqType << ", ID =" << pMsg->cab.idDestino.id
                               << ") de" << pMsg->cab.lng << "bytes";

                ++d->m_receivedMsgCount;

                // Almacena una copia del último mensaje recibido de los equipos supervisados
                JVariant jMessage = msgPacket.userData();
                SAM_DataBase & state = d->m_stateMachine->samDataBase();

                JVariant & jLast = state.mensajes
                        [urlOrigen.toString()][szIdMsg]["ultimo"];
                jLast["mensaje"] = jMessage;
                jLast["fecha"] = QDateTime::currentDateTime();

                /**
                 *  5.2.2.1.2 Cancelación de una Batería de Pruebas en Ejecución
                 *
                 *  El servidor SAM debe poder realizar la Cancelación de una
                 *  Batería de Pruebas en Ejecución, bajo petición del cliente SAM.
                 *  Para cancelar la ejecución de una batería de pruebas, el servidor
                 *  SAM debe detener el análisis de audio que esté ejecutando, y no
                 *  ejecutar el resto de los análisis que tenga pendientes.
                 *
                 *  Al cancelar la ejecución de una batería de pruebas, el servidor SAM debe:
                 *   - Si hay un bucle activo, desactivar el Bucle.
                 *   - Si hay una llamada SAM establecida, finalizar de la llamada SAM.
                 *   - Si hay una posición o interfaz en modo Medición, desactivar el modo Medición.
                 *   - Desactivar la posición Patrón.
                 */

                // Capta las configuraciones y sectorizaciones aunque no fueran
                //  destinadas al Gestor SAM.
                if(idMsg == M_INF_FICH_CONFIG) {
                    cancelBatteryExecution("Recibido inicio de fichero de configuración M_INF_FICH_CONFIG (INCO)");
                    parseDataInfFichConfig(pMsg);
                } else if(idMsg == M_FICH_CONFIG) {
                    cancelBatteryExecution("Recibido fichero de configuración M_FICH_CONFIG (FICO)");
                    parseDataFichConfig(pMsg);
                } else if(idMsg == M_PRESENCIA) {
                    parsePresencia(pMsg);
                } else {
                    // No se tendrán en cuenta las sectorizaciones: M_ENVIA_TABLA
                    E_TIPO_EQUIPO tipoEqDestino = pMsg->cab.idDestino.tipoEq;

                    if( tipoEqDestino == EQ_SAM
                        || tipoEqDestino == EQ_EQUI_GEN
                        || tipoEqDestino == EQ_NULO) {
                        // El mensaje es para éste procesador de mensajes
                        processMessageSAM(pMsg);
                    } else {
                        bProcessed = false;
                    }
                }
            }
            else
            {
                LOG_WARN(SERV) << "Se ha detectado un mensaje SAM mal formado "
                                  "en campos internos.";
            }
        }
        else
        {
            LOG_WARN(SERV) << "Se ha detectado un mensaje SAM mal formado.";
        }

        if(bProcessed)
            ++d->m_processedMsgCount;

        return bProcessed;
    }

    qint64 MessageProcessor::receivedMsgCount() const
    {
        Q_D(const MessageProcessor);

        return d->receivedMsgCount();
    }

    qint64 MessageProcessor::processedMsgCount() const
    {
        Q_D(const MessageProcessor);

        return d->processedMsgCount();
    }

    qint64 MessageProcessor::rejectedBytesCount() const
    {
        Q_D(const MessageProcessor);

        return d->m_nRejectedBytes;
    }

    qint64 MessageProcessor::receivedBytesCount() const
    {
        Q_D(const MessageProcessor);

        return d->m_nReceivedBytes;
    }

    unsigned int MessageProcessor::sessionCounter() const
    {
        Q_D(const MessageProcessor);

        return d->m_sessionCounter;
    }

    unsigned int MessageProcessor::batteryCounter() const
    {
        Q_D(const MessageProcessor);

        return d->m_batteryCounter;
    }

    unsigned int MessageProcessor::sessionOpened() const
    {
        Q_D(const MessageProcessor);

        return d->m_sessionOpened;
    }

    int MessageProcessor::timerPresWatchdogInterval() const
    {
        Q_D(const MessageProcessor);

        return d->m_timerPresWatchdog ? d->m_timerPresWatchdog->interval() : 0;
    }

    int MessageProcessor::timerPresEstadInterval() const
    {
        Q_D(const MessageProcessor);

        return d->m_timerPresEstad ? d->m_timerPresEstad->interval() : 0;
    }

    qint64 MessageProcessor::packetsWrittenToWatchdog() const
    {
        Q_D(const MessageProcessor);

        return d->m_pWatchdogClient ? d->m_pWatchdogClient->packetsWritten() : 0;
    }

    QString MessageProcessor::modoSAM() const
    {
        Q_D(const MessageProcessor);

        EnumHolder<E_MODO_SAM> modo(d->modoSAM());

        return modo.toString();
    }

    bool MessageProcessor::parseDataFichConfig(const MSG_TH *pMsg)
    {
        TRACE(CONF);

        Q_D(MessageProcessor);

        if(pMsg->cab.idMsg != M_FICH_CONFIG) {
            return false;
        }

        // Obtiene el nombre del fichero con un límite de NMAX_NOMBRE_FICHERO caracteres
        QString nombreFichero(qPrintable(QString::fromLatin1(
                              pMsg->fichConf.fich.nombreFichero, NMAX_NOMBRE_FICHERO)));

        int nReceived = 0; // 0=Se necesitan más bloques, -1=Error, <otro>=Recibido OK
        do
        {
            MessageProcessorPrivate::ReceivingFile & receivingFile
                    = d->m_receivingFiles[nombreFichero];

            if(pMsg->fichConf.fich.bloque != receivingFile.blockCounter + 1) {
                LOG_ERROR(CONF) << "La cuenta del bloque del archivo de configuración ("
                                >> nombreFichero << pMsg->fichConf.fich.bloque
                                << ") no corresponde con la que se esperaba ("
                                << (receivingFile.blockCounter + 1)
                                << "), por lo que éste mensaje será desechado.";
                nReceived = -1;
                break;
            }

            if(receivingFile.data.size() > int(pMsg->fichConf.fich.numBytes + 100)
                    || int(pMsg->fichConf.fich.numBytesBloque) > (1<<23))
            {
                LOG_ERROR(CONF) << "Se evita el procesamiento del archivo de configuración"
                                >> nombreFichero
                                << "excesivamente grande (posible ataque por buffer overrun).";
                nReceived = -1;
                break;
            }

            // Añade los datos de éste bloque
            receivingFile.data.append((const char *)pMsg->fichConf.byte,
                                      pMsg->fichConf.fich.numBytesBloque);
            ++receivingFile.blockCounter;

            LOG_INFO(CONF) << "Añadido el bloque" << pMsg->fichConf.fich.bloque << '/'
                           << pMsg->fichConf.fich.numBloques << "al archivo de configuración"
                           >> nombreFichero;

            bool bCompletado = receivingFile.data.size() >= int(pMsg->fichConf.fich.numBytes);
            if(bCompletado)
            {
                nReceived = receivingFile.data.size();
                LOG_INFO(CONF) << "Se han recibido completamente" << nReceived
                               << "bytes del archivo de configuración" >> nombreFichero;

                // Hacer una copia local en el sistema de archivos
                QString path = appSettings()[CONF_LOCAL]["SETTINGS"]["CONFIG_PATH"];
                if(path.isEmpty()) {
                    path = ".";
                }

                if(path != "." && path != "..") {
                    QDir().mkpath(path); // Asegurarse de que la ruta exista
                }

                QString confPath(QString("%1/%2.zip").arg(path).arg(nombreFichero));

                QFile confFile(confPath);

                if(confFile.open(QFile::WriteOnly))
                {
                    qint64 nWritten = confFile.write(receivingFile.data);
                    if(nWritten > 0) {
                        LOG_INFO(CONF) << "Escritos" << nWritten
                                       << "bytes del archivo de configuración" >> confPath;

                        emit fileWritten(confPath);
                    } else {
                        LOG_ERROR(CONF) << "No se puede escribir en el archivo de configuración"
                                        >> confPath << ":" >> confFile.errorString();
                    }
                }
                else
                {
                    LOG_ERROR(CONF) << "No se puede crear el archivo de configuración"
                                    >> confPath << ":" >> confFile.errorString();
                }

                // Libera los recursos que se habían empleado para reconstruir el archivo
                receivingFile.clear();
            }
        } while(false);

        // Envío del mensaje de confirmación de recepción de los archivos de configuración.
        if(nReceived != 0)
        {
            /**
             *  6.1.5 EvtSAM_ACKFicheroConfig
             *
             *  El servidor SAM envía un mensaje de confirmación cuando ha recibido
             *  todo el fichero de configuración para que el servidor TMCS pueda
             *  comprobar la correcta recepción.
             */

            /////////////////////////////////////////////////////////////////////////////////
            // Envía el mensaje de confirmación de la recepción del fichero de configuración
            short lng = sizeof(CAB_MSG_TH) + sizeof(MSG_CONFIG_FICH_CONFIG);

            // Crea en BigEndian el mensaje de respuesta
            MSG_TH msg, msgME;
            msgME.cab = pMsg->cab;
            msgME.cab.lng = lng;
            msgME.cab.idMsg = M_FICH_CONFIG;
            initResponseMessageBE(msg, msgME);

            msg.cab.tipoMsg = NOTIF;
            msg.cab.idMsg = M_ACK_FICHERO;

            // Ha habido un error o se ha recibido completamente el archivo
            EnumHolder<E_ESTADO_POSICION> fichCorrecto((nReceived > 0) ? OK : NO_OK);
            msg.fichConf = pMsg->fichConf;
            msg.fichConf.fichCorrecto = fichCorrecto;

            sendToRemoteSupervisionServers(msg, objectName());

            LOG_INFO(CONF) >> "Enviado reconocimiento ( EvtSAM_ACKFicheroConfig ="
                           << fichCorrecto << ") del archivo de configuración"
                           >> nombreFichero;
        }

        return nReceived >= 0; // Devuelve false si hubo error.
    }

    bool MessageProcessor::parseDataInfFichConfig(const MSG_TH *pMsg)
    {
        TRACE(LOCAL);

        Q_D(MessageProcessor);

        if(pMsg->cab.idMsg != M_INF_FICH_CONFIG)
            return false;

        int8 szNombreFichero[NMAX_NOMBRE_FICHERO+1];
        qstrncpy(szNombreFichero, pMsg->infFichConfig.fich.nombreFichero, NMAX_NOMBRE_FICHERO);
        szNombreFichero[NMAX_NOMBRE_FICHERO] = 0;
        QString nombreFichero(szNombreFichero);

        MessageProcessorPrivate::ReceivingFile & receivingFile = d->m_receivingFiles[nombreFichero];

        if(!receivingFile.data.isEmpty()) {
            LOG_ERROR(CONF) << "El archivo de configuración anterior" >> nombreFichero
                            << "no fue completamente analizado";
        }

        if(pMsg->infFichConfig.fich.numBytes > (1<<23)) {
            LOG_ERROR(CONF) << "Se evita el procesamiento de un archivo de configuración"
                            >> nombreFichero << "excesivamente grande";
            return false;
        }

        LOG_INFO(CONF) << "El archivo de configuración" >> nombreFichero << "de"
                       << pMsg->infFichConfig.fich.numBytes << "bytes será recibido partido en"
                       << pMsg->infFichConfig.fich.numBloques << "trozos de"
                       << pMsg->infFichConfig.fich.numBytesBloque << "bytes";

        receivingFile.clear();
        receivingFile.data.reserve(pMsg->infFichConfig.fich.numBytes);

        return true;
    }

    const MSG_TH * MessageProcessor::getSamMessage(const Packet msgPacket)
    {
        TRACE(LOCAL);

        const MSG_TH *ret = 0;
        int size = msgPacket.byteArray().size();

        // Un mensaje SAM ha de ocupar por lo menos 40 bytes (el espacio de la cabecera)
        if(size < 40) {
            return NULL;
        }

        ret = (const MSG_TH *) msgPacket.byteArray().constData();

        bool bRecvId = getEnumElemName( NULL, ret->cab.idMsg );

        // Comprueba que la cabecera contenga un identificador válido
        if(!bRecvId) {
            return NULL;
        }

        // Comprueba que la cabecera tenga un tamaño suficiente
        if(ret->cab.lng > size) {
            return NULL;
        }

        return ret;
    }

    void MessageProcessor::initResponseMessageBE(
            MSG_TH & msgBE,
            const MSG_TH & msgME)
    {
        TRACE(LOCAL);

        uint suitableEqId = g_pMainTask->suitableEqId();

        const char * szElemName = "*INVALID*";

        getEnumElemName(&szElemName, msgME.cab.idMsg);

        uint & nMessage = settings()["numMessagesSent"][szElemName].asUInt();
        ++nMessage;

        memset(&msgBE, 0, sizeof(msgBE));

        msgBE.cab.lng = hton(msgME.cab.lng);
        msgBE.cab.tipoMsg = RESP; // (E_TIP_MSG)
        msgBE.cab.idMsg = msgME.cab.idMsg; // (E_ID_MSG)
        msgBE.cab.idOrigen.id = hton((uint32)suitableEqId);
        msgBE.cab.idOrigen.tipoEq = EQ_SAM;

        if(msgME.cab.portDestino)
        {
            msgBE.cab.dirOrigen.s_addr = msgME.cab.dirDestino.s_addr;
            msgBE.cab.idDestino.tipoEq = msgME.cab.idOrigen.tipoEq;
            msgBE.cab.dirDestino.s_addr = msgME.cab.dirOrigen.s_addr;
            msgBE.cab.portDestino = hton(msgME.cab.portDestino);
            msgBE.cab.idDestino.id = hton(msgME.cab.idOrigen.id);
        }
        else
        {
            QString suitableIpAddress = g_pMainTask->suitableIpAddress();

            msgBE.cab.dirOrigen.s_addr = inet_addr(suitableIpAddress); // inet_addr("127.0.0.1");
            msgBE.cab.idDestino.tipoEq = ntoh(EQ_EQUI_GEN);
            msgBE.cab.dirDestino.s_addr = 0;
            msgBE.cab.portDestino = hton((uint16)7002);
            msgBE.cab.idDestino.id = hton(1UL);
        }

        msgBE.cab.numMensaje = hton(nMessage);
    }

    int MessageProcessor::parseBinaryPacket(Packet packet)
    {
        Q_D(MessageProcessor);

        int nRejectedBytes;
        d->m_nReceivedBytes += packet.byteArray().size();
        int nPackets = parseBinaryPacket(this, packet, &nRejectedBytes);
        d->m_nRejectedBytes += nRejectedBytes;

        return nPackets;
    }

    int MessageProcessor::parseBinaryPacket(QObject * object, Packet packet, int *nRejectedBytes)
    {
        TRACE(LOCAL);

        int nMessagesFound = 0;
        int baSize = packet.byteArray().size();

        std::stringstream ssMsgTh;

        if(nRejectedBytes) {
            *nRejectedBytes = 0;
        }

        const char * pPacketMemory = packet.byteArray().constData();
        ssMsgTh.write(pPacketMemory, baSize);

        const MSG_TH *pMsgBigEndian = (const MSG_TH *) pPacketMemory;
        if(pMsgBigEndian->cab.idOrigen.tipoEq == EQ_SAM)
        {
            LOG_INFO(COMS) << "Desechando un paquete de"
                << baSize << "bytes generado en EQ_SAM desde"
                >> packet.origin();

            if(nRejectedBytes) {
                *nRejectedBytes = baSize;
            }

            return 0; // No contabilizar éste mensaje
        }

        LOG_INFO(COMS) << "Recibido un paquete desde" << packet.origin()
            << "de" << baSize << "bytes";

        // Transforma los endianness desde el protocolo de red al de la máquina.
        InBinInetArchive inBinAr(ssMsgTh);

        int toProcess = sizeof(MSG_TH); // Fijar un límite de iteraciones para buscar el inicio de la trama
        while(!ssMsgTh.eof() && --toProcess)
        {
            const char *szElemName = NULL;
            int begin = ssMsgTh.tellg();
            const MSG_TH *pMsgIn = (const MSG_TH *) (pPacketMemory + begin);

            // Comprueba que esta cabecera contenga un identificador válido del SCV
            bool bRecvId = getEnumElemName( &szElemName, pMsgIn->cab.idMsg );
            if( !bRecvId )
            {
                // Avanzar el cursor 1 byte y volverlo a intentar
                ssMsgTh.seekg(begin + 1, ios::beg);
                if(nRejectedBytes) {
                    ++*nRejectedBytes;
                }

                continue;
            }

            toProcess = sizeof(MSG_TH);

            // Obtiene una representacion en JSON del mensaje
            JVariant jMessage;
            OutJsonArchive inBinJson;
            serialize(inBinJson, *pMsgIn);
            jMessage = inBinJson.data();

            LOG_DEBUG(PARSNG) << "Analizado mensaje SCV: " << jMessage.toJson();

            MSG_TH th;
            memset(&th, 0, sizeof(th));

            // Corrige los endianness y pone los del anfitrión
            inBinAr & AR_SERIALIZATION(th);

            int end = ssMsgTh.tellg();
            int total = end - begin;

            if(total <= 0) // No se ha podido analizar un mensaje completo
                break;

            int limitLng = qMin((size_t) th.cab.lng, sizeof(MSG_TH));
            int remain = limitLng - total;

            if(remain != 0)
            {
                // Se han recibido más bytes de los estrictamente necesarios.
                LOG_DEBUG(PARSNG) << "El mensaje" << szElemName
                                  << "notifica que ocupa" << th.cab.lng
                                  << "bytes aunque en realidad ocupa" << total;

                if(nRejectedBytes && remain > 0) {
                    *nRejectedBytes += remain;
                }
                if(remain > 8 && limitLng == (int) th.cab.lng) {
                    // Avanzar el cursor para coincidir con la siguiente cabecera
                    // Suele haber un desfase de 8 bytes (error incorregible del SCV).
                    ssMsgTh.seekg(end + remain - 8, ios::beg);
                }
            }

            // Sobreescribir con el tamaño adecuado
            th.cab.lng = total;

            // Preparar un paquete con una sola estructura con el Endian del anfitrión
            Packet msgPacket(QByteArray((const char*) &th, total), packet.origin());

            // Pasar también una copia del mensaje en formato JSON.
            msgPacket.setUserData(jMessage);

            // Encolar el mensaje para ser procesado asíncronamente
            // en lugar de llamarlo directamente con `processMessage(msgPacket)`
            QMetaObject::invokeMethod( object, "processMessage",
                                       Qt::QueuedConnection,
                                       Q_ARG( Packet, msgPacket ) );

            ++nMessagesFound;
        }

        return nMessagesFound;
    }

    void MessageProcessor::processCommand(QStringList command)
    {
        TRACE(LOCAL);

        if(command.count() > 0)
        {
            QString comm(command[0].toLower());

//            if(command.count() > 1 && comm == "maxbuffercount") // Mensaje de terminación
//            {
//                int maxElems = command[1].toInt();
//                setMaxBufferPacketCount(maxElems);
//            }
//            else
            {
                CommandObject::processCommand(command);
            }
        }
    }

    void MessageProcessor::processMessageSAM(const MSG_TH *pMsg)
    {
        E_ID_MSG idMsg = pMsg->cab.idMsg;

        switch(idMsg)
        {
        case M_INICIO_SESION_SAM:
            // Solicitud de un inicio de sesión
            parseInicioSesionSAM(pMsg);
            break;
        case M_FIN_SESION_SAM:
            // Solicitud de un fin de sesión
            parseFinSesionSAM(pMsg);
            break;
        case M_CREACION_BATERIA_SAM:
            // Solicitud de creacion de una bateria de pruebas
            parseCreaBateriaSAM(pMsg);
            break;
        case M_BORRADO_BATERIA_SAM:
            // Solicitud de borrado de una bateria de pruebas
            parseBorraBateriaSAM(pMsg);
            break;
        case M_CONSULTA_BATERIA_SAM:
            // Solicitud de consulta de una bateria de pruebas mediante
            // el envío de varios mensajes de tipo
            // MSG_POSICION_BATERIA y M_INTERFAZ_BATERIA
            parseConsultaBateriaSAM(pMsg);
            break;
        case M_RESULTADO_BATERIA_SAM:
            // Solicitud de consulta de las mediciones de una bateria de pruebas mediante
            // el envío de varios mensajes de tipo
            // MSG_AUDIO_SAM_POSICION y MSG_AUDIO_SAM_INTERFAZ
            parseResultadoBateriaSAM(pMsg);
            break;
        case M_POSICION_BATERIA_SAM:
            // Solicitud de modificación de una bateria de pruebas
            // para posición
            parsePosicionBateriaSAM(pMsg);
            break;
        case M_INTERFAZ_BATERIA_SAM:
            // Solicitud de modificación de una bateria de pruebas
            // para interfaz
            parseInterfazBateriaSAM(pMsg);
            break;
        case M_MODIFICA_PARAMETROS:
            // Solicitud de modificación de los paramertos de
            // configuracion del SAM
            parseModificaParametros(pMsg);
            break;
        case M_CONSULTA_PARAMETROS:
            // Solicitud de consulta de los paramertos de
            // configuracion del SAM
            parseConsultaParametros(pMsg);
            break;
        case M_EJECUTA_BATERIA_SAM:
            // Solicitud de ejecución de batería SAM
            parseEjecutaBateriaSAM(pMsg);
            break;
        case M_RESP_ACTIVACION_PATRON:
            // Respuesta de la petición de activar o desactivar la posición patrón
            parseRespActivacionPatron(pMsg);
            break;
        case M_RESP_POSICION_MEDICION_SAM:
            // Respuesta de la petición de activar o desactivar la posición de medida
            parseRespActivacionPosicionSAM(pMsg);
            break;
        case M_RESP_LLAMADA_SAM:
            // Respuesta de la petición que se envió al patron de activar o desactivar
            // la posición de medida. Inicio del ciclo de configuración de bucles de la UGA.
            parseRespLlamadaSAM(pMsg);
            break;
        case M_SET_BUCLE_SAM:
            // Respuesta de la petición que se envió al puesto de activar los bucles de la UGA.
            parseSetBucleSAM(pMsg);
            break;

            /** @todo Hacer la parte del gateway
             */
        default:
            ;
        }
    }

}
