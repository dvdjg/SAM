#ifndef SAM_TCPSERVERTHREAD_H
#define SAM_TCPSERVERTHREAD_H

#include <QRunnable>

#include "SAM_Packet.h"

class QTcpSocket;

namespace SAM
{

    /** @brief Tarea que procesa los mensajes TCP recibidos por el servidor.
     *  Para usar esta clase, hay que derivar otra e implementar la función TcpServerThread::readResponse.
     */
    class TcpServerThread : public QRunnable
    {
     public:
        TcpServerThread(int socketDescriptor, QObject * parent = 0);
        virtual ~TcpServerThread();

        /**
         * @brief Síncronamente lee datos del socket "socketDescriptor" y devuelve una respuesta.
         */
        virtual void run();

        /**
         * @brief Devuelve el identificador del que envió el paquete TCP.
         *
         * @return QString
         */

        QString getPeerId() const;

        /**
         * @brief Confecciona y devuelve el identificador del que envió el paquete TCP.
         *
         * @return QString
         */
        QString getPeerId();

    protected:

        /**
         *  @brief Función a sobrecargar cuando se derive una clase de TcpServerThread.
         *      La implementación por defecto devuelve un eco de la entrada por el mismo socket.
         *      Esta función será llamada mientras se reciban datos por el socket.
         *  @param packet Datos leidos del socket.
         *  @return bool true si se ha terminado el procesamiento y
         *      no se requiere esperar a que lleguen más datos.
         */
        virtual bool readResponse(Packet packet);


        /**
         * @brief Accede al socket directamente para comunicarse con el otro extremo.
         *
         * @return QTcpSocket * Socket de comunicaciones
         */
        QTcpSocket * socket();

    protected:
        int m_socketDescriptor;  /**< Descriptor numérico del socket que es pasado desde el hilo que creó éste objeto. */
        QTcpSocket * m_pTcpSocket;  /**< Socket obtenido desde el descriptor m_socketDescriptor */
        QString m_peerId;  /**< Identificación del extremo que ha realizado la conexión */
    };

}

#endif // SAM_TCPSERVERTHREAD_H
