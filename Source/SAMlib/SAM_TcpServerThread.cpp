#include <QtNetwork>
#include <QTcpSocket>

#include "SAM_Log.h"
#include "SAM_TcpServerThread.h"


namespace SAM
{

    TcpServerThread::TcpServerThread(int socketDescriptor, QObject *parent)
        : m_socketDescriptor(socketDescriptor)
    {
        TRACE(SERV);
        Q_UNUSED(parent);
    }

    TcpServerThread::~TcpServerThread()
    {
        TRACE(SERV);
    }

    QString TcpServerThread::getPeerId()
    {
        TRACE(SERV);

        if(m_peerId.isEmpty() && m_pTcpSocket && m_pTcpSocket->isValid())
            m_peerId = QString("tcp://%1:%2")
                    .arg(m_pTcpSocket->peerAddress().toString())
                    .arg(m_pTcpSocket->peerPort());

        return m_peerId;
    }

    QString TcpServerThread::getPeerId() const
    {
        return m_peerId;
    }

    void TcpServerThread::run()
    {
        TRACE(SERV);

        QTcpSocket tcpSocket;
        m_pTcpSocket = &tcpSocket;

        if (!tcpSocket.setSocketDescriptor(m_socketDescriptor))
        {
            LOG_ERROR(COMS) <<  "No se ha podido acceder al socket TCP para atender la conexión entrante:"
                << tcpSocket.errorString();

            return;
        }

        bool continueReading = true;
        getPeerId(); ///< Que se confeccione el nombre

        while(continueReading)
        {
            if(tcpSocket.waitForReadyRead(1000)) // Esperar a que se reciba el mensaje
            {
                /// Lee como mucho 1Mb para evitar ataques de envíos muy grandes.
                QByteArray ba(tcpSocket.read(1<<20));
                Packet packet(ba, getPeerId());

                continueReading = !readResponse(packet);

                continueReading = continueReading
                        && tcpSocket.state() == QTcpSocket::ConnectedState
                        && tcpSocket.isValid();
            }
            else
            {
                LOG_WARN(COMS) <<  "Timeout mientras se leían datos desde" << getPeerId();

                continueReading = false;
            }
        }

        if(tcpSocket.state() != QAbstractSocket::UnconnectedState)
        {
            /// Esperar a que se termine de escribir la respuesta (si la hubiera)
            tcpSocket.waitForBytesWritten(1000);

            /// Liberar el socket
            tcpSocket.disconnectFromHost();

            if(tcpSocket.state() != QAbstractSocket::UnconnectedState
                    && !tcpSocket.waitForDisconnected(1000)) // Espera un poco para desconectar
            {
                LOG_WARN(COMS) <<  "Timeout mientras se intentaba cerrar el socket conectado a"
                               << getPeerId() << ". Posible pérdida de datos.";
            }
        }
    }

    bool TcpServerThread::readResponse(Packet packet)
    {
        TRACE(SERV);

        QString strPacket(packet.byteArray());

        LOG_INFO(COMS) <<  "Recibido desde" << getPeerId() << ": " << strPacket;

        /// Escribir la respuesta por el mismo socket
        qint64 nWritten = socket()->write(packet.byteArray());

        LOG_INFO(COMS) <<  "Escritos" << nWritten << "bytes.";

        return true;
    }

    QTcpSocket * TcpServerThread::socket() { return m_pTcpSocket; }
}
