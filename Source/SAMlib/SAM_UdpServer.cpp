#include <QRunnable>
#include <QTimer>
#include <QThreadPool>
#include <QUdpSocket>
#include <QCryptographicHash>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#   include <QUrlQuery>
#endif

#include "MurmurHash.h"

#include "SAM_Log.h"
#include "SAM_UdpServerThread.h"
#include "SAM_UdpServer.h"
#include "SAM_UdpServerPrivate.h"

#ifdef _WIN32
// Por ::setsockopt()
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

namespace SAM
{
    UdpServer::UdpServer(QObject *parent, int i)
        : TcpUdpServer(parent, i)
    {
        TRACE(SERV);
    }

    UdpServer::UdpServer(QObject *parent)
        : TcpUdpServer(parent, -1)
    {
        TRACE(SERV);
        m_ptr.reset(new UdpServerPrivate);
    }

    UdpServer::~UdpServer()
    {
        TRACE(SERV);
    }

    QString UdpServer::errorString() const
    {
        TRACE(SERV);

        Q_D(const UdpServer);

        return d->m_socketServer->errorString();
    }

    void UdpServer::setRunnableFactoryInner(runnable_factory_ptr pfn_runnable_factory)
    {
        Q_D(UdpServer);

        d->m_pfn_runnable_factory = pfn_runnable_factory;
    }

    void UdpServer::setRunnableFactoryInner(ClassGenParent<Packet> * pClassGen)
    {
        TRACE(LOCAL);

        Q_D(UdpServer);

        delete d->m_pGen;
        d->m_pGen = pClassGen;
    }

    bool UdpServer::isListening() const
    {
        TRACE(SERV);

        Q_D(const UdpServer);

        return d->m_socketServer->state() == QAbstractSocket::BoundState;
    }

    int UdpServer::unprocessedBytes() const
    {
        TRACE(SERV);

        Q_D(const UdpServer);

        return d->m_byteArray.size();
    }
    void UdpServer::close()
    {
        TRACE(SERV);

        Q_D(UdpServer);

        d->m_socketServer->close();
    }

    void UdpServer::listen()
    {
        TRACE(SERV);

        Q_D(UdpServer);

        if(!d->m_socketServer)
        {
            d->m_socketServer = new QUdpSocket(this);
            connect(d->m_socketServer, SIGNAL(readyRead()), this, SLOT(incomingConnection()),
                    Qt::UniqueConnection);
        }

        QHostAddress address;
        if(!d->m_url.host().isEmpty()) // La dirección IPv4 "any" es equivalente a QHostAddress("0.0.0.0")
            address.setAddress(d->m_url.host());

        qint16 port = (qint16) d->m_url.port();

        QString name;
        if(objectName() == d->m_url.toString())
            name = "UDP";
        else
            name = objectName();

        // Permitir que más de un socket esté a la escucha en el mismo puerto (útil para sniffers)
        if(d->m_socketServer->bind(address, port, QUdpSocket::ShareAddress
                                | QUdpSocket::ReuseAddressHint))
        {
            // Si se requiere una conexión en multicast, añadirse al grupo
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
            QString strMulticast = d->m_url.queryItemValue("multicast");
#else
            QUrlQuery urlParser(d->m_url);
            QString strMulticast = urlParser.queryItemValue("multicast");
#endif
            if(!strMulticast.isEmpty())
            {
#if QT_VERSION < QT_VERSION_CHECK(4, 8, 0)
                LOG_ERROR(COMS) << "Se ha compilado la aplicación con una versión Qt"QT_VERSION_STR" pero para "
                                   "habilitar el soporte de multicast es necesario usar versiones superiores a 4.8.0";
#else
                QHostAddress multicastGroup(strMulticast);

                if(d->m_socketServer->joinMulticastGroup(multicastGroup)) {
                    LOG_INFO(COMS) << "El servidor" << name << "que está escuchando con"
                                   << d->m_url.toString() << "se ha añadido al grupo multicast.";
                } else {
                    LOG_WARN(COMS) << "El servidor" << name << "que está escuchando con"
                                   << d->m_url.toString() << "no se ha añadido al grupo multicast. "
                                   << d->m_socketServer->errorString();
                }
#endif
            }

            if(d->m_readBufferSize > 0)
            {
                // Establecer el tamaño del buffer de la pila de recepción
                // UDP del Sistema Operativo para permitir capturar ráfagas
                // de tramas sin pérdida.
                if (::setsockopt(d->m_socketServer->socketDescriptor(),
                                 SOL_SOCKET,
                                 SO_RCVBUF,
                                 (char *) &d->m_readBufferSize,
                                 sizeof(d->m_readBufferSize)
                                 ) == -1)
                {
                    LOG_WARN(COMS) << "No se puede establecer el tamaño del buffer de "
                                      "la pila de recepción UDP del Sistema Operativo "
                                      "en" << d->m_url.toString();
                }
            }

            LOG_INFO(COMS) << "Servidor" << name << "escuchando en" << d->m_url.toString();
        }
        else
        {
            LOG_ERROR(COMS) << "El servidor" << name << "no puede escuchar en"
                            << d->m_url.toString() << ":" << d->m_socketServer->errorString();

            // Reintentar la escucha pasado un tiempo prudencial
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

    quint64 UdpServer::rejectedRepeatedDatagrams() const
    {
        Q_D(const UdpServer);
        return d->m_repeatedCount;
    }

    int UdpServer::rejectRepeatedInterval() const
    {
        Q_D(const UdpServer);
        return d->m_repeatedMsecs;
    }

    void UdpServer::setRepeatedInterval(int msec)
    {
        Q_D(UdpServer);
        d->m_repeatedMsecs = msec;
    }

    void UdpServer::incomingConnection()
    {
        TRACE(SERV);

        Q_D(UdpServer);

        ++d->m_incomingConnectionsCount;

        if(d->m_socketServer->hasPendingDatagrams())
        {
            while(d->m_socketServer->hasPendingDatagrams())
            {
                qint64 pendingDatagramSize = d->m_socketServer->pendingDatagramSize();
                if(pendingDatagramSize > (1<<16)) {
                    LOG_WARN_LN(COMS) << "Se ha recibido un datagrama UDP de más de 64k en"
                                       << objectName() << ":" << pendingDatagramSize;
                }

                // Limitar los datagramas de entrada a 1MB
                if(pendingDatagramSize > (1<<20)) {
                    pendingDatagramSize = (1<<20);
                }

                d->m_incomingDatagram.resize(pendingDatagramSize);
                d->m_socketServer->readDatagram(d->m_incomingDatagram.data(), d->m_incomingDatagram.size());

                if(d->m_repeatedMsecs > 0)
                {
                    // Determinar si éste mismo paquete se había recibido en un intervalo de menos de d->m_repeatedMsecs ms.
                    qint64 msecs = 0x7FFFFFFFFFFFFFFF;
                    //QByteArray hash = QCryptographicHash::hash((d->m_incomingDatagram),QCryptographicHash::Md5);
                    // uint hash = qHash(d->m_incomingDatagram);
                    unsigned long long hash = MurmurHash64(
                                d->m_incomingDatagram.constData(),
                                d->m_incomingDatagram.size());
                    QDateTime now(QDateTime::currentDateTime());
                    if(d->m_repeatedHash.contains(hash))
                    {
                        QDateTime date = d->m_repeatedHash[hash];

                        msecs = date.msecsTo(now);
                    }

                    if(msecs > d->m_repeatedMsecs)
                    {
                        d->m_repeatedHash[hash] = now;

                        // Evitar que la tabla de mensajes repetidos crezca demasiado
                        int repeatedHashSize = d->m_repeatedHash.size();
                        if(repeatedHashSize > 100)
                        {
                            for(UdpServerPrivate::repeatedTable_type::Iterator i(d->m_repeatedHash.begin());
                                i != d->m_repeatedHash.end(); )
                            {
                                qint64 msecs = i.value().msecsTo(now);
                                if(msecs > d->m_repeatedMsecs) {
                                    // Borra todos los mensajes con más de d->m_repeatedMsecs ms de antigüedad
                                    i = d->m_repeatedHash.erase(i);
                                } else {
                                    ++i;
                                }
                            }
                        }
                    }
                    else
                    {
                        // Se ha encontrado un mensaje repetido en los últimos 2 segundos,
                        // por lo que no se procesará.
                        ++d->m_repeatedCount;
                        continue;
                    }
                }

                d->m_byteArray.append(d->m_incomingDatagram);

                // Limitar el crecimiento del buffer temporal
                int byteArraySize = d->m_byteArray.size();
                if(byteArraySize > (1<<16))
                {
                    processDatagram();
                    break;
                }
            }

            if(!d->m_pTimerIncommingConnection)
            {
                d->m_pTimerIncommingConnection = new QTimer(this);
                connect(d->m_pTimerIncommingConnection, SIGNAL(timeout()), this, SLOT(incomingConnection()));
                d->m_pTimerIncommingConnection->setSingleShot(true);
            }

            // Antes de procesar el buffer, intentar captar más datagramas pasados 30ms.
            d->m_pTimerIncommingConnection->start(30);
        }
        else
        {
            // Sólo procesa el buffer cuando no hayan más datagramas pendientes.
            processDatagram();
        }
    }

    void UdpServer::processDatagram()
    {
        TRACE(SERV);

        Q_D(UdpServer);

        QRunnable *runnable = 0;

        if(d->m_byteArray.size() > 0)
        {
            Packet packet(d->m_byteArray, objectName());
            d->m_byteArray.clear();
            int capacity = d->m_byteArray.capacity();
            if(capacity > (1<<20)) {
                // No permitir que se aloje más de 1MB
                d->m_byteArray.squeeze();
            }

            if(d->m_pGen) {
                runnable = d->m_pGen->createRunnableInstance(packet);
            } else {
                // Crea una instancia de una clase que procesará el datagrama
                runnable = d->m_pfn_runnable_factory(packet);
            }

            // QThreadPool toma al `runnable` en propiedad y lo borra automáticamente cuando la tarea termine.
            QThreadPool::globalInstance()->start(runnable);
        }
    }

}
