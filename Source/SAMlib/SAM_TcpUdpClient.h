#ifndef SAM_TCPUDPCLIENT_H
#define SAM_TCPUDPCLIENT_H

#include <QUrl>
#include <QAbstractSocket>

#include "SAM_Packet.h"
#include "SAM_CommandObject.h"


namespace SAM
{
    // Hilos al estilo: http://mayaposch.wordpress.com/2011/11/01/how-to-really-truly-use-qthreads-the-full-explanation/

    class TcpUdpClientPrivate;

    /**
         @brief Cliente para enviar paquetes de datos a Servidores de tipo UDP o TCP.
         @todo A implementar si fuera necesario:
       - void hostFound();
       - void disconnected();
       - void stateChanged(QAbstractSocket::SocketState);
       - void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);
    */
    class TcpUdpClient : public CommandObject
    {
        Q_OBJECT

        Q_PROPERTY( bool perPacketReopen READ perPacketReopen WRITE setPerPacketReopen )
        Q_PROPERTY( QUrl url READ url )
        Q_PROPERTY( int maxBufferPacketCount READ maxBufferPacketCount WRITE setMaxBufferPacketCount )
        Q_PROPERTY( int bufferPacketCount READ bufferPacketCount )
        Q_PROPERTY( qint64 packetsTaken READ packetsTaken )
        Q_PROPERTY( qint64 bytesTaken READ bytesTaken )
        Q_PROPERTY( qint64 packetsWritten READ packetsWritten )
        Q_PROPERTY( qint64 bytesWritten READ bytesWritten )
        Q_PROPERTY( qint64 bytesWrittenAck READ bytesWrittenAck )

    protected:
        J_DECLARE_PRIVATE(TcpUdpClient)
        Q_DISABLE_COPY(TcpUdpClient)

    public:
        /** Crea el objeto encargado de enviar paquetes al servidor especificado por la URL "serverUrl".
            @param serverUrl URL del servidor. Ejemplo:
            * "udp://10.40.64.6:7012"
            * "tcp://10.40.64.10:7020"
            @param parent Objeto padre.
          */
        TcpUdpClient(const QUrl &serverUrl, QObject *parent = 0);

        virtual ~TcpUdpClient();

        /**
         * @brief Lee si se tiene que reabrir el socket por cada paquete que se envía.
         *
         * @return bool
         */
        bool perPacketReopen() const;

        /**
         * @brief Establece si se tiene que reabrir el socket por cada paquete que se envía.
         *
         * @param perPacketReopen
         */
        void setPerPacketReopen(bool perPacketReopen);

        /**
         * @brief Mantiene el socket vivo. Configura el socket para que envíe una serie
         *  de mensajes de control que comprueban que la conexión sigue existiendo.
         *  En caso de que se rompa el socket generará la señal de que el socket se ha desconectado
         *  QAbstractSocket::disconnected().
         *
         * @param maxIdle Tiempo máximo en segundos que transcurrirá desde la última actividad
         *  detectada hasta el inicio de la petición de mensajes de ACK de control.
         *
         *  @note Para que se habilite éste mecanismo los parámetros socketAliveMaxIdle y
         *  socketAliveInterval deben ser mayores que 0.
         *  @note Éste parámetro será efectivo a partir de la siguiente conexión que se realice
         *  sobre el socket.
         */
        void setSocketAliveMaxIdle(int maxIdle = 20);

        /**
         * @brief Mantiene el socket vivo. Configura el socket para que envíe una serie
         *  de mensajes de control que comprueban que la conexión sigue existiendo.
         *  En caso de que se rompa el socket generará la señal de que el socket se ha desconectado
         *  QAbstractSocket::disconnected().
         *
         * @param interval Tiempo en segundos que transcurrirá entre peticiones de ACKs.
         *
         *  @note Para que se habilite éste mecanismo los parámetros socketAliveMaxIdle y
         *  socketAliveInterval deben ser mayores que 0.
         *  @note Éste parámetro será efectivo a partir de la siguiente conexión que se realice
         *  sobre el socket.
         */
        void setSocketAliveInterval(int interval = 10);

        /**
         * @brief Devuelve el URL con el que se configuró el socket.
         *
         * @return QUrl
         */
        const QUrl & url() const;

        bool operator ==(const QUrl & url) const;
        bool operator ==(const TcpUdpClient & other) const;

        /// Máximo número paquetes que se almacenarán en el buffer.
        int maxBufferPacketCount() const;

        /**
         * @brief Establece el tamaño máximo del buffer en número paquetes.
         *  Esta función puede desencadenar pérdida de datos si quedaran más
         *  paquetes sin enviar que el tamaño especificado.
         *
         * @param maxElems
         */
        void setMaxBufferPacketCount(int maxElems);


        /// Número paquetes que están almacenados en el buffer.
        int bufferPacketCount() const;

        /**
         * @brief Número de paquetes recibidos por éste objeto.
         * @return qint64
         */
        qint64 packetsTaken() const;

        /**
         * @brief Número de bytes recibidos por éste objeto.
         * @return qint64
         */
        qint64 bytesTaken() const;

        /**
         * @brief Número de paquetes escritos a la interfaz.
         * @note Podría ocurrir que no todos los paquetes que se escriban a la
         *  interfaz lleguen a su destinatario. Ésto podría ocurrir con los paquetes UDP
         *  o por errores de red.
         * @return qint64
         */
        qint64 packetsWritten() const;

        /**
         * @brief Número de bytes escritos.
         * @note Podría ocurrir que no todos los bytes que se escriban a la
         *  interfaz lleguen a su destinatario. Ésto podría ocurrir con los paquetes UDP
         *  o por errores de red.
         *  Para saber en TCP la cantidad real de bytes que se ha escrito con éxito
         *  utilice la función bytesWrittenAck().
         * @return qint64
         */
        qint64 bytesWritten() const;

        /**
         * @brief Número de bytes escritos y confirmados.
         * @note Sólo se puede confirmar la escritura si se usa el protocolo TCP.
         * @return qint64
         * @see bytesWritten()
         */
        qint64 bytesWrittenAck() const;

    public slots:

        /**
         * @brief Llamar para hacer una terminación ordenada del objeto
         *  (cerrando el socket).
         * @note Esta función llama al método CommandObject::quit().
         */
        virtual void quit();

        /** Se encarga de enviar el paquete al destinatario.
          */
        void sendPacket(Packet packet);

        /**
         * @brief Procesa las órdenes que se le envíen a éste objeto.
         *
         * @param command Comando separado por los espacios.
         * @sa processCommand(QString strCommand, QString namePattern)
         */
        void processCommand(QStringList command);

        /**
         * @brief Realiza la conexión del socket con el anfitrión TcpUdpClient::url()
         *   si su estado actual es QAbstractSocket::UnconnectedState.
         */
        void connectToHost();

    private slots:

        /** Petición para enviar la cola de paquetes al Servidor mediante TcpUdpClient::flushPendingPackets().
         *  Se llama automáticamente cuando haya datos en la cola.
         *  Si el socket está cerrado, lo abre para realizar la comunicación de datos.
         *  @sa TcpUdpClient::flushPendingPackets()
         */
        void sendPacketQueue();

        /** Se llama cuando se ha conseguido conectar con el Servidor.
          */
        void connected();

        /** Se llama cuando se ha conseguido desconectar con el Servidor.
          */
        void disconnected();

        /** Se llama cuando el Servidor devuelve datos.
          */
        void readyRead();

        /** Función auxiliar para mostrar algunos de los errores más comunes que emiten los QAbstractSocket.
          */
        void socketError(QAbstractSocket::SocketError socketError);

        /**
         * @brief Llamada cuando el socket consiga escribir bytes al destino
         *
         * @param nBytes Número de bytes que se han escrito.
         */
        void bytesWritten ( qint64 nBytes );

    signals:

        void error(QString err);

    protected:
        /**
         * @brief Asegura que el tamaño del buffer de paquetes a enviar no supera
         *  el tamaño m_maxPacketCount establecido con setMaxBufferElementCount().
         */
        void checkBufferSize();

        /**
         * @brief Desconecta el socket del Anfitrión (Servidor) en el que estaba conectado.
         *  Esta función espera un tiempo a que se terminen de enviar los datos por el socket.
         *  En caso de no poder desconectarse en un tiempo prudencial, aborta la conexión
         *  y devuelve false.
         * @return bool Retorna false si no ha conseguido cerrarse adecuadamente.
         */
        bool disconnectFromHost(int msecs);

        /** Envía todos los mensajes pendientes si los hubiera.
          */
        void flushPendingPackets();
    };

}

#endif // SAM_TCPUDPCLIENT_H
