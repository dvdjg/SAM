#ifndef SAM_TCPUDPSERVERPRIVATE_H
#define SAM_TCPUDPSERVERPRIVATE_H

#include <QUrl>
#include <QTimer>

#include "SAM_CommandObjectPrivate.h"

namespace SAM
{

    class TcpUdpServerPrivate : public CommandObjectPrivate
    {
        friend class TcpUdpServer;

    protected:
        TcpUdpServerPrivate();

        QUrl m_url; ///< URL con la que se configuró el servidor.
        QByteArray m_incomingDatagram; ///< Almacenamiento de los datos leídos del socket
        int m_readBufferSize; ///< Tamaño máximo del buffer de la pila de recepción de datos TCP/UDP.
        int m_incomingConnectionsCount; ///< Número de conexiones que se han atendido.
        int m_retryTimeLapse; ///< Tiempo en milisegundos entre reintentos de escucha. A 0 para no seguir intentándolos.
        QTimer* m_pTimerRetry;
    };
}

#endif // SAM_TCPUDPSERVERPRIVATE_H
