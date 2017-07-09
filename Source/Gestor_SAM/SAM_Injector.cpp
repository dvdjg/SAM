#include <QTimer>
#include <QDateTime>
#include <QUrl>
#include <QUdpSocket>

#include "SAM_Log.h"
#include "SAM_Declaration.h"
#include "SAM_Archive.h"
#include "SAM_InjectorPrivate.h"
#include "SAM_Injector.h"
#include "SAM_JsonUtil.h"

namespace SAM
{

    using namespace SAM_Declaration;

    InjectorPrivate::InjectorPrivate() :
        m_injectFile(),
        m_ioCompressor(&m_injectFile),
        m_pDevice(&m_injectFile),
        m_fileOffset(0),
        m_packetInjectedCount(0),
        m_bytesInjectedCount(0),
        m_pTimer(NULL)
    {
    }

    Injector::Injector(QObject *parent, int i)
        : CommandObject(parent, i)
    {
    }

    Injector::Injector(QObject *parent)
        : CommandObject(parent, -1)
    {
        m_ptr.reset(new InjectorPrivate());
    }

    void Injector::injectPacket()
    {
        TRACE(COMS);

        Q_D(Injector);

        if(d->m_packet["cabecera"].contains("origen"))
        {
            QUrl url(d->m_packet["cabecera"]["origen"]);
            QUdpSocket socket;

            if(socket.bind())
            {
                MSG_TH msg;
                memset(&msg, 0, sizeof(msg));

                JVariant msgth = d->m_packet["cuerpo"][0]["MSG_TH"];
                InJsonArchive inBinJson(msgth);

                serialize(inBinJson, msg);
                QHostAddress hostAddress(url.host());

                uint len = qMin(uint(sizeof(msg)), uint(msgth["cab"]["lng"]));

                LOG_DEBUG(COMS) << "Inyectando paquete de" << len << "bytes a" >> url.toString();

                d->m_bytesInjectedCount += socket.writeDatagram(
                            (const char*) &msg, len, hostAddress, url.port());
                ++d->m_packetInjectedCount;
            }
        }

        injectPacketInner();
    }

    void Injector::injectPacketInner()
    {
        TRACE(COMS);

        Q_D(Injector);

        do
        {
            d->m_packet = JsonUtil::parseJson(d->m_textStream);

            if(d->m_packet && d->m_packet["cabecera"].contains("fecha"))
            {
                qint64 msecs = 0;
                QDateTime fecha = d->m_packet["cabecera"]["fecha"].toDateTime();

                if(d->m_fileOffset) {
                    // Calcula los milisegundos hasta enviar éste mensaje
                    msecs = QDateTime::currentDateTime().msecsTo(fecha.addMSecs(d->m_fileOffset));
                } else {
                    // Toma el offset del primer paquete
                    d->m_fileOffset = fecha.msecsTo(QDateTime::currentDateTime());
                }

                if(!d->m_pTimer)
                {
                    d->m_pTimer = new QTimer(this);
                    connect(d->m_pTimer, SIGNAL(timeout()), this, SLOT(injectPacket()));
                    d->m_pTimer->setSingleShot(true);
                }

                // Programa el envío del siguiente mensaje
                d->m_pTimer->start(msecs);

                return;
            }
        } while(d->m_packet);

        d->m_pDevice->close();

        emit fileInjected();
    }

    void Injector::setFileName(QString fileName)
    {
        Q_D(Injector);

        d->m_injectFile.setFileName(fileName);
    }

    uint Injector::packetInjectedCount() const
    {
        Q_D(const Injector);

        return d->m_packetInjectedCount;
    }

    /**
     * @brief Número de bytes inyectados
     *
     * @return qint64
     */
    qint64 Injector::bytesInjectedCount() const
    {
        Q_D(const Injector);

        return d->m_bytesInjectedCount;
    }

    void Injector::injectFile()
    {
        TRACE(COMS);

        Q_D(Injector);


        if(d->m_pDevice->isOpen()) {
            d->m_pDevice->close();
        }

        QString fileName = d->m_injectFile.fileName();

        if(fileName.endsWith(".gz")) {
            d->m_ioCompressor.setStreamFormat(QtIOCompressor::GzipFormat);
            d->m_pDevice = &d->m_ioCompressor;
        } else {
            d->m_pDevice = &d->m_injectFile;
        }

        if(!d->m_pDevice->isOpen())
        {
            if(d->m_pDevice->open(QIODevice::ReadOnly))
            {
                d->m_textStream.setDevice(d->m_pDevice);

                JVariant header = JsonUtil::parseJson(d->m_textStream);

                if(header.contains("sniffer")) {
                    LOG_INFO(COMS) << "Inyectando el archivo" >> d->m_injectFile.fileName();

                    injectPacketInner();
                }
            }
        }
        //const QByteArray text = d->m_ioCompressor.readAll();
        //d->m_pDevice->close();
    }
}
