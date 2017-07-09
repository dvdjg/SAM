#include <QTimer>
#include <QThreadPool>
#include <QUrl>

#include "SAM_Log.h"
#include "SAM_TcpServerThread.h"
#include "SAM_TcpServer.h"
#include "SAM_TcpServerPrivate.h"

namespace SAM
{
    void TcpServerPrivate::TcpServerInner::incomingConnection(int handle)
    {
        TRACE(LOCAL);
        // Pasa el evento a la clase del servidor
        server->incomingConnection(handle);
    }

    TcpServer::TcpServer(QObject *parent, int i)
        : TcpUdpServer(parent, i)
    {
        TRACE(LOCAL);
        setRunnableFactoryInner(&createRunnableInstance<TcpServerThread>);
    }

    TcpServer::TcpServer(QObject *parent)
        : TcpUdpServer(parent, -1)
    {
        TRACE(LOCAL);

        m_ptr.reset(new TcpServerPrivate());

        setRunnableFactoryInner(&createRunnableInstance<TcpServerThread>);
    }

    void TcpServer::setRunnableFactoryInner(runnable_factory_ptr pfn_runnable_factory)
    {
        TRACE(LOCAL);

        Q_D(TcpServer);

        d->m_pfn_runnable_factory = pfn_runnable_factory;
    }

    void TcpServer::setRunnableFactoryInner(ClassGenParent<int> * pClassGen)
    {
        TRACE(LOCAL);

        Q_D(TcpServer);

        delete d->m_pGen;
        d->m_pGen = pClassGen;
    }

    bool TcpServer::isListening() const
    {
        TRACE(SERV);

        Q_D(const TcpServer);

        return d->m_socketServer->isListening();
    }

    void TcpServer::close()
    {
        TRACE(SERV);

        Q_D(TcpServer);

        d->m_socketServer->close();
    }

    void TcpServer::listen()
    {
        TRACE(SERV);

        Q_D(TcpServer);

        if(!d->m_socketServer)
        {
            d->m_socketServer = new TcpServerPrivate::TcpServerInner(this);

            d->m_socketServer->server = d;
        }

        QString name;
        if(objectName() == d->m_url.toString())
            name = "TCP";
        else
            name = objectName();

        QHostAddress address;
        if(!d->m_url.host().isEmpty()) // La dirección IPv4 "any" es equivalente a QHostAddress("0.0.0.0")
            address.setAddress(d->m_url.host());

        qint16 port = (qint16) d->m_url.port();

        bool bListen = d->m_socketServer->listen(address, port);

//        // Only QTcpSocket uses QAbstractSocket's internal buffer;
//        // QUdpSocket does not use any buffering at all, but rather relies on
//        // the implicit buffering provided by the operating system.
//        // Because of this, calling this function on QUdpSocket has no effect.
//        if(bListen && d->m_readBufferSize > 0)
//        {
//            // Obtener el socket que está a la escucha
//            QTcpSocket tcpSocket;

//            tcpSocket.setSocketDescriptor(d->m_socketServer->socketDescriptor());
//            tcpSocket.setReadBufferSize(d->m_readBufferSize);
//        }

        if (bListen)
        {
            LOG_INFO(COMS) << "El Servidor" << name << "está a la escucha en"
                           << url().toString();
        }
        else
        {
            LOG_ERROR(COMS) << "El Servidor" << name << "no puede iniciar la escucha en"
                            << url().toString() << d->m_socketServer->errorString();

            // Reintentar la escucha
            if(d->m_retryTimeLapse > 0)
            {
                if(!d->m_pTimerRetry)
                {
                    d->m_pTimerRetry = new QTimer(this);
                    connect(d->m_pTimerRetry, SIGNAL(timeout()), this, SLOT(listen()));
                    d->m_pTimerRetry->setSingleShot(true);
                }

                // Antes de procesar el buffer, intentar captar más datagramas pasados 30ms.
                d->m_pTimerRetry->start(d->m_retryTimeLapse);
            }
        }
    }

    QString TcpServer::errorString() const
    {
        TRACE(SERV);

        Q_D(const TcpServer);

        return d->m_socketServer->errorString();
    }

    void TcpServerPrivate::incomingConnection(int socketDescriptor)
    {
        TRACE(SERV);
        QRunnable *runnable = NULL;
        ++m_incomingConnectionsCount;

        if(m_pGen) {
            runnable = m_pGen->createRunnableInstance(socketDescriptor);
        } else {
            // Crea una instancia de una clase que procesará los datos del socket.
            runnable = m_pfn_runnable_factory(socketDescriptor);
        }

        // QThreadPool toma `runnable` en propiedad.
        // Lo borrará automáticamente cuando la tarea termine.
        QThreadPool::globalInstance()->start(runnable);
    }

}
