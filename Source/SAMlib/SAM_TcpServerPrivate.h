#ifndef SAM_TCPSERVERPRIVATE_H
#define SAM_TCPSERVERPRIVATE_H

#include <QTcpServer>

#include "SAM_TcpServer.h"
#include "SAM_TcpUdpServerPrivate.h"


namespace SAM
{
    class TcpServer;

    class TcpServerPrivate : public TcpUdpServerPrivate
    {
        friend class TcpServer;

    protected:
        TcpServerPrivate()
            : m_socketServer(NULL), m_pfn_runnable_factory(NULL), m_pGen(0) {}

        ~TcpServerPrivate() {
            delete m_pGen;
        }

        /**
         * @brief Clase auxiliar para capturar el evento `QTcpServer::incomingConnection`
         *  ya que es necesario para implementar un sistema de hilos con QTcpServer.
         * @note Sería deseable que el método virtual `QTcpServer::incomingConnection`
         *  hubiera sido implementado mediante un `signal` de Qt.
         */
        class TcpServerInner : public QTcpServer
        {
        public:
            //TcpServerInner() {}
            explicit TcpServerInner(QObject *parent)
                : QTcpServer(parent), server(NULL) {}

            virtual void incomingConnection(int handle);

            TcpServerPrivate * server;
        };

    protected:
        void incomingConnection(int socketDescriptor);

//        /** Establece la factoría de clases que se creará cada vez que llegue un nuevo paquete.
//          El constructor debe aceptar un descriptor de socket de tipo entero.
//          Ej: Para elegir la clase TcpServerThread se llamaría a:
//                setRunnableInstance(&createRunnableInstance<TcpServerThread>;
//          @param pfn_runnable_factory Variable de tipo runnable_factory_ptr que devuelve
//            una instancia de un QRunnable.
//          @note Puede usar la función TcpServer::setRunnableFactory<>() en lugar de esta.
//          */
//        void setRunnableFactory(runnable_factory_ptr pfn_runnable_factory)
//        {
//            m_pfn_runnable_factory = pfn_runnable_factory;
//        }

        TcpServerInner * m_socketServer;

        TcpServer::runnable_factory_ptr m_pfn_runnable_factory;
        TcpUdpServer::ClassGenParent<int> * m_pGen;
    };

}

#endif // SAM_TCPSERVERPRIVATE_H
