#ifndef SAM_UDPSERVERPRIVATE_H
#define SAM_UDPSERVERPRIVATE_H

#include <QByteArray>
#include <QTimer>
#include <QMap>
#include <QDateTime>

#include "SAM_TcpUdpServerPrivate.h"
#include "SAM_UdpServerThread.h"
#include "SAM_UdpServer.h"

class QUdpSocket;

namespace SAM
{
    class UdpServerPrivate : public TcpUdpServerPrivate
    {
        friend class UdpServer;

    protected:

        UdpServerPrivate()
            : m_socketServer(0),
              m_repeatedCount(0),
              m_repeatedMsecs(2000),
              m_pTimerIncommingConnection(NULL),
              m_pfn_runnable_factory(&UdpServer::createRunnableInstance<UdpServerThread>),
              m_pGen(0)
        {
            m_readBufferSize = 1<<18; // 256kB
        }

        ~UdpServerPrivate()
        {
            delete m_pGen;
        }

        QUdpSocket * m_socketServer;

        typedef QMap<unsigned long long, QDateTime> repeatedTable_type;
        repeatedTable_type m_repeatedHash; ///< Tabla para descartar paquetes repetidos
        quint64 m_repeatedCount; ///< Cuenta de paquetes que no se han procesado por que estaban repetidos
        int m_repeatedMsecs; ///< Milisegundos que se considerarán como máximo para comparar datagramas repetidos
        QTimer * m_pTimerIncommingConnection; ///< Temporizador de espera de conexiones de entrada
        QByteArray m_byteArray; ///< Datos binarios recibidos y no procesados.
        UdpServer::runnable_factory_ptr m_pfn_runnable_factory;
        TcpUdpServer::ClassGenParent<Packet> * m_pGen;

    };

}

#endif // SAM_UDPSERVERPRIVATE_H
