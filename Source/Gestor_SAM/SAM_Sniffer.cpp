#include <QFile>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QNetworkInterface>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QUrlQuery>
#endif

#include "qtiocompressor.h"
#include "SAM_Log.h"
#include "SAM_Packet.h"
#include "SAM_Declaration.h"
#include "SAM_Archive.h"
#include "SAM_JsonUtil.h"
#include "SAM_Sniffer.h"
#include "SAM_Helpers.h"
#include "SAM_MessageProcessor.h"
#include "SAM_CommandObjectPrivate.h"

namespace SAM
{
    class SnifferPrivate : public CommandObjectPrivate
    {
        friend class Sniffer;

    protected:
        SnifferPrivate();

        QIODevice * m_snifferDevice;
        QFile m_snifferFile;
        QtIOCompressor m_snifferFileCompressor;
    };

    SnifferPrivate::SnifferPrivate() :
        m_snifferDevice(NULL),
        m_snifferFile(),
        m_snifferFileCompressor(&m_snifferFile)
    {
    }

    Sniffer::Sniffer(QObject *parent, int i)
        : CommandObject(parent, i)
    {
    }

    Sniffer::Sniffer(QObject *parent)
        : CommandObject(parent, -1)
    {
        m_ptr.reset(new SnifferPrivate());
    }

    QString Sniffer::snifferFileName() const
    {
        J_D(const Sniffer);

        QString fileName = d->m_snifferFile.fileName();

        return fileName;
    }

    bool Sniffer::isSnifferFileOpen() const
    {
        J_D(const Sniffer);

        return d->m_snifferDevice && d->m_snifferDevice->isOpen();
    }

    bool Sniffer::processMessage(Packet msgPacket)
    {
        sniffMessage(msgPacket); // --sniffer=udp://:7003

        return true;
    }

    void Sniffer::sniffMessage(const Packet msgPacket)
    {
        TRACE(LOCAL);

        J_D(Sniffer);

        if(d->m_snifferFile.fileName().isEmpty())
        {
            // Establece el nombre del archivo
            QUrl url(msgPacket.origin());

            // Establece el nombre del archivo tal como esté indicado en el
            // parámetro `path` de la url.
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
            QString strPath = url.queryItemValue("path");
#else
            QString strPath = QUrlQuery(url).queryItemValue("path");
#endif
            if(strPath.isEmpty()) {
                d->m_snifferFile.setFileName("sniffer/messages.sniff.json.gz");
            } else {
                d->m_snifferFile.setFileName(strPath);
            }
        }

        QString fileName = d->m_snifferFile.fileName();

        if(!d->m_snifferDevice)
        {
            QString path = QFileInfo(fileName).path();
            if(!QDir().mkpath(path)) {
                LOG_INFO(COMS) << "Creada la carpeta" >> path << "para "
                                  "almacenar los paquetes esnifados.";
            }

            if(fileName.endsWith(".gz")) {
                d->m_snifferFileCompressor.setStreamFormat(QtIOCompressor::GzipFormat);
                d->m_snifferDevice = &d->m_snifferFileCompressor;
            } else {
                d->m_snifferDevice = &d->m_snifferFile;
            }
        }

        if(!d->m_snifferDevice->isOpen())
        {
            // Si existe el archivo lo almacena con una marca de tiempo
            store(fileName);

            if(d->m_snifferDevice->open(QIODevice::WriteOnly))
            {
                // Crear una cabecera para describir el tipo de archivo
                JVariant inicio;
                JVariant & cabecera = inicio["sniffer"];

                cabecera["fecha"] = QDateTime::currentDateTime();
                cabecera["organización"] = QCoreApplication::organizationName();
                cabecera["dominio"] = QCoreApplication::organizationDomain();
                cabecera["nombreAplicación"] = QCoreApplication::applicationName();
                cabecera["versiónAplicación"] = QCoreApplication::applicationVersion();
                cabecera["arquitectura"] = SAM_HOST_ARCH;
                cabecera["versiónOS"] = SAM_HOST_VERSION;
                cabecera["OS"] = SAM_HOST_OS;
                cabecera["nombreEquipo"] = SAM_HOST_NAME;
                cabecera["versiónCompilador"] = SAM_COMPILER_VERSION;
                cabecera["localIPs"] = QNetworkInterface::allAddresses();

                if(false)
                {
                    QListIterator<QNetworkInterface> it(QNetworkInterface::allInterfaces());
                    while(it.hasNext())
                    {
                        const QNetworkInterface & ni = it.next();
                        JVariant inter;
                        inter["índice"] = ni.index();
                        inter["nombre"] = ni.humanReadableName();
                        inter["mac"] = ni.hardwareAddress();
                        cabecera["interfaces"].asList().append(inter);
                    }
                }

                d->m_snifferDevice->write(inicio.toJson().toUtf8());
                d->m_snifferDevice->putChar('\n');
            }
            else
            {
                LOG_WARN(LOCAL) << "No se puede crear el archivo" >> fileName
                                << "por:" >> d->m_snifferDevice->errorString();
            }
        }

        if(d->m_snifferDevice->isOpen())
        {
            JVariant MSG_TH = msgPacket.userData();
            JVariant jStore;
            jStore["cabecera"]["fecha"] = QDateTime::currentDateTime();
            jStore["cabecera"]["origen"] = msgPacket.origin();
            jStore["cuerpo"][0]["MSG_TH"] = MSG_TH;

            d->m_snifferDevice->write(jStore.toJson().toUtf8());
            d->m_snifferDevice->putChar('\n');

            long long size = d->m_snifferFile.size();

            if(size > (1<<20))
            {
                d->m_snifferDevice->close();

                store(fileName);
            }

            char chInfo = '.';

            QString idMsg = MSG_TH["cab"]["idMsg"];
            if(!idMsg.isEmpty())
            {
                if(idMsg == "ESTF") {
                    chInfo = 'T';
                } else if(idMsg == "ESRD") {
                    chInfo = 'R';
                } else {
                    chInfo = idMsg.at(0).toLatin1();
                }
            }

            IF_LOG(LOCAL, SAM::Logger::DebugLevel) cerr << chInfo << flush;
        }
    }


    int Sniffer::parseBinaryPacket(Packet packet)
    {
        // Se reutiliza la función estática MessageProcessor::parseBinaryPacket
        return MessageProcessor::parseBinaryPacket(this, packet);
    }

}

