
#include "SAM_Log.h"
#include "SAM_MainTask.h"
#include "SAM_Sniffer.h"
#include "SAM_UdpServerSniffer.h"


namespace SAM
{

    UdpServerSniffer::UdpServerSniffer(const Packet packet)
        : UdpServerThread(packet)
    {
    }

    bool UdpServerSniffer::readResponse(Packet packet)
    {
        TRACE(SERV);

        Sniffer * pSniffer = g_pMainTask->getSniffer();

        if(pSniffer) {
            QMetaObject::invokeMethod( pSniffer, "parseBinaryPacket",
                                       Qt::QueuedConnection,
                                       Q_ARG( Packet, packet ) );
        }

        return true;
    }

}
