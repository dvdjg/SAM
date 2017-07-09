#include <QHostAddress>

#include "SAM_Log.h"
#include "SAM_TcpUdpServerPrivate.h"
#include "SAM_TcpUdpServer.h"

namespace SAM
{
    TcpUdpServerPrivate::TcpUdpServerPrivate()
      : m_readBufferSize(0),
        m_incomingConnectionsCount(0),
        m_retryTimeLapse(10000), // 10s
        m_pTimerRetry(NULL)
    {
        TRACE(SERV);
    }

    TcpUdpServer::TcpUdpServer(QObject *parent, int i) :
        CommandObject(parent, i)
    {
        TRACE(SERV);
    }
    TcpUdpServer::TcpUdpServer(QObject *parent) :
        CommandObject(parent, -1)
    {
        TRACE(SERV);
        m_ptr.reset(new TcpUdpServerPrivate);
    }

    TcpUdpServer::~TcpUdpServer()
    {
        TRACE(SERV);
    }

    void TcpUdpServer::setReadBufferSize(int readBufferSize)
    {
        TRACE(SERV);

        Q_D(TcpUdpServer);

        if(readBufferSize < (1<<10)) {
            LOG_WARN(COMS) << "Se está estableciendo en la interfaz" >> d->m_url.toString()
                           << "un tamaño de lectura de tramas" << readBufferSize
                           << "demasiado pequeño.";
        }

        d->m_readBufferSize = readBufferSize;
    }

    const QUrl & TcpUdpServer::url() const
    {
        Q_D(const TcpUdpServer);

        return d->m_url;
    }

    void TcpUdpServer::setUrl(const QUrl &url)
    {
        Q_D(TcpUdpServer);

        d->m_url = url;
    }

    int TcpUdpServer::retryTimeLapse() const
    {
        Q_D(const TcpUdpServer);

        return d->m_retryTimeLapse;
    }

    void TcpUdpServer::setRetryTimeLapse(int retryTimeLapse)
    {
        Q_D(TcpUdpServer);

        d->m_retryTimeLapse = retryTimeLapse;
    }

    int TcpUdpServer::readBufferSize() const
    {
        Q_D(const TcpUdpServer);

        return d->m_readBufferSize;
    }

    int TcpUdpServer::incomingConnectionsCount() const
    {
        Q_D(const TcpUdpServer);

        return d->m_incomingConnectionsCount;
    }

}
