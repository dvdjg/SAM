#ifndef SAM_MAINTASK_H
#define SAM_MAINTASK_H

#include "SAM_Packet.h"
#include "SAM_SignalHandler.h"
#include "SAM_JVariant.h"
#include "SAM_CommandObject.h"

// Etiqueta que se usará para la raíz de la configuración local.
#define CONF_LOCAL "LOCAL"

#define TERMINATE_DELAY 500

/**
  * @brief Namespace principal donde se implementa la funcionalidad requerida por el Gestor de SAM.
  */
namespace SAM
{
    class MessageProcessor;
    class Sniffer;
    class Ping;

    QString infoString();

    /**
     * @brief Tarea principal del SAM. Crea el resto de los hilos.
     */
    class MainTask : public CommandObject
    {
        Q_OBJECT

        Q_PROPERTY( QStringList arguments READ arguments )
        Q_PROPERTY( QString infoString READ infoString )
        Q_PROPERTY( QString suitableIpAddress READ suitableIpAddress )
        Q_PROPERTY( QString suitableBroadcastAddress READ suitableBroadcastAddress )
        Q_PROPERTY( QString datetimeFormat READ datetimeFormat )
        Q_PROPERTY( uint suitableEqId READ suitableEqId )
        Q_PROPERTY( QVariant memoryInfo READ memoryInfo )
        Q_PROPERTY( QVariant cpuInfo READ cpuInfo )

    protected:
        Q_DISABLE_COPY(MainTask)

    public:
        explicit MainTask(QObject *parent = 0);

        /** Obtiene una copia del objeto encargado de almacenar las configuraciones
          @note Para leer configuraciones:
            QReadLocker locker(getSettingsLocker());
            QString iniPath = getSettings()[CONF_LOCAL]["SETTINGS"]["INI_PATH"];
        */

        /**
         * @brief Registra un servidor remoto al que enviarle paquetes
         *  mediante sendPacketToRemoteServers().
         *
         * @param serverType Tipo de servidor. Actualmente definidos 'TMCS', 'ESTAD'. y 'SAGE'.
         * @param serverUrl Url del servidor al que se está registrando.
         * @return QObject Puntero al servidor recién creado.
         */
        QObject *registerRemoteServer(QString serverType, QUrl serverUrl);

        /**
         * @brief Procesa un comando. Esta función es reentrante y thread-safe.
         *
         * @param packetCommand Comando
         * @param response Aquí se recibe la respuesta
         * @return bool true si se pudo procesar bien.
         */
        bool processCommand(Packet packetCommand, QByteArray & response);

        /**
         * @brief Devuelve el formato que se va a usar para formatear fechas en
         *  el sistema interno de objetos.
         *
         * @return QString
         */
        QString datetimeFormat() const { return m_datetimeFormat; }

        /**
         * @brief Devuelve varios indicadores relativos al uso de memoria.
         *
         * @return QVariant
         */
        QVariant memoryInfo() const;

        /**
         * @brief Devuelve varios indicadores relativos al uso de CPU.
         *
         * @return QVariant
         */
        QVariant cpuInfo() const;

        /**
         * @brief Devuelve el objeto encargado de procesar los mensajes SAM o `NULL`
         *  si no el objeto no está activo.
         *
         * @return MessageProcessor
         */
        MessageProcessor * getMessageProcessor() { return m_pMessageProcessor; }

        /**
         * @brief Devuelve el objeto encargado de esnifar mensajes de SAM o `NULL`
         *  si no el objeto no está activo.
         *
         * @return Sniffer
         */
        Sniffer * getSniffer() { return m_pSniffer; }
        const Sniffer * getSniffer() const { return m_pSniffer; }

        /**
         * @brief Configura el objeto que se mantendrá a la escucha de órdenes.
         *
         * @param commandsPort Puerto de escucha. La interfaz es `Any = 0.0.0.0`
         */
        void configCommandsListener(quint16 commandsPort);

        /**
         * @brief Configura el sistema de trazas
         *
         * @param trazas
         */
        void configTraces(const JVariant& trazas);

        /**
         * @brief Configuración de la RED de Control/Supervisión
         *  según el punto *5.2.6 RED* del SRD.
         *
         * @param T_SUPER
         * @param szType "CONTROL" o "SUPERVISION"
         */
        void configCommunications(JVariant T_SUPER, const char* szType);

        /**
         * @brief Configura los sniffers que se especifiquen en el parámetro
         *
         * @param SNIFFERS Lista de cadenas con las URL para escuchar.
         */
        void configSniffers(JVariant SNIFFERS);

        /**
         * @brief Configura los inyectores que se especifiquen en el parámetro INJECTORS.
         *  Creará una instancia de Injector en su propio hilo, por cada inyector detectado.
         *
         * @param INJECTORS
         */
        void configInjectors(JVariant INJECTORS);

        /** Envía el paquete `packet` asíncronamente a todos los servidores del tipo ESTAD.*/
        void sendPacketToRemoteEstadServers(Packet packet);

        /** Envía el paquete `packet` asíncronamente a todos los servidores del tipo SAGE.*/
        void sendPacketToRemoteSageServers(Packet packet);

        /** Envía el paquete `packet` asíncronamente a todos los servidores del tipo Control-TMCS.*/
        void sendPacketToRemoteControlServers(Packet packet);

        /** Envía el paquete `packet` asíncronamente a todos los servidores del tipo Supervisión.*/
        void sendPacketToRemoteSupervisionServers(Packet packet);

        /** Envía el paquete `packet` asíncronamente a todos los servidores del tipo Patrón.*/
        void sendPacketToRemotePatronServers(Packet packet);

        /** Identificador del equipo en el que se ejecuta éste programa. */
        uint suitableEqId() const { return m_suitableIdEq; }  /// @todo Usar el ID real del equipo

        /** Dirección IP por defecto del Gestor SAM compatible con la del TMCS (misma subred) */
        QString suitableIpAddress() const { return m_suitableIpAddress; }

        /** Dirección Broadcast por defecto del Gestor SAM compatible con la del TMCS */
        QString suitableBroadcastAddress() const { return m_broadcastIpAddress; }

        /**
         * @brief Argumentos pasados a la aplicación.
         *
         * @return QStringList
         */
        QStringList arguments() const;

        /**
         * @brief Devuelve una cadena con información sobre el entorno de ejecución de la aplicación.
         *
         * @return QString
         */
        QString infoString() const;

        /**
         * @brief Encuentra una interfaz IP desde la que se pueda escuchar como Server.
         */
        void findSuitableIpAddress();

    public slots:
        /**
         * @brief Lee los archivos de configuración si se se han habilitado mediante
         *  CONFIG_PATH. Llama internamente a la función MainTask::loadHwConfigFile().
         */
        void loadConfigFiles();

        /**
         * @brief Lee el archivo de configuración `config_sdcxxi_hw.txt`
         *
         * @param filePath Ruta al arhivo comprimido en zip. Ej: `./config_sdcxxi_hw.txt.zip`
         */
        void loadHwConfigFile(QString filePath);

    protected:
        /** Obtiene una referencia al objeto encargado de almacenar las configuraciones
          @note Para escribir configuraciones:

            settings()[CONF_LOCAL]["SETTINGS"]["INI_PATH"] = "path/to/dir";

          @note Para leer configuraciones:

            QString iniPath = settings()[CONF_LOCAL]["SETTINGS"]["INI_PATH"];
        */
        //JVariant & appSettings() { return m_appSettings; }

        /**
         * @brief Establece algunas configuraciones por defecto.
         */
        void initSettings();

        /**
         * @brief Analiza los modificadores de entrada. Ej: ./SAM_Gestor -i/home/puesto/SAM_Gestor.ini
         */
        void parseArguments();

        /**
         * @brief Aplica las configuraciones.
         *
         * @param settings
         */
        void applySettings(const JVariant &appSettings);

        /**
         * @brief Registra en el sistema objetos derivados de CommandObject para
         *  que se ejecuten en su propio hilo y mantengan un sistema de comunicaciones.
         *  Se puede consultar el estado de todos estos objetos dentro de las
         *   configuraciones en: `INFO/runningObjects`
         * @param commandObject
         * @return QThread Hilo que se ha creado para atender al objeto.
         */
        QThread* registerCommandObject(CommandObject * commandObject);

        /**
         * @brief Configura el sistema de supervisión de hosts mediante pings.
         */
        void configureHostsPings();

    public:
        /**
         * @brief Registra en el sistema objetos derivados de CommandObject  para
         *  que mantengan un sistema de comunicaciones.
         *  Se puede consultar el estado de todos estos objetos dentro de las
         *   configuraciones en: `INFO/runningObjects`.
         *  En caso de que el `commandObject` vaya a tener su propio hilo, usar
         *  la función MainTask::registerCommandObject() en su lugar.
         *
         * @param commandObject
         */
        void registerLocalCommandObject(CommandObject * commandObject);

    public slots:
        /**
         * @brief Punto de entrada de la aplicación.
         */
        void run();

        /**
         * @brief Petición de terminación del proceso.
         *  Este slot envía la señal terminated(). Si msec = 0, envía también la señal finished().
         *  Si msec > 0 programa una nueva llamada a quit() pasados msec milisegundos.
         *  El tiempo especificado en msec se usa para dar oportunidad a los hilos a salir ordenadamente.
         *
         * @param msec Número de milisegundos que trascurrirán desde que se genera
         *  la señal terminated() hasta que se genere señal de finished().
         */
        void quit(int msec = 0);

        /**
         * @brief Programa una llamada a quit() pasados msec milisegundos.
         *
         * @param msec Milisegundos de demora hasta que se llame a quit().
         */
        void delayedQuit(int msec);

        /**
         * @brief Recepción de información de objetos.
         *
         * @param who Identifica al objeto mediante una cadena de caracteres
         * @param what Identifica qué se está comunicando.
         * @param data Datos asociados a la información que se comunique.
         */
        void info(QString who, QString what, QVariant data);

        /**
         * @brief Realiza tareas de mantenimiento
         *
         */
        void maintenance();

        void error(QString err);

    private slots:
        /**
         * @brief Manejador de señales del S.O.
         *
         * @param signal Número de la señal recibida tal como se indica en SignalHandler::SIGNALS.
         */
        void handleSignal(int signal);

    signals:
        /**
         * @brief Señal que se usa para avisar de que se está haciendo una salida ordenada
         *  del proceso. Se enviará cuando se reciba un comando de 'quit' o una señal Ctrl+C del S.O.
         */
        void terminated();

        /** Envía el paquete `packet` asíncronamente a todos los servidores del tipo ESTAD.*/
        void sendPacketToEstadServers(Packet packet);

        /** Envía el paquete `packet` asíncronamente a todos los servidores del tipo SAGE.*/
        void sendPacketToSageServers(Packet packet);

        /** Envía el paquete `packet` asíncronamente a todos los servidores del tipo Control-TMCS.*/
        void sendPacketToControlServers(Packet packet);

        /** Envía el paquete `packet` asíncronamente a todos los servidores del tipo Supervisión.*/
        void sendPacketToSupervisionServers(Packet packet);

        /** Envía el paquete `packet` asíncronamente a todos los servidores del tipo Patrón.*/
        void sendPacketToPatronServers(Packet packet);

        /** Envío de la ordene command a todos los servidores cuya URL destino cumpla con el patrón "urlPattern".
          Ej.: sendCommandToObjects("udp:.*", "quit"); // Petición de terminación a todos los servidores UDP.
          */
        void sendCommandToObjects(QString command, QString namePattern = ".*");

        /**
         * @brief Emitida cuando la configuración de la aplicación cambia.
         *
         * @param appSettings
         */
        void appSettingsChanged(QVariant appSettings, QStringList path = QStringList());

    protected:

        /**
         * @brief m_suitableIpAddress Dirección IP por defecto del Gestor SAM.
         *  Es obtenida mediante la comparación de las interfaces de la máquina con
         *  respecto a las IPs del archivo de configuración. Se elige la primera que
         *  sea compatible.
         */
        QString m_suitableIpAddress; ///< Dirección IP por defecto del Gestor SAM
        QString m_broadcastIpAddress; ///< Dirección broadcast por defecto del Gestor SAM
        QString m_datetimeFormat; ///< Formato de fecha. Ej: "yyyy-MM-dd_hh:mm:ss.zzz"
        uint m_suitableIdEq; ///< Identificador del equipo donde se ejecuta éste proceso.

        /**
         * @brief m_messageProcessor Puntero al objeto que procesa toda la mensajería
         *  del SAM. Se mantiene una referencia aquí para que las clases puedan acceder
         *   directamente a su interfaz pública.
         */
        MessageProcessor * m_pMessageProcessor;

        /**
         * @brief m_sniffer Puntero al objeto que hace de sniffer
         */
        Sniffer * m_pSniffer;

        Ping * m_pPingToHosts;
    };

    extern MainTask * g_pMainTask;
}

#endif // SAM_MAINTASK_H
