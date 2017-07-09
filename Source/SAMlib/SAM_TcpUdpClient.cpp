#include <QDataStream>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTimer>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QUrlQuery>
#endif

#include "keepSocketAlive.h"
#include "SAM_Log.h"

#include "SAM_TcpUdpClientPrivate.h"
#include "SAM_TcpUdpClient.h"

namespace SAM
{
    TcpUdpClientPrivate::TcpUdpClientPrivate(const QUrl &serverUrl)
        : m_url(serverUrl),
          m_maxPacketCount(100),
          m_bPerPacketReopen(true),
          m_packetsTaken(0),
          m_bytesTaken(0),
          m_packetsWritten(0),
          m_bytesWritten(0),
          m_bytesWrittenAck(0),
          m_maxIdle(0),
          m_interval(0),
          m_pTimer(NULL)
    {
        TRACE(CLIENT);
    }

    TcpUdpClient::TcpUdpClient(const QUrl & serverUrl, QObject *parent)
        : CommandObject(parent)
    {
        TRACE(CLIENT);

        m_ptr.reset(new TcpUdpClientPrivate(serverUrl));

        Q_D(TcpUdpClient);

        setObjectName(serverUrl.toString());

        QString scheme = d->m_url.scheme().toLower();

        if(scheme == "udp") {
            d->m_inetSocket = new QUdpSocket(this);
        } else {
            if(scheme != "tcp") {
                LOG_WARN(COMS) <<  "No se ha encontrado el protocolo para el cliente socket "
                    << d->m_url.toString() << ". Usando tipo TCP";
            }

            d->m_inetSocket = new QTcpSocket(this);
        }

        connect(d->m_inetSocket, SIGNAL(readyRead()),
                this, SLOT(readyRead()));
        connect(d->m_inetSocket, SIGNAL(connected()),
                this, SLOT(connected()));
        connect(d->m_inetSocket, SIGNAL(disconnected()),
                this, SLOT(disconnected()));
        connect(d->m_inetSocket, SIGNAL(bytesWritten(qint64)),
                this, SLOT(bytesWritten(qint64)));
        connect(d->m_inetSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(socketError(QAbstractSocket::SocketError)));
    }

    TcpUdpClient::~TcpUdpClient()
    {
        TRACE(CLIENT);
    }

    void TcpUdpClient::checkBufferSize()
    {
        TRACE(CLIENT);

        Q_D(TcpUdpClient);

        while(d->m_packetQueue.size() >= d->m_maxPacketCount)
        {
            // Borra el elemento más moderno
            d->m_packetQueue.removeLast();

            emit error("Desbordamiento del buffer de mensajes del servidor " + d->m_url.toString());
        }
    }


    void TcpUdpClient::quit()
    {
        TRACE(CLIENT);

        // Intentar desconectar ordenadamente escribiendo lo que quede
        disconnectFromHost(100);

        CommandObject::quit();
    }

    void TcpUdpClient::processCommand(QStringList command)
    {
        TRACE(CLIENT);

        if(command.count() > 0)
        {
            QString comm(command[0].toLower());

            if(command.count() > 1 && comm == "maxbuffercount") // Mensaje de terminación
            {
                int maxElems = command[1].toInt();
                setMaxBufferPacketCount(maxElems);
            }
            else
            {
                CommandObject::processCommand(command);
            }
        }
    }

    void TcpUdpClient::setMaxBufferPacketCount(int maxElems)
    {
        TRACE(CLIENT);

        Q_D(TcpUdpClient);

        if(maxElems > 0)
        {
            d->m_maxPacketCount = maxElems;

            LOG_INFO(COMS) << "Establecido el tamaño del buffer del cliente" << objectName()
                       << "a" << maxElems;
        }

        checkBufferSize();
    }

    void TcpUdpClient::sendPacket(Packet packet)
    {
        TRACE(CLIENT);

        Q_D(TcpUdpClient);

        d->m_packetQueue.append(packet);
        ++d->m_packetsTaken;
        d->m_bytesTaken += packet.byteArray().size();

        checkBufferSize();

        emit sendPacketQueue();
    }

    void TcpUdpClient::sendPacketQueue()
    {
        TRACE(CLIENT);

        Q_D(TcpUdpClient);

        if(d->m_packetQueue.isEmpty())
            return; // Si no hay datos que enviar, regresa

        QAbstractSocket::SocketState state = d->m_inetSocket->state();
        if(!d->m_bPerPacketReopen
                && state == QAbstractSocket::ConnectedState
                && d->m_inetSocket->isWritable())
        {
            // Si el socket está conectado y escribible, no reabrirlo.
            flushPendingPackets();
        }
        else
        {
            connectToHost();
        }
    }

    void TcpUdpClient::connectToHost()
    {
        TRACE(CLIENT);

        Q_D(TcpUdpClient);

        QAbstractSocket::SocketState state = d->m_inetSocket->state();

        // No interrumpir una operación previa de conexión. Esperar al error().
        if(state == QAbstractSocket::UnconnectedState)
        {
            d->m_inetSocket->abort();

            QString host = d->m_url.host();
            quint16 port = (quint16) d->m_url.port();

            //d->m_inetSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
            bool ok;
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
            int ttl = d->m_url.queryItemValue("ttl").toInt(&ok);
#else
            int ttl = QUrlQuery(d->m_url).queryItemValue("ttl").toInt(&ok);
#endif
            if(ttl)
            {
#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
                if(d->m_url.scheme().toLower() != "udp"){
                    LOG_WARN(COMS) << "Se está estableciendo una opción Multicast TTL al socket "
                                   << d->m_url.toString() << "que no es UDP.";
                }

                d->m_inetSocket->setSocketOption(QAbstractSocket::MulticastTtlOption, ttl);
#else
                LOG_WARN(COMS) << "Se está intentando una conexión Multicast, pero la versión de Qt v"
                                  QT_VERSION_STR " con la que se ha compilado no lo permite.";
#endif
            }
            // Reabrir el socket y esperar la conexión
            d->m_inetSocket->connectToHost(host, port); // Provoca la llamada a connected()
        }
    }

    void TcpUdpClient::connected()
    {
        TRACE(CLIENT);
        Q_D(TcpUdpClient);

        if(d->m_inetSocket->socketType() == QAbstractSocket::TcpSocket)
        {
            // Para sockets orientados a la conexión, configurar si se quiere que
            // se mantenga la conexión viva.
            int fd = d->m_inetSocket->socketDescriptor();
            if(d->m_maxIdle > 0 && d->m_interval > 0) {
                keepSocketAlive(fd, 1, d->m_maxIdle, d->m_interval);
            } else {
                keepSocketAlive(fd, 0);
            }
        }

        flushPendingPackets();
    }

    void TcpUdpClient::setSocketAliveMaxIdle(int maxIdle)
    {
        TRACE(CLIENT);

        Q_D(TcpUdpClient);

        d->m_maxIdle = maxIdle;
    }

    void TcpUdpClient::setSocketAliveInterval(int interval)
    {
        TRACE(CLIENT);

        Q_D(TcpUdpClient);

        d->m_interval = interval;
    }

    void TcpUdpClient::flushPendingPackets()
    {
        TRACE(CLIENT);

        Q_D(TcpUdpClient);

        // Envía paquetes del buffer
        while(!d->m_packetQueue.isEmpty())
        {
            Packet packet = d->m_packetQueue.first();

            // Escribe a la interfaz. Puede que no todo lo escrito llegue al destinatario.
            qint64 nWritten = d->m_inetSocket->write(packet.byteArray());

            LOG_INFO(COMS) << "Escritos" << nWritten << "bytes a" << url().toString();

            if(nWritten == packet.byteArray().length())
            {
                d->m_bytesWritten += nWritten; // Bytes escritos a la interfaz

                if(d->m_bPerPacketReopen)
                {
                    // No enviar más paquetes hasta que nos hayamos asegurado de que
                    // el destinatario lo ha recibido y se haya reabierto el socket.
                    break;
                }
                else
                {
                    // Asume que el paquete se ha enviado
                    d->m_packetQueue.removeFirst();
                    ++d->m_packetsWritten;
                }
            }
            else
            {
                LOG_WARN(COMS) << "No se ha podido escribir el paquete a"
                                << d->m_url.toString();
                break;
            }
        }
    }

    void TcpUdpClient::bytesWritten ( qint64 nBytes )
    {
        TRACE(CLIENT);

        Q_D(TcpUdpClient);

        // Número de bytes escritos confirmados
        d->m_bytesWrittenAck += nBytes;

        if(d->m_bPerPacketReopen) {
            // Esperar a la señal de desconexión
            d->m_inetSocket->disconnectFromHost();
        }
    }

    void TcpUdpClient::disconnected()
    {
        TRACE(CLIENT);

        Q_D(TcpUdpClient);

        if(d->m_bPerPacketReopen)
        {
            if(!d->m_packetQueue.empty())
            {
                // Contabiliza un nuevo paquete enviado
                d->m_packetQueue.removeFirst();
                ++d->m_packetsWritten;
            }
        }

        // Reintentar enviar otro paquete
        QMetaObject::invokeMethod( this, "sendPacketQueue",
                                   Qt::QueuedConnection );
    }

    void TcpUdpClient::readyRead()
    {
        TRACE(CLIENT);

        Q_D(TcpUdpClient);

        QByteArray all = d->m_inetSocket->read(1<<20); // por seguridad, lee como mucho 1MB
    }

    bool TcpUdpClient::disconnectFromHost(int msecs)
    {
        TRACE(CLIENT);

        Q_D(TcpUdpClient);

        if(d->m_inetSocket->state() == QAbstractSocket::UnconnectedState)
            return true;

        /// Esperar a que se terminen de escribir los datos (si los hubiera)
        d->m_inetSocket->waitForBytesWritten(msecs);

        /// Liberar el socket
        d->m_inetSocket->disconnectFromHost();

        if(d->m_inetSocket->state() != QAbstractSocket::UnconnectedState
            && !d->m_inetSocket->waitForDisconnected(msecs)) // Espera un poco para desconectar
        {
            d->m_inetSocket->abort();

            LOG_WARN(COMS) <<  "Timeout mientras se intentaba cerrar el socket conectado a "
                << url().toString() << ". Posible pérdida de datos.";

            return false;
        }

        return true;
    }

    void TcpUdpClient::socketError(QAbstractSocket::SocketError socketError)
    {
        TRACE(CLIENT);

        Q_D(TcpUdpClient);

        QString err = '[' + d->m_url.toString() + "] ";

        switch (socketError)
        {
        case QAbstractSocket::RemoteHostClosedError:
            err += "RemoteHostClosedError.- ";
            break;
        case QAbstractSocket::HostNotFoundError:
            //LOG_ERROR(COMS) <<  "No se encuentra el anfitrión. Compruebe el nombre y el puerto del servidor: " << d->m_inetSocket->errorString();
            err += "HostNotFoundError.- No se encuentra el anfitrión. Compruebe el nombre y el puerto del servidor: ";
            break;
        case QAbstractSocket::ConnectionRefusedError:
            //LOG_ERROR(COMS) <<  "La conexión fue rechazada por el par. Compruebe que el servidor se esté ejecutando y que el nombre y el puerto del servidor sean correctos: " << d->m_inetSocket->errorString();
            err += "ConnectionRefusedError.- La conexión fue rechazada por el par. Compruebe que el servidor se esté ejecutando y que el nombre y el puerto del servidor sean correctos: ";
            break;
        default:
            //LOG_ERROR(COMS) <<  "Ha ocurrido el siguiente error: " << d->m_inetSocket->errorString();
            err += "Ha ocurrido el siguiente error: ";
        }

        err += d->m_inetSocket->errorString();

        // Reiniciar el socket
        // La función close() es más drástica que abort()
        // se usa aquí por si acaso hubiera habido un error importante en la
        // interfaz de la propia máquina
        d->m_inetSocket->close();

        emit error(err);

        if(!d->m_pTimer)
        {
            d->m_pTimer = new QTimer(this);
            connect(d->m_pTimer, SIGNAL(timeout()), this, SLOT(sendPacketQueue()));
            d->m_pTimer->setSingleShot(true);
        }

        // Reintentar enviar paquetes pasado un segundo
        d->m_pTimer->start(1000);
    }


    bool TcpUdpClient::perPacketReopen() const
    {
        Q_D(const TcpUdpClient);

        return d->m_bPerPacketReopen;
    }

    void TcpUdpClient::setPerPacketReopen(bool perPacketReopen)
    {
        Q_D(TcpUdpClient);

        d->m_bPerPacketReopen = perPacketReopen;
    }

    const QUrl &TcpUdpClient::url() const
    {
        Q_D(const TcpUdpClient);

        return d->m_url;
    }

    bool TcpUdpClient::operator ==(const QUrl & url) const
    {
        Q_D(const TcpUdpClient);

        return d->m_url == url;
    }

    bool TcpUdpClient::operator ==(const TcpUdpClient & other) const
    {
        Q_D(const TcpUdpClient);


        return d->m_url == other.url();
    }

    int TcpUdpClient::maxBufferPacketCount() const
    {
        Q_D(const TcpUdpClient);


        return d->m_maxPacketCount;
    }

    int TcpUdpClient::bufferPacketCount() const
    {
        Q_D(const TcpUdpClient);

        return d->m_packetQueue.size();
    }

    qint64 TcpUdpClient::packetsTaken() const
    {
        Q_D(const TcpUdpClient);

        return d->m_packetsTaken;
    }

    qint64 TcpUdpClient::bytesTaken() const
    {
        Q_D(const TcpUdpClient);

        return d->m_bytesTaken;
    }

    qint64 TcpUdpClient::packetsWritten() const
    {
        Q_D(const TcpUdpClient);

        return d->m_packetsWritten;
    }

    qint64 TcpUdpClient::bytesWritten() const
    {
        Q_D(const TcpUdpClient);

        return d->m_bytesWritten;
    }

    qint64 TcpUdpClient::bytesWrittenAck() const
    {
        Q_D(const TcpUdpClient);

        return d->m_bytesWrittenAck;
    }
}
