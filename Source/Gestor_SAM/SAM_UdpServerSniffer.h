#ifndef SAM_UDPSERVERSNIFFER_H
#define SAM_UDPSERVERSNIFFER_H

#include "SAM_Packet.h"
#include "SAM_UdpServerThread.h"

namespace SAM
{

    /**
     * @brief Recepción de los mensajes binarios de tipo SAM_Declaration::MSG_TH recibidos desde:
     *  TMCS, Posición, Gateway,...
     *  Puede haber 0 o más instancias de SAM::UdpServerSniffer,
     *  cada uno escuchando en diferentes puertos.
     */
    class UdpServerSniffer : public UdpServerThread
    {
    public:
        UdpServerSniffer(const Packet packet);

        /**
         *  @brief Esta función será llamada cuando se reciban datos por el socket.
         *  Los datos recibidos deben estar en el formato SAM tal como se definen
         *  en SAM_Declaration::MSG_TH.
         *  @param packet Paquete de datos que trae el datagrama UDP.
         *  @return bool true si se ha terminado el procesamiento.
         */
        virtual bool readResponse(Packet packet);
    };

}

#endif // SAM_UDPSERVERSNIFFER_H
