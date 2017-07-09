#include <QFinalState>
#include <QSignalMapper>
#include <QTimer>
#include <QDateTime>
#include <QMetaEnum>
#include <QStateMachine>


#include "SAM_StateMachineResponseTransition.h"
#include "SAM_StateMachineResponseEvent.h"
#include "SAM_MessageProcessor.h"
#include "SAM_Log.h"
#include "SAM_DataBase.h"
#include "SAM_StateMachinePrivate.h"
#include "SAM_MainTask.h"
#include "SAM_SAGE.h"
#include "SAM_StateMachine.h"

namespace SAM
{

    StateMachine::StateMachine(JVariant &settings, MessageProcessor *parent) :
        CommandObject(static_cast<QObject*>(parent), -1)
    {
        TRACE(STATE);

        m_ptr.reset(new StateMachinePrivate(settings, parent));
    }

    const char * StateMachine::getStateTypeName(StateType tipoEstado)
    {
        TRACE(STATE);

        const QMetaObject &mo = StateMachine::staticMetaObject;
        int index = mo.indexOfEnumerator("StateType");
        QMetaEnum metaEnum = StateMachine::staticMetaObject.enumerator(index);
        const char * szTipoEstado = metaEnum.valueToKey(tipoEstado);

        return szTipoEstado;
    }

    bool StateMachine::cancelBatteryExecution(const QString & strMotivoCancelacion)
    {
        TRACE(STATE);

        Q_D(StateMachine);

        if(strMotivoCancelacion.isEmpty()) {
            LOG_ALERT_LN(STATE) << "Es necesario especificar una causa para cancelar "
                                   "la ejecución de la batería.";
            return false;
        }

        if(!d->m_strMotivoCancelacion.isEmpty()) {
            LOG_ALERT(STATE) << "Se está intentando cancelar por" >> strMotivoCancelacion
                             << "la ejecución de una batería que ya se estaba cancelando por"
                             >> d->m_strMotivoCancelacion;
        }

        if(isInState(eReposo)) {
            LOG_WARN(STATE) << "Se está intentando cancelar por" >> strMotivoCancelacion
                            << "la ejecución de una batería que está en reposo.";
        }

        // Por alguna causa se ha ordenado que se cancele la ejecución de la batería de pruebas.
        // Envía el mensaje de salir del estado actual.
        d->m_machine->postEvent(new ResponseEvent(ResponseEvent::Salir));

        return true;
    }

    int StateMachine::loadBatteries(const QString & dirPath)
    {
        TRACE(STATE);

        SAM_DataBase & samDB = samDataBase();

        return samDB.loadSessions(dirPath);
    }

    void StateMachine::init()
    {
        TRACE(STATE);

        Q_D(StateMachine);

        if(d->m_sigMapper)
            return;

        LOG_DEBUG(STATE) << "Iniciando la máquina de estados" >> objectName();

        d->m_pSAGE = new SAGE(this);
        g_pMainTask->registerLocalCommandObject(d->m_pSAGE);

        d->m_sigMapper = new QSignalMapper(this);
        d->m_machine = new QStateMachine(this);
        d->m_machine->setObjectName("SAM_QStateMachine");

        JVariant LOCAL = appSettingsConst()["LOCAL"];

        JVariant SETTINGS = LOCAL["SETTINGS"];

        // Carga desde el sistema de archivos el estado de las baterías de pruebas
        QString storagePath = SETTINGS["STORAGE_PATH"];

        JVariant GENERAL = LOCAL["GENERAL"];

        /**
         *  5.2.2.5.1 Activación Posición Patrón SAM
         *
         *  El servidor SAM debe esperar, durante un tiempo (parámetro de configuración,
         *  TIMEOUT_SET_POS_PATRON) la contestación de activación del modo Patrón
         *  mediante el mensaje M_RESP_ACTIVACION_PATRON.
         */
        int timeoutEstadoPatron = GENERAL["TIMEOUT_SET_POS_PATRON"]; // Timeout a la espera de que se conmute el estado del patrón

        /**
         *  5.2.2.6.1 Activación de la posición en modo Medición
         *
         *  Si se supera el tiempo TIMEOUT_SET_POS_MEDICION y no se recibe el mensaje
         *  M_RESP_ACTIVACION_MEDIDAS, el servidor SAM debe suponer que la activación
         *  de la posición a medir no se ha realizado correctamente (estado DESACTIVADA).
         */
        int timeoutEstadoMedicionPuesto = GENERAL["TIMEOUT_SET_POS_MEDICION"]; // Timeout a la espera de que se conmute el estado de medición del puesto

        int timeoutEstadoMedicionPatron = GENERAL["TIMEOUT_SET_LLAMADA_SAM"]; // Timeout a la espera de que se conmute el estado de medición del patrón

        /**
         *  El servidor SAM debe esperar, durante un tiempo (parámetro de configuración,
         *  TIMEOUT_SET_BUCLE_UGA) la contestación del CWP en modo Medición al mensaje
         *  M_SET_BUCLE mediante el mensaje M_SET_BUCLE, indicando si el bucle se ha
         *  activado/desactivado correctamente o no.
         */
        int timeoutEstadoBucle = GENERAL["TIMEOUT_SET_BUCLE_UGA"]; // Timeout a la espera de que se conmute el estado del bucle

        int timeoutEstadoCanal = 0; // Timeout a la espera de que se acepte el canal
        int timeoutInforme = 0; // Timeout a la espera del informe de medida

        // Establecer todos los estados posibles de la aplicación

        // Estados de SAM: MED_AUDIO, MED_RED, REPOSO
        QState *sSAM = new QState();
        sSAM->setObjectName("eTronco");

        QState *sSAM_REPOSO = new QState(sSAM);
        sSAM_REPOSO->setObjectName("eReposo");

        // Contiene los estados necesarios para realizar la medición del audio
        // Comienza en el estado sSolicitaPatronOn
        //QState *sSAM_MED_RED = new QState(sSAM);

        /**
         *  @todo 5.2.2.1.1 Ejecución de una Batería de Pruebas
         *
         *   Si la activación de la posición patrón no se realiza correctamente,
         *   el servidor SAM debe parar la Ejecución de la Batería de Pruebas y
         *   avisar al cliente SAM del error y el estado en que está la posición
         *   patrón (DESACTIVADA o ERROR), siendo los posibles errores los siguientes:
         *
         *      - "ERROR": no se reciben presencias de la posición.
         *      - "TIMEOUT": TIMEOUT_SET_POS_PATRON superado.
         *      - "SECTORIZADA": la posición ya está sectorizada.
         *      - "FALLO_SECTORIZACION": la posición no pudo sectorizar con el usuario ID_USR_PATRON.
         *      - "FALLO_REGISTRO": la posición no se pudo registrar en el proxy.
         *      - "OTROS": otros motivos de rechazo.
         *
         *   Si durante la Ejecución de la Batería de Pruebas el servidor SAM
         *   detecta que la posición Patrón ya no está en estado ACTIVADA,
         *   el servidor SAM debe parar la Ejecución de la Batería de Pruebas
         *   y avisar al cliente SAM del error y el estado en que está la posición
         *   patrón (DESACTIVADA o ERROR).
         *
         *   Si durante la Ejecución de la Batería de Pruebas llega una nueva configuración
         *   del sistema, el servidor SAM debe parar la Ejecución de la Batería de Pruebas
         *   y avisar al cliente SAM del error.
         *
         *   Al finalizar la Ejecución de una Batería de Pruebas, el servidor SAM debe
         *   informar al cliente SAM si se ha ejecutado con éxito o no.
         *
         *   Al finalizar la Ejecución de una Batería de Pruebas, el servidor SAM debe
         *   generar el informe de pruebas.
         *
         *   Si la desactivación de la posición patrón no se realiza correctamente,
         *   el servidor SAM debe avisar al cliente SAM del error y el estado en que está
         *   la posición patrón (ACTIVADA o ERROR), siendo los posibles errores los siguientes:
         *
         *       - "ERROR": no se reciben presencias de la posición.
         *       - "TIMEOUT": TIMEOUT_SET_POS_PATRON superado.
         *       - "FALLO_SECTORIZACION": la posición no pudo des-sectorizarse.
         *       - "FALLO_REGISTRO": la posición no se pudo desregistrar en el proxy.
         *       - "OTROS": otros motivos de rechazo.
         *
         *   El resultado del informe de audio se envía al TMCS y al SeeSCV para que estén al
         *   tanto de las medidas realizadas, y al cliente SAM, para que pueda mostrar por
         *   pantalla el resultado obtenido.
         *
         *   Al finalizar la Ejecución de una Batería de Pruebas, el servidor SAM debe enviar
         *   el Informe de Pruebas de Audio de la Batería de Pruebas al cliente SAM.
         *
         *   Al finalizar la Ejecución de una Batería de Pruebas, el servidor SAM debe enviar
         *   el Informe de Pruebas de Audio de la Batería de Pruebas al S-TMCS.
         *
         *   Al finalizar la Ejecución de una Batería de Pruebas, el servidor SAM debe enviar
         *   el Informe de Pruebas de Audio de la Batería de Pruebas al S-SeeSCV.
         **/
        QState *sSAM_MED_AUDIO                      = new QState(sSAM);
        sSAM_MED_AUDIO->setObjectName("eAudio");
        QState *sSolicitaPatronOn                   = new QState(sSAM_MED_AUDIO);
        QState *sSolicitaMedicionEquipoOn           = new QState(sSAM_MED_AUDIO);
        QState *sSolicitaMedicionPatronOn           = new QState(sSAM_MED_AUDIO);
        QState *sSolicitaMedicionPatronOn1intento   = new QState(sSAM_MED_AUDIO);
        QState *sSolicitaMedicionPatronOn2intento   = new QState(sSAM_MED_AUDIO);
        QState *sSolicitaBucleOn                    = new QState(sSAM_MED_AUDIO);
        QState *sSolicitaBucleOn1intento            = new QState(sSAM_MED_AUDIO);
        QState *sSolicitaBucleOn2intento            = new QState(sSAM_MED_AUDIO);
        QState *sSolicitaMedidaOn                   = new QState(sSAM_MED_AUDIO);
        QState *sSolicitaInformeCanal               = new QState(sSAM_MED_AUDIO);
        QState *sSiguienteMedida                    = new QState(sSAM_MED_AUDIO);
        QState *sSolicitaBucleOff                   = new QState(sSAM_MED_AUDIO);
        QState *sSiguienteBucle                     = new QState(sSAM_MED_AUDIO);
        QState *sSolicitaMedicionPatronOff          = new QState(sSAM_MED_AUDIO);
        QState *sSolicitaMedicionEquipoOff          = new QState(sSAM_MED_AUDIO);
        QState *sSiguienteEquipo                    = new QState(sSAM_MED_AUDIO);
        QState *sSolicitaPatronOff                  = new QState(sSAM_MED_AUDIO);
        QFinalState *sNoMasEquipos                  = new QFinalState(sSAM);

        // El servidor SAM se encuentra en reposo y espera el evento de ejecución de la batería de pruebas EjecutaBateria(sSAM_MED_AUDIO).
        sSAM->setInitialState(sSAM_REPOSO);
        addTransition(ResponseEvent::EjecutaBateria, sSAM_REPOSO, sSAM_MED_AUDIO);

        // Estados de MED_AUDIO:
        // El servidor SAM está realizando medidas de audio analógico.

        /**
         *  5.2.2.1.1 Ejecución de una Batería de Pruebas
         *
         *  Si la activación de la posición patrón no se realiza correctamente,
         *  el servidor SAM debe parar la Ejecución de la Batería de Pruebas y
         *  avisar al cliente SAM del error y el estado en que está la posición
         *  patrón (DESACTIVADA o ERROR).
         *
         *  @todo Si durante la Ejecución de la Batería de Pruebas llega una
         *  nueva configuración del sistema, el servidor SAM debe parar la
         *  Ejecución de la Batería de Pruebas y avisar al cliente SAM del error.
         */
        // Se le solicita la activación al patrón y se espera a EstadoPatronOn(sSolicitaMedicionEquipoOn).
        // Si después de un timeout no puede, sale al estado final de reposo (sNoMasEquipos).
        sSAM_MED_AUDIO->setInitialState(sSolicitaPatronOn);
        addTransitions(sSolicitaPatronOn, eSolicitaPatronOn,
                       timeoutEstadoPatron, sNoMasEquipos,
                       ResponseEvent::EstadoPatronOn, sSolicitaMedicionEquipoOn,
                       ResponseEvent::EstadoPatronOff, sNoMasEquipos,
                       ResponseEvent::Salir, sNoMasEquipos);

        /**
         *  5.2.2.6.1 Activación de la posición en modo Medición
         *
         *   Para activar un S-CWP en modo Medición, el servidor SAM le debe enviar
         *  el mensaje M_POSICION_MEDICION_SAM con el campo "ACTIVAR_MEDICION_SAM"
         *  con valor “On”.
         *
         *  El servidor SAM debe considerar que la Posición a medir no se ha activado
         *  correctamente (estado DESACTIVADA) si antes de superar el
         *  TIMEOUT_SET_POS_MEDICION, se recibe el mensaje M_RESP_ACTIVACION_MEDIDAS
         *  con los siguientes valores:
         *
         *  - "ESTADO"="Off"
         *  - "MOTIVO_RECHAZO" con alguno de los siguientes valores:
         *      + "SECTORIZADA"
         *      + "FALLO_SECTORIZACION"
         *      + "FALLO_REGISTRO"
         *      + "OTROS".
         */
        /**
         *  El servidor SAM debe poder recibir un mensaje M_RESP_POSICION_MEDICION_SAM
         *   procedente de la posición a medir, indicando si el modo Medición se
         *   ha activado correctamente o no.
         */
        /** 5.2.2.3 Análisis de Audio de la Posición
         *
         *  Si la activación de la posición en Modo Medición no se realizó
         *   correctamente, el servidor SAM debe cancelar el Análisis de Audio
         *   de esa Posición.
         */
        /**
         *  El servidor SAM debe esperar, durante un tiempo (parámetro de configuración,
         *  TIMEOUT_SET_POS_MEDICION) la contestación de activación del modo Medición
         *  mediante el mensaje M_RESP_POSICION_MEDICION_SAM.
         */
        // Se inicia el bucle de medidas de audio de equipos.
        // Se considera que el patrón está activo.
        // Se toma el puesto a medir y se le solicita que pase a medición EstadoMedicionPuestoOn(sSolicitaMedicionPatronOn).
        // Si después de un timeout no puede, vuelve a intentarlo avanzando al siguiente puesto (sSiguienteEquipo).
        // Si no hay más puestos solicita pasar a reposo Patrón ().
        addTransitions(sSolicitaMedicionEquipoOn, eSolicitaMedicionEquipoOn,
                       timeoutEstadoMedicionPuesto, sSolicitaPatronOff,
                       ResponseEvent::EstadoMedicionPuestoOn, sSolicitaMedicionPatronOn,
                       ResponseEvent::EstadoMedicionPuestoOff, sSiguienteEquipo,
                       ResponseEvent::Salir, sSolicitaPatronOff);

        /**
         *  5.2.2.3 Análisis de Audio de la Posición
         *
         *  Si la activación de la posición en Modo Medición se realizó correctamente,
         *  el servidor SAM debe iniciar el Establecimiento de la llamada SAM.
         *
         *  Si la activación de la posición en Modo Medición no se realizó correctamente,
         *  el servidor SAM debe cancelar el Análisis de Audio de esa Posición.
         */
        // Se le pide al patrón que se ponga también en modo medición y empiece el bucle de mediciones EstadoMedicionPatronOn(sSolicitaBucleOn) 5.2.2.8 Llamada SAM
        // Si después de un timeout no puede, vuelve a intentarlo (sSolicitaMedicionPatronOn1intento).
        addTransitions(sSolicitaMedicionPatronOn, eSolicitaMedicionPatronOn,
                       timeoutEstadoMedicionPatron, sSolicitaMedicionPatronOn1intento,
                       ResponseEvent::EstadoMedicionPatronOn, sSolicitaBucleOn,
                       ResponseEvent::EstadoMedicionPatronOff, sSolicitaMedicionEquipoOff,
                       ResponseEvent::Salir, sSolicitaMedicionEquipoOff);

        /**
         *  5.2.2.8 Llamada SAM
         *
         *  Si el servidor SAM no recibe de la CWP Patrón el mensaje M_RESP_LLAMADA_SAM
         *  durante el tiempo TIMEOUT_SET_LLAMADA_SAM, el servidor SAM debe reenviar el
         *  mensaje M_SET_LLAMADA_SAM (considerará que el primer mensaje se ha perdido).
         */
        // Si ocurrió un timeout se le pide de nuevo al patrón que se ponga en modo medición EstadoMedicionPatronOn(sSolicitaBucleOn)
        // Si después de un timeout no puede, solicita abortar la medición de éste puesto (sSolicitaMedicionEquipoOff).
        addTransitions(sSolicitaMedicionPatronOn1intento, eSolicitaMedicionPatronOn1intento,
                       timeoutEstadoMedicionPatron, sSolicitaMedicionPatronOn2intento,
                       ResponseEvent::EstadoMedicionPatronOn, sSolicitaBucleOn,
                       ResponseEvent::EstadoMedicionPatronOff, sSolicitaMedicionEquipoOff,
                       ResponseEvent::Salir, sSolicitaMedicionEquipoOff);

        /**
         *  5.2.2.8 Llamada SAM
         *
         *  El mensaje M_SET_LLAMADA_SAM se debe reenviar como máximo dos veces.(3 envíos en total)
         */
        // Si ocurrió un timeout se le pide de nuevo al patrón que se ponga en modo medición EstadoMedicionPatronOn(sSolicitaBucleOn)
        // Si después de un timeout no puede, solicita abortar la medición de éste puesto (sSolicitaMedicionEquipoOff).
        addTransitions(sSolicitaMedicionPatronOn2intento, eSolicitaMedicionPatronOn2intento,
                       timeoutEstadoMedicionPatron, sSolicitaMedicionEquipoOff,
                       ResponseEvent::EstadoMedicionPatronOn, sSolicitaBucleOn,
                       ResponseEvent::EstadoMedicionPatronOff, sSolicitaMedicionEquipoOff,
                       ResponseEvent::Salir, sSolicitaMedicionEquipoOff);

        /**
         *  5.2.2.3 Análisis de Audio de la Posición
         *
         *  Si el Establecimiento de la llamada SAM se realizó correctamente,
         *   el servidor SAM debe realizar, para cada bucle seleccionado para
         *   esa posición en la batería de pruebas:
         *
         *  - la Activación del Bucle de la UGA.
         *  - la Realización del Test de Audio para las medidas seleccionadas para la posición.
         *  - la Desactivación del Bucle de la UGA.
         */
        /**
         * 5.2.2.9.1 Activación del Bucle de la UGA
         *
         *  Para activar un bucle de la UGA el servidor SAM debe enviar a la posición
         *   en modo Medición el mensaje M_SET_BUCLE con:
         *      - el campo "NBUCLE" indicando el bucle a activar.
         *      - el campo "ESTADO" con valor “On”.
         *
         *  Si el servidor SAM recibe de la CWP en modo Medición el mensaje
         *  de respuesta M_SET_BUCLE con:
         *  - el campo "NBUCLE" igual al indicado en la petición de activación.
         *  - el campo "ESTADO"="Off"
         *  durante el tiempo TIMEOUT_SET_LLAMADA_SAM, el servidor SAM debe
         *  considerar que el bucle indicado no se ha establecido correctamente.
         */
        // Se inicia el bucle de medición de los bucles de audio.
        // Se solicita la activación del bucle de medidas del puesto EstadoBucleOn(sSolicitaCanalOn).
        // Si no quedan más bucles de medidas se solicita la desactivación del modo medición del patrón (sSolicitaMedicionPatronOff).
        // Si después de un timeout no puede, vuelve a intentarlo (sSolicitaBucleOn1intento).
        addTransitions(sSolicitaBucleOn, eSolicitaBucleOn,
                       timeoutEstadoBucle, sSolicitaBucleOn1intento,
                       ResponseEvent::EstadoBucleOn, sSolicitaMedidaOn,
                       ResponseEvent::EstadoBucleOff, sSiguienteBucle,
                       ResponseEvent::Salir, sSolicitaMedicionPatronOff);

        /**
         *  5.2.2.9 Bucles de la UGA
         *
         *  Si el servidor SAM no recibe de la CWP en modo Medición el mensaje
         *  M_SET_BUCLE durante el tiempo TIMEOUT_SET_BUCLE_UGA, el servidor SAM debe
         *  reenviar el mensaje M_SET_BUCLE una vez más (considerará que el primer
         *  mensaje se ha perdido).
         */
        // Se reintenta el bucle de medición de los bucles de audio.
        // Se solicita la activación del bucle de medidas del puesto EstadoBucleOn(sSolicitaCanalOn).
        // Si no quedan más bucles de medidas se solicita la desactivación del modo medición del patrón (sSolicitaMedicionPatronOff).
        // Si después del timeout no puede, vuelve a intentarlo (sSolicitaBucleOn2intento).
        addTransitions(sSolicitaBucleOn1intento, eSolicitaBucleOn1intento,
                       timeoutEstadoBucle, sSolicitaBucleOn2intento,
                       ResponseEvent::EstadoBucleOn, sSolicitaMedidaOn,
                       ResponseEvent::EstadoBucleOff, sSiguienteBucle,
                       ResponseEvent::Salir, sSolicitaMedicionPatronOff);

        /**
         *  El mensaje M_SET_BUCLE se debe reenviar como máximo dos veces. (En total 3 intentos)
         */
        // Se solicita la activación del bucle de medidas del puesto EstadoBucleOn(sSolicitaCanalOn).
        // Si después de un timeout no puede, solicita la medición del siguiente bucle ().
        addTransitions(sSolicitaBucleOn2intento, eSolicitaBucleOn2intento,
                       timeoutEstadoBucle, sSolicitaMedicionPatronOff,
                       ResponseEvent::EstadoBucleOn, sSolicitaMedidaOn,
                       ResponseEvent::EstadoBucleOff, sSiguienteBucle,
                       ResponseEvent::Salir, sSolicitaMedicionPatronOff);

        // Se inicia el bucle de medición de los canales de los bucles de audio con el SAGE.
        // Se solicita al SAGE la activación del canal EstadoBucleOn(solicitaInformeCanal).
        // Si después de un timeout no puede activar el canal, se pasa al siguiente canal (sSiguienteCanal).
        // Si no quedan más canales se solicita la desactivación del bucle (sSolicitaBucleOff)
        addTransitions(sSolicitaMedidaOn, eSolicitaMedidaOn,
                       timeoutEstadoCanal, sSiguienteMedida,
                       ResponseEvent::EstadoCanalOn, sSolicitaInformeCanal,
                       ResponseEvent::EstadoCanalOff, sSiguienteMedida,
                       ResponseEvent::Salir, sSolicitaBucleOff);

        // Se solicita el informe al SAGE (sSiguienteCanal).
        // Si después de un timeout no puede, informa de la pérdida del informe y solicita el siguiente canal (sSiguienteCanal).
        addTransitions(sSolicitaInformeCanal, eSolicitaInformeCanal,
                       timeoutInforme, sSiguienteMedida,
                       ResponseEvent::InformeCanal, sSiguienteMedida,
                       ResponseEvent::Salir, sSolicitaBucleOff);

        // Se ordena la medición del siguiente canal (sSolicitaCanalOn).
        addTransitions(sSiguienteMedida, eSiguienteMedida,
                       0, sSolicitaMedidaOn);

        /** @todo 5.2.2.9.2 Desactivación del Bucle de la UGA
         */
        // Se solicita la desactivación del bucle de medidas del puesto EstadoBucleOff(eSolicitaBucleOff).
        // Si no quedan más bucles de medidas se solicita la salida del bucle (sSolicitaMedicionPatronOff).
        // Si después de un timeout no puede, vuelve a intentarlo con el siguiente bucle (sSiguienteBucle).
        addTransitions(sSolicitaBucleOff, eSolicitaBucleOff,
                       timeoutEstadoBucle, sSiguienteBucle,
                       ResponseEvent::EstadoBucleOff, sSiguienteBucle,
                       ResponseEvent::Salir, sSolicitaMedicionPatronOff);

        // Se considera que el bucle se ha desactivado
        // Se ordena la medición del siguiente bucle (sSolicitaBucleOn).
        addTransitions(sSiguienteBucle, eSiguienteBucle,
                       0, sSolicitaBucleOn);

        /**
         *  5.2.2.3 Análisis de Audio de la Posición
         *
         *  Cuando el servidor SAM haya realizado el Test de Audio en todos los
         *  bucles seleccionados para esa posición y haya desactivado el último
         *  bucle, debe continuar con la Finalización de la llamada SAM.
         */
        // Se le pide al patrón que salga del modo medición EstadoMedicionPatronOff(sSolicitaMedicionEquipoOff)
        addTransitions(sSolicitaMedicionPatronOff, eSolicitaMedicionPatronOff,
                       timeoutEstadoMedicionPatron, sSolicitaMedicionPatronOff,
                       ResponseEvent::EstadoMedicionPatronOff, sSolicitaMedicionEquipoOff,
                       ResponseEvent::Salir, sSolicitaMedicionEquipoOff);


        /**
         *  5.2.2.6.2 Desactivación de la posición en modo Medición
         *
         *  Para desactivar un S-CWP en modo Medición, el servidor SAM le debe enviar
         *  el mensaje M_POSICION_MEDICION_SAM con el campo "ACTIVAR_MEDICION_SAM"
         *  con valor “Off”.
         *
         *  El servidor SAM debe poder recibir un mensaje M_RESP_POSICION_MEDICION_SAM
         *  procedente de la posición a medir, indicando si el modo Medición se ha
         *  desactivado correctamente o no.
         */
        // Solicita que el puesto deje de estar en modo medición
        addTransitions(sSolicitaMedicionEquipoOff, eSolicitaMedicionEquipoOff,
                       timeoutEstadoMedicionPuesto, sSiguienteEquipo,
                       ResponseEvent::EstadoMedicionPuestoOff, sSiguienteEquipo,
                       ResponseEvent::Salir, sSolicitaPatronOff);

        /** 5.2.2.3 Análisis de Audio de la Posición
         *
         *  Si la Finalización de la llamada SAM se realizó correctamente,
         *  el servidor SAM debe continuar con la Desactivación de la interfaz???
         *  en Modo Medición.
         */
        // Solicita que se desactive el patrón
        addTransitions(sSolicitaPatronOff, eSolicitaPatronOff,
                       timeoutEstadoPatron, sNoMasEquipos,
                       ResponseEvent::EstadoPatronOff, sNoMasEquipos,
                       ResponseEvent::Salir, sNoMasEquipos);


        // Se considera que el equipo de medida está en reposo.
        // Se ordena que se hagan las mediciones del siguiente equipo (sSolicitaMedicionEquipoOn).
        addTransitions(sSiguienteEquipo, eSiguienteEquipo,
                       0, sSolicitaMedicionEquipoOn);

        // Se considera que el patrón ha pasado a reposo.
        // Se pasa al estado de reposo (sSAM_REPOSO).
        sSAM_MED_AUDIO->addTransition(sSAM_MED_AUDIO, SIGNAL(finished()), sSAM_REPOSO);

        // Estados de MED_RED:
        // El servidor SAM está realizando medidas de red.
        d->m_machine->addState(sSAM);
        d->m_machine->setInitialState(sSAM);

        d->m_machine->start();

//#ifdef QT_DEBUG
//        // Volcar información de depuración para ver cómo se ha construido la máquina de estados.
//        d->m_machine->dumpObjectInfo();
//        d->m_machine->dumpObjectTree();
//#endif
    }

    void StateMachine::addTimedTransition(StateType tipoEstado,
                                          int timeout,
                                          QState * origin,
                                          QAbstractState * dest)
    {
        TRACE(STATE);

        Q_D(StateMachine);

        if(timeout == 0)
        {
            // Se pasa directamente al destino según se entra en el estado origin
            origin->addTransition(origin, SIGNAL(entered()), dest);
            return;
        }
        else if(timeout > 0)
        {
            QTimer *timer = new QTimer(origin);
            timer->setInterval(timeout);
            timer->setSingleShot(true);

            QObject::connect(origin, SIGNAL(entered()), timer, SLOT(start()));
            QObject::connect(origin, SIGNAL(exited()), timer, SLOT(stop()));
            QObject::connect(timer, SIGNAL(timeout()), d->m_sigMapper, SLOT(map()));

            d->m_sigMapper->setMapping(timer, (int)tipoEstado);

            // Transmitir el evento de timeout hacia el exterior
            QObject::connect(d->m_sigMapper, SIGNAL(mapped(int)), this, SLOT(onStateTimeout(int)));

            origin->addTransition(timer, SIGNAL(timeout()), dest);
        }
        else
        {
            LOG_ALERT(STATE) << "Se ha configurado un timeout infinito para el estado"
                            >> origin->objectName();
        }
    }

    void StateMachine::connectStateEntryExit(StateType tipoEstado,
                                         QAbstractState * origin)
    {
        TRACE(STATE);

        Q_D(StateMachine);

        QObject::connect(origin, SIGNAL(entered()), d->m_sigMapper, SLOT(map()));

        d->m_sigMapper->setMapping(origin, (int)tipoEstado);

        QObject::connect(d->m_sigMapper, SIGNAL(mapped(int)), this, SLOT(onStateEntered(int)));
    }

    ResponseTransition * StateMachine::addTransition(ResponseEvent::EventType tipoMensaje,
                                                     QState *sourceState,
                                                     QAbstractState *targetState,
                                                     ResponseEvent::EventType tipoMensajePreferido)
    {
        TRACE(STATE);

        // Verificar que los campos mínimos han sido proporcionados
        if(tipoMensaje == ResponseEvent::Nulo || !sourceState || !targetState)
            return NULL;

        ResponseTransition * ret = new ResponseTransition(tipoMensaje, sourceState, this);

        if(ret->objectName().isEmpty()) {
            ret->setObjectName(ResponseEvent::getEventTypeName(tipoMensaje));
        }

        ret->setTargetState(targetState);
        ret->setPreferredEventType(tipoMensajePreferido);

        return ret;
    }

    void StateMachine::onStateEntered(int tipoEstado)
    {
        TRACE(STATE);

        // Simplemente reemite la señal
        stateEntered((StateMachine::StateType) tipoEstado);
    }

    void StateMachine::onStateTimeout(int tipoEstado)
    {
        TRACE(STATE);

        // Simplemente reemite la señal
        stateTimeout((StateMachine::StateType) tipoEstado);
    }

    void StateMachine::addTransitions(QState *sourceState, StateType tipoEstado,
                        int timeout, QAbstractState *timeoutTargetState,
                        ResponseEvent::EventType tipoMensaje1, QAbstractState *targetState1,
                        ResponseEvent::EventType tipoMensaje2, QAbstractState *targetState2,
                        ResponseEvent::EventType tipoMensaje3, QAbstractState *targetState3,
                        ResponseEvent::EventType tipoMensaje4, QAbstractState *targetState4)
    {
        TRACE(STATE);

        if(sourceState->objectName().isEmpty()) {
            sourceState->setObjectName(getStateTypeName(tipoEstado));
        }

        connectStateEntryExit(tipoEstado, sourceState);

        if(timeoutTargetState) {
            addTimedTransition(tipoEstado, timeout, sourceState, timeoutTargetState);
        }

        addTransition(tipoMensaje1, sourceState, targetState1);
        addTransition(tipoMensaje2, sourceState, targetState2, tipoMensaje1);
        addTransition(tipoMensaje3, sourceState, targetState3, tipoMensaje1);
        addTransition(tipoMensaje4, sourceState, targetState4, tipoMensaje1);
    }

    Battery & StateMachine::getBattery(uint idSesion, ushort idBateria)
    {
        TRACE(STATE);

        static Battery retNull;

        SAM_DataBase & samDB = samDataBase();

        if(!samDB.sesionesUsuario.contains(idSesion)) {
            LOG_ALERT_LN(STATE) << "No se encuentra la sesión" << idSesion << "solicitada";
            return retNull;
        }

        if(!samDB.sesionesUsuario[idSesion].baterias.contains(idBateria)){
            LOG_ALERT_LN(STATE) << "No se encuentra la batería" << idBateria
                           << "solicitada en la sesión" << idSesion;
            return retNull;
        }

        Battery & battery = samDB.sesionesUsuario[idSesion].baterias[idBateria];

        return battery;
    }

    bool StateMachine::isEquipmentRunning(QString ipEquipo)
    {
        TRACE(STATE);

        bool bRet = false;

        const SAM_DataBase &samDB = samDataBase();

        // Comprobar la fecha del último mensaje de presencia recibido del Patrón
        JVariant jLast = samDB.mensajes[ipEquipo]["PRES"]["ultimo"]["mensaje"];

        if(jLast)
        {
            QDateTime fecha = jLast["fecha"].toDateTime();

            if(fecha.isValid())
            {
                int secsDiff = fecha.secsTo(QDateTime::currentDateTime());

                if(secsDiff > 5) {
                    LOG_WARN(AUDIO) << "No se puede ejecutar la prueba porque hace"
                                    << secsDiff << "segundos que el equipo"
                                    >> ipEquipo << "no tiene actividad.";
                } else {
                    bRet = true;
                }
            }
            else
            {
                LOG_WARN_LN(AUDIO) << "No se puede ejecutar la prueba porque la fecha "
                                      "del último mensaje de presencia del equipo con IP ="
                                   >> ipEquipo << "es inválida.";
            }
        }
        else
        {
            LOG_WARN(AUDIO) << "No se puede ejecutar la prueba porque "
                               "no se ha detectado un equipo con IP ="
                            >> ipEquipo << "en el sistema. Verificar que genera presencias.";
        }

        return bRet;
    }

    const JVariant & StateMachine::settings() const
    {
        Q_D(const StateMachine);

        return d->m_settings;
    }

    JVariant & StateMachine::settings()
    {
        Q_D(StateMachine);

        return d->m_settings;
    }

    QStateMachine & StateMachine::stateMachineInternal()
    {
        Q_D(StateMachine);

        return *d->m_machine;
    }

    const QStateMachine & StateMachine::stateMachineInternal() const
    {
        Q_D(const StateMachine);

        return *d->m_machine;
    }

    const SAM_DataBase & StateMachine::samDataBase() const
    {
        Q_D(const StateMachine);

        return d->m_samDB;

    }

    SAM_DataBase & StateMachine::samDataBase()
    {
        Q_D(StateMachine);

        return d->m_samDB;

    }

    MessageProcessor & StateMachine::messageProcessor()
    {
        Q_D(StateMachine);

        return *d->m_pMessageProcessor;

    }

    QVariant StateMachine::samStateMachineInfo() const
    {
        Q_D(const StateMachine);

        JVariant jState(d->m_samDB.toVariant());

        // Añadir una lista con los estados seleccionados actualmente en la máquina de estados
        QStringList stateNames;

        QSet<QAbstractState*> configuration = stateMachineInternal().configuration();
        foreach(const QAbstractState* pState, configuration)
        {
            QString name = pState->objectName();
            stateNames << name;
        }

        jState["estado"]["currentStateNames"] = stateNames;

        return jState;
    }

    bool StateMachine::isInState(StateType stateType) const
    {
        //Q_D(const StateMachine);

        const char * szState = StateMachine::getStateTypeName(stateType);

        // Busca el estado en la lista de los estados en los que está la máquina virtual.
        QSet<QAbstractState*> configuration = stateMachineInternal().configuration();
        foreach(const QAbstractState* pState, configuration)
        {
            QString name = pState->objectName();
            if(name == szState) {
                return true;
            }
        }

        return false;
    }

    QVariant StateMachine::totalMeasures() const
    {
        Q_D(const StateMachine);

        return d->m_totalMeasures.toVariant();
    }

    QVariant StateMachine::requestedMeasures() const
    {
        Q_D(const StateMachine);

        return d->m_requestedMeasures.toVariant();
    }

    QVariant StateMachine::confirmedMeasures() const
    {
        Q_D(const StateMachine);

        return d->m_confirmedMeasures.toVariant();
    }

    uint StateMachine::countCurrentBatteryMeasures()
    {
        Q_D(StateMachine);

        // Cuenta el número de medidas previstas para esta batería
        d->m_totalMeasures.clear();

        return samDataBase().countBatteryMeasureDevices(&d->m_totalMeasures);
    }

    const MeasuresCount & StateMachine::getTotalMeasures() const
    {
        Q_D(const StateMachine);

        return d->m_totalMeasures;
    }

    const MeasuresCount & StateMachine::getRequestedMeasures() const
    {
        Q_D(const StateMachine);

        return d->m_requestedMeasures;
    }

    const MeasuresCount & StateMachine::getConfirmedMeasures() const
    {
        Q_D(const StateMachine);

        return d->m_confirmedMeasures;
    }

    MeasuresCount & StateMachine::getRequestedMeasures()
    {
        Q_D(StateMachine);

        return d->m_requestedMeasures;
    }

    MeasuresCount & StateMachine::getConfirmedMeasures()
    {
        Q_D(StateMachine);

        return d->m_confirmedMeasures;
    }

    float StateMachine::getPercentageMeasuresAdvance() const
    {
        Q_D(const StateMachine);

        float totalMeasures = static_cast<float>(d->m_totalMeasures.sum()) * 100.f;

        return isNumber(totalMeasures) ? d->m_requestedMeasures.sum() / totalMeasures : 0.f;
    }
}
