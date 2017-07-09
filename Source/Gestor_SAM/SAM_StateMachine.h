#ifndef SAM_STATEMACHINE_H
#define SAM_STATEMACHINE_H

#include "SAM_CommandObject.h"
#include "SAM_StateMachineResponseEvent.h"

class QState;
class QAbstractState;
class QStateMachine;

namespace SAM
{

    class ResponseTransition;
    class StateMachinePrivate;
    class MessageProcessor;
    struct SAM_DataBase;
    struct Battery;
    struct MeasuresCount;

    class StateMachine : public CommandObject
    {
        Q_OBJECT
        Q_ENUMS(StateType)

        Q_PROPERTY( QVariant samStateMachineInfo READ samStateMachineInfo )
        Q_PROPERTY( QVariant totalMeasures READ totalMeasures )
        Q_PROPERTY( QVariant requestedMeasures READ requestedMeasures )
        Q_PROPERTY( QVariant confirmedMeasures READ confirmedMeasures )
        Q_PROPERTY( float percentageMeasuresAdvance READ getPercentageMeasuresAdvance )

    protected:
        J_DECLARE_PRIVATE(StateMachine)
        Q_DISABLE_COPY(StateMachine)

    public:
        /**
         * @brief Posibles estados disponibles para la máquina de estados
         *
         */
        enum StateType
        {
            eNulo,
            eDesconocido,
            eTronco,
            eAudio,
            eRed,
            eReposo,
            eSolicitaPatronOn,
            eSolicitaMedicionEquipoOn,
            eSolicitaMedicionPatronOn,
            eSolicitaMedicionPatronOn1intento,
            eSolicitaMedicionPatronOn2intento,
            eSolicitaBucleOn,
            eSolicitaBucleOn1intento,
            eSolicitaBucleOn2intento,
            eSolicitaMedidaOn,
            eSolicitaInformeCanal,
            eSiguienteMedida,
            eSolicitaBucleOff,
            eSiguienteBucle,
            eSolicitaMedicionPatronOff,
            eSolicitaMedicionEquipoOff,
            eSiguienteEquipo,
            eSolicitaPatronOff
        };

        explicit StateMachine(JVariant & settings, MessageProcessor *parent = 0);

        static const char * getStateTypeName(StateType tipoEstado);

        /**
         * @brief Comprobar si un equipo se encuentra activo en el sistema.
         *  Se verificará la fecha del último mensaje de presencia recibido del Patrón
         *
         * @param ipEquipo IP del equipo
         * @return bool
         */
        bool isEquipmentRunning(QString ipEquipo);

        /**
         * @brief Inicia los estados y las transiciones de la máquina de estados.
         *
         */
        void init();

        /**
         * @brief Carga las baterías y mediciones desde el sistema de archivos.
         *
         * @param filePath Ruta del directorio que contiene los archivos con las baterías.
         * @return int Retorna el número de baterías que ha leído.
         */
        int loadBatteries(const QString & dirPath);

        /**
         * @brief Devuelve la configuración local de éste objeto.
         *
         * @return const JVariant
         */
        const JVariant & settings() const;

        /**
         * @brief Devuelve la configuración local de éste objeto.
         *
         * @return const JVariant
         */
        JVariant & settings();

        /**
         * @brief Función auxiliar para recuperar una batería de pruebas.
         *
         * @param idSesion
         * @param idBateria
         * @return JVariant
         */
        Battery &getBattery(uint idSesion, ushort idBateria);

        /**
         * @brief Devuelve el objeto Qt interno que representa la máquina de estados.
         *
         * @return QStateMachine
         */
        QStateMachine & stateMachineInternal();

        /**
         * @brief Devuelve el objeto Qt interno que representa la máquina de estados.
         *
         * @return QStateMachine
         */
        const QStateMachine & stateMachineInternal() const;

        /**
         * @brief Retorna el estado del Gestor SAM.
         *
         * @return const SAM_DataBase
         */
        const SAM_DataBase & samDataBase() const;

        /**
         * @brief Retorna el estado del Gestor SAM.
         *
         * @return const SAM_DataBase
         */
        SAM_DataBase & samDataBase();

        /**
         * @brief Obtiene el procesador de mensaje padre.
         *
         * @return MessageProcessor
         */
        MessageProcessor & messageProcessor();

        /**
         * @brief Devuelve el estado de la máquina de estados.
         *
         * @return QVariant
         */
        QVariant samStateMachineInfo() const;

        /**
         * @brief Comprueba si la máquina de estados está actualmente en el estado proporcionado.
         *
         * @param stateType
         * @return bool
         */
        bool isInState(StateType stateType) const;

        /**
         * @brief Cuenta del número de medidas previstas en una batería actual.
         *
         * @return QVariant
         */
        QVariant totalMeasures() const;

        /**
         * @brief Cuenta de las medidas solicitadas en la batería actual.
         *
         * @return QVariant
         */
        QVariant requestedMeasures() const;

        /**
         * @brief Lleva la cuenta de las medidas confirmadas en la batería.
         *
         * @return QVariant
         */
        QVariant confirmedMeasures() const;

        /**
         *  @brief Cuenta recursivamente la cantidad de dispositivos que se tienen que
         *  medir en la batería actual.
         *
         *  @return uint
         *  @sa SAM_DataBase::countBatteryMeasureDevices()
         */
        uint countCurrentBatteryMeasures();

        /**
         * @brief Devuelve el número de medidas que se tienen que medir.
         *  Es necesario que previamente se haya hecho una cuenta de dichos dispositivos
         *  mediante una llamada a StateMachine::countCurrentBatteryMeasures().
         *
         * @return const MeasuresCount
         */
        const MeasuresCount & getTotalMeasures() const;

        /**
         * @brief Devuelve el número de medidas que se han solicitado, ordenadas por dispositivos.
         *
         * @return const MeasuresCount
         */
        const MeasuresCount & getRequestedMeasures() const;

        /**
         * @brief Devuelve el número de medidas que se leído, ordenadas por dispositivos.
         *
         * @return const MeasuresCount
         */
        const MeasuresCount & getConfirmedMeasures() const;

        /**
         * @brief Devuelve el número de medidas que se han solicitado, ordenadas por dispositivos.
         *
         * @return const MeasuresCount
         */
        MeasuresCount & getRequestedMeasures();

        /**
         * @brief Devuelve el número de medidas que se leído, ordenadas por dispositivos.
         *
         * @return const MeasuresCount
         */
        MeasuresCount & getConfirmedMeasures();

        /**
         * @brief Obtiene el avance de las medidas de la ejecución actual.
         *
         * @return float Porcentage
         */
        float getPercentageMeasuresAdvance() const;

        /**
         * @brief Notifica que ha saltado un timeout mientras se encontraba en
         *  el estado `tipoEstado`.
         *
         * @param tipoEstado
         */
        void stateTimeout(StateType tipoEstado);

        /**
         * @brief Notifica que se ha entrado en el estado `tipoEstado`
         *  de la máquina de estados.
         *  Ésta función realiza el procesamiento que se da en el cuerpo de los estados
         *  definidos en la máquina de estados. Su labor principal es enviar los mensajes
         *  de medición a los equipos del sistema: patrón, puestos, gateways (interfaces), y SAGE.
         *
         * @param tipoEstado
         */
        void stateEntered(StateType tipoEstado);

        /**
         * @brief Ordena que se cancele la ejecución actual de la batería de medidas.
         *
         * @param strMotivoCancelacion
         * @return bool
         */
        bool cancelBatteryExecution(const QString & strMotivoCancelacion);

    protected slots:
        /**
         * @brief Llama internamente a stateEntered(StateType).
         *
         * @param tipoEstado
         */
        void onStateEntered(int tipoEstado);

        /**
         * @brief Llama internamente a stateTimeout(StateType).
         *
         * @param tipoEstado
         */
        void onStateTimeout(int tipoEstado);

    protected:
        /**
         * @brief Función auxiliar para configurar las transiciones y los timeouts entre un
         *  estado origen y n estados destino.
         *
         * @param sourceState
         * @param tipoEstado
         * @param timeout
         * @param timeoutTargetState
         * @param tipoMensaje1
         * @param targetState1
         * @param tipoMensaje2
         * @param targetState2
         * @param tipoMensaje3
         * @param targetState3
         * @param tipoMensaje4
         * @param targetState4
         */
        void addTransitions(QState *sourceState, StateType tipoEstado,
                            int timeout = 0, QAbstractState *timeoutTargetState = NULL,
                            ResponseEvent::EventType tipoMensaje1 = ResponseEvent::Nulo, QAbstractState *targetState1 = NULL,
                            ResponseEvent::EventType tipoMensaje2 = ResponseEvent::Nulo, QAbstractState *targetState2 = NULL,
                            ResponseEvent::EventType tipoMensaje3 = ResponseEvent::Nulo, QAbstractState *targetState3 = NULL,
                            ResponseEvent::EventType tipoMensaje4 = ResponseEvent::Nulo, QAbstractState *targetState4 = NULL);

        /**
         * @brief Añade una transición temporizada
         *
         * @param tipoEstado
         * @param timeout
         * @param origin
         * @param dest
         */
        void addTimedTransition(StateType tipoEstado,
                                int timeout,
                                QState *origin,
                                QAbstractState *dest);

        ResponseTransition * addTransition(ResponseEvent::EventType tipoMensaje,
                                           QState *sourceState = 0,
                                           QAbstractState *targetState = 0,
                                           ResponseEvent::EventType tipoMensajePreferido = ResponseEvent::Nulo);

        /**
         * @brief Conecta las señales de entrada y salida del estado a los slots
         *  onStateEntered() y onStateTimeout().
         *
         * @param tipoEstado
         * @param origin
         */
        void connectStateEntryExit(StateType tipoEstado, QAbstractState *origin);

     };

}

#endif // SAM_STATEMACHINE_H
