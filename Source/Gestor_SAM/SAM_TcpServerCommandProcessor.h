#ifndef SAM_TCPSERVERCOMMANDPROCESSOR_H
#define SAM_TCPSERVERCOMMANDPROCESSOR_H

#include "SAM_TcpServerThread.h"

namespace SAM
{
    /**
     * @brief Especialización de SAM::TcpServerThread para procesar órdenes
     *  recibidad por TCP.
     */
    class TcpServerCommandProcessor : public TcpServerThread
    {
    protected:
        /**
         *  @brief Esta función será llamada mientras se reciban datos por el socket.
         *
         *  @param packet Datos leidos del socket.
         *  @return bool true si se ha terminado el procesamiento y
         *      no se requiere esperar a que lleguen más datos.
         */
        virtual bool readResponse(Packet packet);

    public:
        /**
         * @brief Constructor compatible con TcpServerThread.
         *
         * @param socketDescriptor Descriptor del socket.
         */
        TcpServerCommandProcessor(int socketDescriptor, QObject * parent = 0);
    };
}

#endif // SAM_TCPSERVERCOMMANDPROCESSOR_H
