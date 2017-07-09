#include "SAM_StateMachineResponseTransition.h"
#include "SAM_StateMachineResponseEvent.h"
#include "SAM_StateMachine.h"
#include "SAM_DataBase.h"
#include "SAM_Helpers.h"
#include "SAM_MeasuresCount.h"
#include "SAM_Log.h"

namespace SAM
{
    ResponseTransition::ResponseTransition(ResponseEvent::EventType tipoMensaje,
                                           QState *sourceState,
                                           StateMachine *stateMachine)
        : QAbstractTransition(sourceState), m_eventType(tipoMensaje), m_stateMachine(stateMachine)
    {
    }

    void ResponseTransition::setEventType(ResponseEvent::EventType type)
    {
        m_eventType = type;
    }

    ResponseEvent::EventType ResponseTransition::eventType() const
    {
        return m_eventType;
    }

    ResponseEvent::EventType ResponseTransition::preferredEventType() const
    {
        return m_preferredEventType;
    }

    void ResponseTransition::setPreferredEventType(ResponseEvent::EventType preferredType)
    {
        m_preferredEventType = preferredType;
    }

    bool ResponseTransition::eventTest(QEvent *e)
    {
        TRACE(STATE);

        if (e->type() != QEvent::Type(QEvent::User+1)) // StringEvent
            return false;

        ResponseEvent *se = static_cast<ResponseEvent*>(e);

        bool bTest = m_eventType == se->type;
        if(!bTest)
            return false;

        const char * szEventType =
                ResponseEvent::getEventTypeName(m_eventType);
        const char * szPreferredEventType =
                ResponseEvent::getEventTypeName(m_preferredEventType);

        MeasuresCount & confirmedMeasures = m_stateMachine->getConfirmedMeasures();

        // Determina si el evento recibido no es el desado.
        bool bNoDeseado = m_preferredEventType != ResponseEvent::Nulo
                && m_preferredEventType != m_eventType;

        if(bNoDeseado) {
            LOG_DEBUG(STATE) << "Se ha recibido el evento" >> szEventType
                              << "pero el flujo normal de ejecución hubiera requerido"
                              >> szPreferredEventType;
        } else {
            LOG_DEBUG(STATE) << "Se ha recibido el evento" >> szEventType;
        }

        const JVariant & mensaje = se->data;

        // Comprueba que el contenido del evento se corresponde con el estado actual
        //SAM_DataBase & s = m_stateMachine->state();
        SAM_DataBase & estadoSAM = m_stateMachine->samDataBase();
        MeasuresState & estado = estadoSAM.estado;

        switch(m_eventType)
        {
        //case ResponseEvent::SalirBateria:
        //    break;
        case ResponseEvent::Salir:
            break;
        case ResponseEvent::EjecutaBateria:
        {
            uint idSesion = mensaje["idSesion"];
            ushort idBateria = mensaje["idBateria"];

            Battery & battery = m_stateMachine->getBattery(idSesion, idBateria);

            // Comprueba que la sesión de usuario y la batería existen
            if(!battery.idConfig)
            {
                LOG_WARN(AUDIO) << "No se puede ejecutar la batería de pruebas porque no "
                                   "existe una sesión de usuario con el identificador"
                                << idSesion << "o bien porque su identificador de batería"
                                << idBateria << "no está activo.";
                bTest = false; // No se cumplen las condiciones de la transición
                break;
            }

            // Inicia los contadores de avance de ejecución de la batería
            m_stateMachine->countCurrentBatteryMeasures();
            confirmedMeasures.clear();

            QString nombreBateria   = battery.nombre;
            quint16 idConfig        = battery.idConfig;
            quint16 subIdConfig     = battery.subIdConfig;
            quint16 idCentro        = battery.idCentro;

            // Tomar el archivo de configuraciones actual para obtener las IPs de las posiciones
            // El antiguo archivo está en: battery.config["HW"];
            JVariant HW = m_stateMachine->appSettingsConst()["CONFIG"]["HW"];

            JVariant T_CONFIG = HW["T_CONFIG"];
            if(!T_CONFIG) {
                LOG_ERROR(AUDIO) << "Se está pidiendo la ejecución de la batería de mediciones "
                                 >> nombreBateria << "ID =" << idConfig << "SUBID = " << subIdConfig
                                 << ", pero no se ha encontrado un archivo de configuración "
                                    "adecuado. Verifique que el Gestor SAM tenga acceso al archivo "
                                    "config_sdcxxi_hw.txt.zip. La ejecución será abortada.";
                bTest = false; // No se cumplen las condiciones de la transición
                break;
            }

            QString nombreConfig = T_CONFIG["NOMBRE"];

            if(   idConfig    != T_CONFIG["ID"].toUInt()
               || subIdConfig != T_CONFIG["SUB_ID"].toUInt()
               || idCentro    != T_CONFIG["ID_CENTRO"].toUInt())
            {
                LOG_WARN(AUDIO) << "La configuración almacenada en la batería de mediciones a posiciones"
                                << battery.toJson() << "no se corresponde con "
                                   "la configuración [T_CONFIG] actual" >> nombreConfig
                                << T_CONFIG.toJson() << ". La batería se va a ejecutar pero "
                                   "podrían haber problemas de concordancia de IDs.";
            }

            LOG_INFO(AUDIO) << "Ejecutando la batería de mediciones a posiciones"
                            << battery.toJson();

            /**
             *  5.2.2.5 Posición Patrón SAM
             *
             *  El servidor SAM debe supervisar el estado de la Posición Patrón SAM mediante
             *  las presencias recibidas de dicha posición (EvtCWP_Presencia). Antes de
             *  enviar el mensaje M_ACTIVACION_PATRON a la posición Patrón, el servidor
             *  (M_POSICION_PATRON_SAM)
             *  SAM debe supervisar el estado de la posición y comprobar que no está en
             *  estado ERROR, es decir, que se están recibiendo presencias de la posición.
             */

            JVariant GENERAL = m_stateMachine->appSettingsConst()["LOCAL"]["GENERAL"];
            QString ipPatron = GENERAL["IP_POS_PATRON"];
            QString batteryName = battery.nombre;

            // Comprobar la fecha del último mensaje de presencia recibido del Patrón
            bool bPatronRunning = m_stateMachine->isEquipmentRunning(ipPatron);
            if(!bPatronRunning)
                break;

            // Copiar el estado para la máquina de estados
            estado.idSesion = idSesion;
            estado.idBateria = idBateria;
            estado.ipPatron = ipPatron;

            LOG_INFO(AUDIO) << "Se ha identificado correctamente la sesión"
                            << idSesion << "y la batería" >> batteryName
                            << '#' << idBateria << "para su ejecución. El patrón"
                            >> ipPatron << "está activo.";
            break;
        }
        case ResponseEvent::EstadoPatronOn: // Respuesta de StateMachine::eSolicitaPatronOn
            bTest = mensaje["ipPatron"] == estado.ipPatron;
            if(bTest) {
                LOG_INFO(AUDIO) << "Se ha activado el patrón" >> estado.ipPatron;
                // Se prepara la iteración por cada puesto que vaya a ser medido.

                // Índices para iterar por los equipos de la batería actual
                estadoSAM.resetBatteryMeasureDevice();
                //estado.nPosicion = estado.nGateway = estado.nInterfaz = estado.nCanalBucle = 0;
            } else {
                bTest = false; // No se cumplen las condiciones de la transición
                LOG_WARN(AUDIO) << "Dirección inválida del patrón"
                                >> estado.ipPatron << "-" << mensaje["ipPatron"]
                                << "mientras se esperaba su activación.";
            }
            break;
        case ResponseEvent::EstadoPatronOff: // Respuesta de StateMachine::eSolicitaPatronOff/On
            bTest = mensaje["ipPatron"] == estado.ipPatron;
            if(bTest) {
                QString motivo = mensaje["motivo"];
                if(bNoDeseado)
                {
                    /**
                     *  @todo 5.2.2.1.1 Ejecución de una Batería de Pruebas
                     *
                     * Si la activación de la posición patrón no se realiza correctamente,
                     * el servidor SAM debe parar la Ejecución de la Batería de Pruebas y
                     * avisar al cliente SAM del error y el estado en que está la posición
                     * patrón (DESACTIVADA o ERROR), siendo los posibles errores los siguientes:
                     *
                     *      - "ERROR": no se reciben presencias de la posición.
                     *      - "TIMEOUT": TIMEOUT_SET_POS_PATRON superado.
                     *      - "SECTORIZADA": la posición ya está sectorizada.
                     *      - "FALLO_SECTORIZACION": la posición no pudo sectorizar con el usuario ID_USR_PATRON.
                     *      - "FALLO_REGISTRO": la posición no se pudo registrar en el proxy.
                     *      - "OTROS": otros motivos de rechazo.
                     */
                }

                LOG_INFO(AUDIO) << "Se ha desactivado el patrón" >> estado.ipPatron
                                << '.' << motivo << ". Se finaliza la batería.";

                estado.ipPatron.clear();
            } else {
                bTest = false; // No se cumplen las condiciones de la transición
                LOG_WARN(AUDIO) << "Dirección inválida del patrón"
                                >> estado.ipPatron << "-" << mensaje["ipPatron"]
                                << "mientras se esperaba su desactivación.";
            }
            break;
        case ResponseEvent::EstadoMedicionPuestoOn: // Respuesta de StateMachine::eSolicitaMedicionEquipoOn
            bTest = mensaje["ipEquipo"] == estado.ipEquipo;
            if(bTest) {
                confirmedMeasures.nPositions++;
                LOG_INFO(AUDIO) << "Se ha activado la medición del equipo"
                                >> estado.ipEquipo;
            } else {
                bTest = false; // No se cumplen las condiciones de la transición
                LOG_WARN(AUDIO) << "Dirección inválida del puesto"
                                >> estado.ipEquipo << "-" << mensaje["ipEquipo"]
                                << "mientras se esperaba su activación.";
            }
            break;
        case ResponseEvent::EstadoMedicionPuestoOff:
            bTest = mensaje["ipEquipo"] == estado.ipEquipo;
            if(bTest) {
                QString motivo = mensaje["motivo"];
                LOG_INFO(AUDIO) << "Se ha desactivado la medición del puesto"
                                >> estado.ipEquipo << '.' << motivo;
                estado.ipEquipo.clear();
            } else {
                bTest = false; // No se cumplen las condiciones de la transición
                LOG_WARN(AUDIO) << "Dirección inválida del equipo"
                                >> estado.ipEquipo << "-" << mensaje["ipEquipo"]
                                << "mientras se esperaba su desactivación.";
            }
            break;
        case ResponseEvent::EstadoMedicionInterfazOn:
            bTest = mensaje["ipEquipo"] == estado.ipEquipo;
            if(bTest) {
                confirmedMeasures.nLinks++;
                LOG_INFO(AUDIO) << "Se ha activado la medición de la interfaz"
                                >> estado.ipEquipo;
            } else {
                bTest = false; // No se cumplen las condiciones de la transición
                LOG_WARN(AUDIO) << "Dirección inválida de la interfaz"
                                >> estado.ipEquipo << "-" << mensaje["ipEquipo"]
                                << "mientras se esperaba su activación.";
            }
            break;
        case ResponseEvent::EstadoMedicionInterfazOff:
            bTest = mensaje["ipEquipo"] == estado.ipEquipo;
            if(bTest) {
                LOG_INFO(AUDIO) << "Se ha desactivado la medición de la interfaz"
                                >> estado.ipEquipo;
                estado.ipEquipo.clear();
            } else {
                bTest = false; // No se cumplen las condiciones de la transición
                LOG_WARN(AUDIO) << "Dirección inválida de la interfaz"
                                >> estado.ipEquipo << "-" << mensaje["ipEquipo"]
                                << "mientras se esperaba su desactivación.";
            }
            break;
        case ResponseEvent::EstadoMedicionPatronOn: // Respuesta de StateMachine::eSolicitaMedicionPatronOn
            bTest = mensaje["ipPatron"] == estado.ipPatron
                    && mensaje["ipEquipo"] == estado.ipEquipo;
            if(bTest) {
                estado.tipoCanalBucle = E_ID_CANAL_BUCLE();
                //estado.nCanalBucle = 0; // Índice para iterar por los bucles del equipo
                LOG_INFO(AUDIO) << "Se ha activado la medición del patrón"
                                >> estado.ipPatron << "conectado al equipo"
                                >> estado.ipEquipo;
            } else {
                bTest = false; // No se cumplen las condiciones de la transición
                LOG_WARN(AUDIO) << "Dirección inválida de patrón"
                                >> estado.ipPatron << "-" << mensaje["ipPatron"]
                                << "o del equipo a medir"
                                >> estado.ipEquipo << "-" << mensaje["ipEquipo"]
                                << "mientras se esperaba su activación de medición.";
            }
            break;
        case ResponseEvent::EstadoMedicionPatronOff:
            bTest = mensaje["ipPatron"] == estado.ipPatron;
            if(bTest) {
                LOG_INFO(AUDIO) << "Se ha desactivado la medición del patrón"
                                >> estado.ipPatron;
            } else {
                bTest = false; // No se cumplen las condiciones de la transición
                LOG_WARN(AUDIO) << "Dirección inválida del patrón"
                                >> estado.ipPatron << "-" << mensaje["ipPatron"]
                                << "mientras se esperaba su desactivación de medición.";
            }
            break;
        case ResponseEvent::EstadoBucleOn:
            bTest = mensaje["ipEquipo"] == estado.ipEquipo
                    && mensaje["tipoCanalBucle"] == estado.tipoCanalBucle;
            if(bTest) {
                confirmedMeasures.nLoops++;
                LOG_INFO(AUDIO) << "Se ha activado el bucle" << estado.tipoCanalBucle
                                << "del equipo" >> estado.ipEquipo;
            } else {
                bTest = false; // No se cumplen las condiciones de la transición
                LOG_WARN(AUDIO) << "Datos inválidos del equipo"
                                >> estado.ipEquipo << "-" << mensaje["ipEquipo"]
                                << "mientras se esperaba la activación de su bucle"
                                << estado.tipoCanalBucle << "-" << mensaje["tipoCanalBucle"];
            }
            break;
        case ResponseEvent::EstadoBucleOff:
            bTest = mensaje["ipEquipo"] == estado.ipEquipo
                    && mensaje["tipoCanalBucle"] == estado.tipoCanalBucle;
            if(bTest) {
                LOG_INFO(AUDIO) << "Se ha desactivado el bucle" << estado.tipoCanalBucle
                                << "del equipo" >> estado.ipEquipo;
                estado.tipoCanalBucle.clear();
            } else {
                bTest = false; // No se cumplen las condiciones de la transición
                LOG_WARN(AUDIO) << "Datos inválidos del equipo"
                                >> estado.ipEquipo << "-" << mensaje["ipEquipo"]
                                << "mientras se esperaba la desactivación de su bucle"
                                << estado.tipoCanalBucle << "-" << mensaje["tipoCanalBucle"];
            }
            break;
        case ResponseEvent::EstadoCanalOn:
            bTest = mensaje["ipEquipo"] == estado.ipEquipo
                    && mensaje["tipoCanalBucle"] == estado.tipoCanalBucle;
            if(bTest) {
                confirmedMeasures.nChannels++;
                LOG_INFO(AUDIO) << "Se ha activado el canal/bucle"
                                << estado.tipoCanalBucle << "del equipo" >> estado.ipEquipo;
            } else {
                bTest = false; // No se cumplen las condiciones de la transición
                LOG_WARN(AUDIO) << "Datos inválidos del equipo"
                                >> estado.ipEquipo << "-" << mensaje["ipEquipo"]
                                << "mientras se esperaba su activación con canal/bucle"
                                << estado.tipoCanalBucle << "-" << mensaje["tipoCanalBucle"];
            }
            break;
        case ResponseEvent::EstadoCanalOff:
            bTest = mensaje["ipEquipo"] == estado.ipEquipo
                    && mensaje["tipoCanalBucle"] == estado.tipoCanalBucle;
            if(bTest) {
                LOG_INFO(AUDIO) << "Se ha desactivado el canal/bucle"
                                << estado.tipoCanalBucle << "del equipo" >> estado.ipEquipo;
                estado.tipoCanalBucle.clear();
            } else {
                bTest = false; // No se cumplen las condiciones de la transición
                LOG_WARN(AUDIO) << "Datos inválidos del equipo"
                                >> estado.ipEquipo << "-" << mensaje["ipEquipo"]
                                << "mientras se esperaba su desactivación con canal/bucle"
                                << estado.tipoCanalBucle << "-" << mensaje["tipoCanalBucle"];
            }
            break;
        case ResponseEvent::InformeCanal:
            LOG_INFO(AUDIO) << "Se ha recibido el informe de medida correspondiente al canal/bucle"
                            << estado.tipoCanalBucle << "del equipo" >> estado.ipEquipo;
            break;

        default:
            ;
        }

        return bTest;
    }

    void ResponseTransition::onTransition(QEvent *) {}


}
