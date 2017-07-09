#include "SAM_Log.h"
#include "SAM_UdpServerMessageProcessor.h"
#include "SAM_MessageProcessor.h"
#include "SAM_MainTask.h"


namespace SAM
{

    UdpServerMessageProcessor::UdpServerMessageProcessor(const Packet packet)
        : UdpServerThread(packet)
    {
    }

    bool UdpServerMessageProcessor::readResponse(Packet packet)
    {
        TRACE(SERV);

        MessageProcessor * pMessageProcessor = g_pMainTask->getMessageProcessor();

        if(pMessageProcessor) {
            QMetaObject::invokeMethod( pMessageProcessor, "parseBinaryPacket",
                                       Qt::QueuedConnection,
                                       Q_ARG( Packet, packet ) );
        }

        return true;
    }

}
