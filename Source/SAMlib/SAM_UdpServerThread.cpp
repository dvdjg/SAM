#include "SAM_Log.h"
#include "SAM_UdpServerThread.h"


namespace SAM
{

    UdpServerThread::UdpServerThread(const Packet packet)
        : m_packet(packet)
    {
        TRACE(SERV);
    }

    UdpServerThread::~UdpServerThread()
    {
        TRACE(SERV);
    }

    void UdpServerThread::run()
    {
        TRACE(SERV);
        readResponse(m_packet);
    }

    bool UdpServerThread::readResponse(Packet packet)
    {
        TRACE(SERV);

        QString str(packet.byteArray());

        LOG_INFO(COMS) <<  "Recibido paquete UDP" << str << "desde "
            << packet.origin();

        // Preparar la respuesta...
        return true;
    }
}
