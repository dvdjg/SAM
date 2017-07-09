#include <QHostAddress>
#include <QTcpSocket>

#include "SAM_Log.h"
#include "SAM_MainTask.h"
#include "SAM_TcpServerCommandProcessor.h"


namespace SAM
{
    TcpServerCommandProcessor::TcpServerCommandProcessor(int socketDescriptor, QObject *parent) :
        TcpServerThread(socketDescriptor)
    {
        Q_UNUSED(parent);
    }

    bool TcpServerCommandProcessor::readResponse(Packet packet)
    {
        TRACE(COMS);

        QByteArray response;
        QString strPacket(packet.byteArray());

        bool bRet = g_pMainTask->processCommand(packet, response);

        if(!bRet)
        {
            QString strUrl = QString("tcp://%1:%2")
                    .arg(socket()->peerAddress().toString())
                    .arg(socket()->peerPort());

            LOG_WARN(COMS) <<  "Recibido la orden inválida" >> strPacket.trimmed()
                << "desde" >> strUrl;
        }
        else if(response.length() > 0)
        {
            // Escribir la respuesta por el mismo socket
            qint64 nWritten = socket()->write(response);

            LOG_INFO(COMS) << "Escritos" << nWritten
                           << "bytes en respuesta al comando" >> strPacket.trimmed();
        }

        return true;
    }
}
