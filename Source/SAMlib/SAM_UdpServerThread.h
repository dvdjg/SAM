#ifndef SAM_UDPSERVERTHREAD_H
#define SAM_UDPSERVERTHREAD_H

#include "SAM_Packet.h"
#include <QRunnable>

namespace SAM
{

    /**
     * @brief Procesamiento de los mensajes recibidos por UDP.
     *  El uso habitual de esta clase es derivarla y reimplementar el método UdpServerThread::readResponse.
     */
    class UdpServerThread : public QRunnable
    {
    public:
        UdpServerThread(const Packet packet);
        virtual ~UdpServerThread();

        /// Asíncronamente lee datos del socket "socketDescriptor" y devuelve una respuesta
        virtual void run();

        /** @brief Función a sobrecargar cuando se derive una clase de UdpServerThread.
         *      Esta función será llamada cuando se reciban datos por el socket.
         *  @return bool true si se ha terminado el procesamiento.
         */
        virtual bool readResponse(Packet packet);

    private:
        Packet m_packet;
    };

}

#endif // SAM_UDPSERVERTHREAD_H
