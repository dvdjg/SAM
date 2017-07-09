#ifndef SAM_TCPUDPCLIENTPRIVATE_H
#define SAM_TCPUDPCLIENTPRIVATE_H

#include <QUrl>
#include <QQueue>

#include "SAM_Packet.h"
#include "SAM_CommandObjectPrivate.h"

class QAbstractSocket;

namespace SAM
{
    // Hilos al estilo: http://mayaposch.wordpress.com/2011/11/01/how-to-really-truly-use-qthreads-the-full-explanation/

    class TcpUdpClientPrivate : public CommandObjectPrivate
    {
        friend class TcpUdpClient;

    protected:
        TcpUdpClientPrivate(const QUrl &serverUrl);

    private slots:

        /** Petición para enviar la cola de paquetes al Servidor.
          Se llama automáticamente cuando haya datos en la cola.
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

    protected:

        /** Puntero al socket.
          * - ´QUdpSocket´ Si el socket es UDP.
          * - ´QTcpSocket´ Si el socket es TCP.
          */
        QAbstractSocket *m_inetSocket;

        QUrl m_url; ///< URL del destinatario. Ej: "udp://10.40.64.6:7012"

        QQueue<Packet> m_packetQueue; ///< Cola de paquetes para enviar.

        int m_maxPacketCount; ///< Máximo número paquetes que se almacenarán en el buffer.

        /** Determina si se tiene que reabrir el socket por cada paquete que se envía.
            Por defecto: true
        */
        bool m_bPerPacketReopen;

        qint64 m_packetsTaken; /**< Número de paquetes recibidos. */
        qint64 m_bytesTaken; /**< Número de bytes recibidos. */
        qint64 m_packetsWritten; /**< Número de paquetes escritos. */
        qint64 m_bytesWritten; /**< Número de bytes escritos. */
        qint64 m_bytesWrittenAck; /**< Número de bytes escritos y confirmados. */

        /** Tiempo máximo en segundos que transcurrirá desde la última actividad
         *  detectada hasta el inicio de la petición de mensajes de ACK de control.
         */
        int m_maxIdle;
        int m_interval; /**< Tiempo en segundos que transcurrirá entre peticiones de ACKs.*/

        QTimer* m_pTimer;
    };
}

#endif // SAM_TCPUDPCLIENTPRIVATE_H
