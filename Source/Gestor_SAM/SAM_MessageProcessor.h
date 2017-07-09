#ifndef SAM_MessageProcessor_H
#define SAM_MessageProcessor_H


#include "SAM_Packet.h"
#include "SAM_StateMachine.h"
#include "SAM_CommandObject.h"

class QStateMachine;

namespace SAM_Declaration
{
    struct MSG_TH;
}

namespace SAM
{

    using namespace SAM_Declaration;

    class MessageProcessorPrivate;

    /**
     * @brief Procesamiento de los mensajes binarios de tipo SAM_Declaration::MSG_TH recibidos desde:
     *  TMCS, Posición, Gateway,...
     */
    class MessageProcessor : public CommandObject
    {
        Q_OBJECT

        Q_PROPERTY( qint64 processedMsgCount READ processedMsgCount )
        Q_PROPERTY( qint64 receivedMsgCount READ receivedMsgCount )
        Q_PROPERTY( qint64 rejectedBytesCount READ rejectedBytesCount )
        Q_PROPERTY( qint64 receivedBytesCount READ receivedBytesCount )
        Q_PROPERTY( QVariant settings READ settings )
        Q_PROPERTY( unsigned int sessionCounter READ sessionCounter )
        Q_PROPERTY( unsigned int batteryCounter READ batteryCounter )
        Q_PROPERTY( unsigned int sessionOpened READ sessionOpened )
        Q_PROPERTY( int timerPresWatchdogInterval READ timerPresWatchdogInterval )
        Q_PROPERTY( int timerPresEstadInterval READ timerPresEstadInterval )
        Q_PROPERTY( qint64 packetsWrittenToWatchdog READ packetsWrittenToWatchdog )
        Q_PROPERTY( QString modoSAM READ modoSAM )

        J_DECLARE_PRIVATE(MessageProcessor)
        Q_DISABLE_COPY(MessageProcessor)


    public:
        explicit MessageProcessor(QObject *parent = 0);

        ~MessageProcessor();

        /**
         * @brief Devuelve el número total de mensajes SCV que se han recibido
         *  y estaban bien formados.
         *  Se incluyen los mensajes que no serán procesados.
         *
         * @return qint64
         */
        qint64 receivedMsgCount() const;

        /**
         * @brief Devuelve el número total de mensajes SAM que se han procesado.
         *
         * @return qint64
         */
        qint64 processedMsgCount() const;

        /**
         * @brief Cuenta de bytes rechazados en el análisis
         * @return
         */
        qint64 rejectedBytesCount() const;

        /**
         * @brief Cuenta de bytes recibidos para procesar.
         * @return
         */
        qint64 receivedBytesCount() const;

        /** Cuenta el número de sesiones de usuario que se han concedido */
        unsigned int sessionCounter() const;

        /** Cuenta el número de baterías de prueba que se han creado hasta el momento */
        unsigned int batteryCounter() const;

        /** Almacena el número de la sesión actualmente abierta. A 0 no hay sesión */
        unsigned int sessionOpened() const;

        /** Intervalo de tiempo entre envíos de mensajes de presencia al Watchdog. */
        int timerPresWatchdogInterval() const;

        /** Intervalo de tiempo entre envíos de mensajes de presencia a Estadísticas. */
        int timerPresEstadInterval() const;

        /** Número de paquetes enviados al Watchdog. */
        qint64 packetsWrittenToWatchdog() const;

        QString modoSAM() const;

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_INF_FICH_CONFIG
         *
         * @param msg Mensaje en el endian del anfitrión.
         */
        bool parseDataInfFichConfig(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_FICH_CONFIG
         *
         * @param msg Mensaje en el endian del anfitrión.
         */
        bool parseDataFichConfig(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_INICIO_SESION_SAM
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parseInicioSesionSAM(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_FIN_SESION_SAM
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parseFinSesionSAM(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_CREACION_BATERIA_SAM
         * @sa M_ENVIA_BATERIA_VACIA.
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parseCreaBateriaSAM(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_BORRADO_BATERIA_SAM
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parseBorraBateriaSAM(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_RESULTADO_BATERIA_SAM.
         *  Consulta del resultado de una bateria de pruebas y lo
         *  envía al `SUPERVISOR` con varios mensajes de tipo
         *  SAM_Declaration::MSG_AUDIO_SAM_POSICION y
         *  SAM_Declaration::MSG_AUDIO_SAM_INTERFAZ
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parseResultadoBateriaSAM(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_CONSULTA_BATERIA_SAM.
         *  Consulta una bateria de pruebas y lo
         *  envía al `SUPERVISOR` con varios mensajes de tipo
         *  SAM_Declaration::MSG_POSICION_BATERIA y
         *  SAM_Declaration::MSG_INTERFAZ_BATERIA
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parseConsultaBateriaSAM(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_POSICION_BATERIA_SAM.
         *  Modifica una bateria de pruebas.
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parsePosicionBateriaSAM(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_INTERFAZ_BATERIA_SAM.
         *  Modifica una bateria de pruebas.
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parseInterfazBateriaSAM(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_MODIFICA_PARAMETROS.
         *  Modificacion de los paramertos de configuracion del SAM.
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parseModificaParametros(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_CONSULTA_PARAMETROS.
         *  Consulta de los paramertos de configuracion del SAM.
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parseConsultaParametros(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_EJECUTA_BATERIA_SAM.
         *
         *  5.2.2.1.1 Ejecución de una Batería de Pruebas
         *   - Activación de la Posición Patrón.
         *   - Análisis de Audio de Posición por cada posición
         *      que intervenga en la batería de pruebas,
         *   - Análisis de Audio de Interfaz por cada interfaz
         *      que intervenga en la batería de pruebas.
         *   - Desactivación de la Posición Patrón.
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parseEjecutaBateriaSAM(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_RESP_ACTIVACION_PATRON.
         *
         *  5.2.2.5.1 Activación Posición Patrón SAM
         *
         *  El servidor SAM debe poder recibir un mensaje M_RESP_ACTIVACION_PATRON
         *   procedente de la posición Patrón indicando si el modo Patrón se ha activado
         *   correctamente o no. El mensaje M_RESP_ACTIVACION_PATRON debe contener un
         *   campo "ESTADO" que indique si la posición Patrón está en estado ACTIVADA
         *   o DESACTIVADA, con dos posibles valores, "On" y "Off".
         *
         *  El mensaje M_RESP_ACTIVACION_PATRON debe contener un campo "MOTIVO_RECHAZO"
         *   que indique si la petición de cambiar a modo "ACTIVADA" se ha rechazado o no,
         *   y si se ha rechazado, el motivo de rechazo.
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parseRespActivacionPatron(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_RESP_POSICION_MEDICION_SAM.
         *
         *  5.2.2.6.1 Activación de la posición en modo Medición
         *
         *  El servidor SAM debe poder recibir un mensaje M_RESP_POSICION_MEDICION_SAM
         *  procedente de la posición a medir, indicando si el modo Medición se ha activado
         *  correctamente o no.
         *
         *  El mensaje M_RESP_POSICION_MEDICION_SAM debe contener un campo "ESTADO" que
         *  indique si la posición a medir está en estado ACTIVADA o DESACTIVADA, con dos
         *  posibles valores, "On" y "Off".
         *
         *  El mensaje M_RESP_POSICION_MEDICION_SAM debe contener un campo "MOTIVO_RECHAZO"
         *  que indique si la petición de cambiar a modo ACTIVADA se ha rechazado o no,
         *  y si se ha rechazado, el motivo de rechazo.
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parseRespActivacionPosicionSAM(const MSG_TH *pMsg);

        /**
         * @brief 5.2.2.3 Análisis de Audio de la Posición
         *        ----------------------------------------
         *
         *   Para realizar el Análisis de Audio de la Posición,
         *    el servidor SAM debe conocer los siguientes datos:
         *
         *   - Nombre de la Batería de Pruebas.
         *   - IP de la Posición Patrón.
         *   - IP de la Posición a medir.
         *   - Los bucles seleccionados para dicha posición.
         *   - Las medidas seleccionadas para dicha posición.
         *
         *   Los pasos que se deben seguir para realizar el
         *   Análisis de Audio de la Posición, son:
         *
         *   1. Activación de la posición en Modo Medición.
         *   2. Establecimiento de la llamada SAM.
         *   3. Para cada Bucle seleccionado para dicha posición:
         *
         *     a) Activación del Bucle de la UGA.
         *     b) Realización del Test de Audio.
         *     c) Desactivación del Bucle de la UGA.
         *
         *   4. Finalización de la llamada SAM.
         *   5. Desactivación de la posición en Modo Medición.
         *
         *  @return bool Si es `false` no se puede hacer las pruebas de audio en posiciones.
         *  El servidor debe salir del estado de `MED_AUDIO`.
         */

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_RESP_LLAMADA_SAM.
         *  Con la respuesta del Patrón comunicando si se ha establecido la llamada
         *  para la posición especificada. Inicia el ciclo de activación de bucles
         *  de la UGA enviando los mensajes M_SET_BUCLE por cada bucle del puesto.
         *
         *  5.2.2.8 Llamada SAM
         *
         *  El servidor SAM debe poder recibir un mensaje M_RESP_LLAMADA_SAM procedente
         *  de la posición Patrón indicando si la llamada SAM se ha establecido o finalizado.
         *
         *  El mensaje M_SET_LLAMADA_SAM debe contener un campo "ACTIVADA" que indique si
         *  la llamada SAM está establecida o no, con dos posibles valores, "On" y "Off".
         *
         *  5.2.2.9 Bucles de la UGA
         *
         *  El servidor SAM debe poder enviar un mensaje M_SET_BUCLE al S-CWP en modo
         *  Medición para solicitar al S-CWP que active o desactive un bucle de la UGA.
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parseRespLlamadaSAM(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_SET_BUCLE_SAM.
         *  Con la respuesta del puesto comunicando si se ha establecido el bucle.
         *
         *  5.2.2.9 Bucles de la UGA
         *
         *  El mensaje de respuesta M_SET_BUCLE debe contener los mismos campos
         *  "NBUCLE" y "ESTADO".
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parseSetBucleSAM(const MSG_TH *pMsg);

        /**
         * @brief Procesa un mensaje del tipo
         *  MSG_TH::cab.idMsg == E_ID_MSG::M_PRESENCIA.
         *
         * @param pMsg Mensaje en el endian del anfitrión.
         */
        void parsePresencia(const MSG_TH *pMsg);

        /**
         * @brief Cancela la ejecución de la batería actual notificándoselo a la máquina de
         *  estados la que finalmente envíará el mensaje 6.1.8 EvtSAM_MedicionCancelada.
         *
         *  Llama internamente a StateMachine::cancelBatteryExecution().
         */
        void cancelBatteryExecution(const QString & causa);

    public:

        /**
         * @brief Devuelve las configuraciones del objeto.
         *
         * @return JVariant
         */
        JVariant & settings();

        /**
         * @brief Devuelve las configuraciones del objeto.
         *
         * @return JVariant
         */
        const JVariant & constSettings() const;

        /**
         * @brief Obtiene el estado completo del SAM
         *
         * @return const SAM_DataBase
         */
        const SAM_DataBase & samDataBase() const;

        /**
         * @brief Obtiene el estado completo del SAM
         *
         * @return const SAM_DataBase
         */
        SAM_DataBase & samDataBase();

        /**
         * @brief Devuelve el estado de la máquina de estados.
         *
         * @return QVariant
         */
        QVariant samStateMachineInfo() const;

        /**
         * @brief Devuelve la batería identificada por idSesion e idBateria, o NULL si no existe.
         *
         * @param idSesion
         * @param idBateria
         * @return Battery
         */
        SAM::Battery *getBattery(uint idSesion, uint idBateria);

        /**
         * @brief Extrae el mensaje SAM contenido en el paquete `msgPacket`.
         *
         * @param msgPacket Paquete conteniendo un único mensaje SAM con el endian del anfitrión.
         *
         * @return const MSG_TH Puntero constante al contenido del paquete
         *  `msgPacket`. Será válido mientras que el paquete que se pasó a msgPacket no se modifique.
         * @note Sólo serán válidos (y accesibles) los primeros MSG_TH::cab.lng bytes.
         */
        static const MSG_TH * getSamMessage(const Packet msgPacket);

        /**
         * @brief Empaqueta un mensaje SAM al protocolo de red BigEndian.
         *
         * @param th Mensaje codificado con el endian del anfitrión.
         * @return Packet El campo origin() se devuelve vacío.
         * @code
         *  // Empaqueta con el protocolo de Red (Big Endian)
         *  Packet msgBE(packetMessageToBigEndian(msg), objectName());
         * @endcode
         */
        static Packet packetMessageToBigEndian(const MSG_TH & th);

        /**
         * @brief Envía el mensaje `th` al agente
         *  de tipo estadísticas (normalmente el puerto UDP 7010).
         *
         * @param th Mensaje a enviar en big endian.
         */
        static void sendToEstadServers(const MSG_TH & th, QString origin = QString());

        /**
         * @brief Envía el mensaje `th` a todos los servidores remotos
         *  de tipo supervisión (normalmente el puerto UDP 7002).
         *
         * @param th Mensaje a enviar en big endian.
         */
        static void sendToRemoteSupervisionServers(const MSG_TH & th, QString origin = QString());

        /**
         * @brief Envía el mensaje `th` a todos los servidores remotos
         *  de tipo patrón (normalmente el puerto UDP 7003).
         *
         * @param th Mensaje a enviar en big endian.
         */
        static void sendToRemotePatronServers(const MSG_TH & th, QString origin = QString());

        /**
         * @brief Procesa el mensaje `pMsg`
         *
         * @param pMsg Mensaje expresado en el endian del host
         */
        void processMessageSAM(const MSG_TH *pMsg);

    public:

        QStateMachine & stateMachine();

        /**
         * @brief Rellena la cabecera de un nuevo mensaje en respuesta a otro
         *  recibido previamente del que toma algunos valores.
         * @note Se asume que se usa la codificacion BigEndian para la estructura de salida.
         *
         * @param msgBE Referencia de la estrcutura cuya cabecera será rellenada en BigEndian.
         * @param msgME Estructura de la que coger algunos datos de la cabecera en MachineEndian.
         *  Si msgME.cab.portDestino == 0, tan solo coge el tipo de mensaje y su tamaño:
         *  msgME.cab.lng y msgME.cab.idMsg.
         * @param idMsg Identificador del mensaje.
         * @param lng longitud de la esctructura resultante
         * @param nMessage Número de mensaje que se va a usar.
         */
        void initResponseMessageBE(MSG_TH & msgBE, const MSG_TH & msgME);

        /**
         * @brief Analiza el paquete binario conteniendo uno o más mensajes
         *  de tipo MSG_TH codificados en BigEndian.
         *  Cada vez que encuentre un mensaje válido,
         *  llama a su vez a MessageProcessor::processMessage().
         * @param packet Paquete de datos con uno o más mensajes MSG_TH
         *  seguidos codificados en bigendian.
         * @param nRejectedBytes Dirección de una variable que será rellenada con el número
         *  de bytes que se han rechazado del análisis (basura). Usar 0 para no devolver datos.
         * @return int Número de paquetes SCV procesados.
         */
        static int parseBinaryPacket(QObject *object, Packet packet, int *nRejectedBytes = 0);

    public slots:

        virtual void init();
        virtual void quit();

        /**
         * @brief Analiza el paquete binario conteniendo uno o más mensajes
         *  de tipo MSG_TH codificados en BigEndian.
         *  Cada vez que encuentre un mensaje válido,
         *  llama a su vez a MessageProcessor::processMessage().
         * @param packet Paquete de datos con uno o más mensajes MSG_TH
         *  seguidos codificados en bigendian.
         * @return bool
         */
        int parseBinaryPacket(Packet packet);

        /**
         * @brief Procesa un mensaje SCV-SAM
         *
         * @param msgPacket Un mensaje SCV-SAM enpaquetado con el endian del anfitrión
         * @return bool Devuelve true si se ha podido procesar el mensaje.
         */
        bool processMessage(Packet msgPacket);

        /**
         * @brief Procesa las órdenes que se le envíen a éste objeto.
         * Ejemplo:
         * @code processCommand(".*", "quit"); // hace que el objeto se libere. @endcode
         * @param strCommand Orden que se da al objeto:
         *  - `quit` .- Provoca que el objeto se auto-destruya.
         *  - `info` .- Provoca que el objeto emita la señal info().
         * @param namePattern Patrón QRegEx con el que se comparará el nombre del objeto.
         *  Si no tiene un match completo, el comando será ignorado.
         * @see CommandObject::processCommand(const QStringList &)
         */
        void processCommand(QStringList command);

        /**
         * @brief Lanza una petición al TMCS para recibir los archivos de configuración.
         */
        void requestTMCSConfigurations(QString origin = QString());

        virtual void setAppSettings(QVariant settings, QStringList path);

        /**
         * @brief Actualiza los parámetros que dependen de la configuración de la aplicación.
         */
        void parseAppSettingsData();

        /**
         * @brief Envía la presencia PING para el Watchdog o el mensaje de STOP.
         *
         * @param bSendStop true->PING, false->STOP
         */
        void sendWatchdogPingOrStop(bool bSendStop = false);

        /**
         * @brief Envía la presencia para Estadísticas
         */
        void sendStatisticsPres();

        /**
         * @brief Envía el porcentaje del avance de la ejecución de la batería actual.
         *
         */
        void sendMeasureAdvance();

    signals:

        /**
         * @brief Será llamada cuando se reciba correctamente un archivo
         *  y haya sido escrito en la ruta indicada.
         *
         * @param filePath
         */
        void fileWritten(QString filePath);
    };

}

#endif // SAM_MessageProcessor_H
