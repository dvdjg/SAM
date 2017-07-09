#ifndef SAM_TCPSERVER_H
#define SAM_TCPSERVER_H

#include "SAM_TcpUdpServer.h"

class QRunnable;
class QUrl;

namespace SAM
{

    class TcpServerPrivate;

     /** @brief Servidor a la escucha de mensajes TCP
      */
    class TcpServer : public TcpUdpServer
    {
        Q_OBJECT

        J_DECLARE_PRIVATE(TcpServer)
        Q_DISABLE_COPY(TcpServer)

    protected:

        /** Función auxiliar para crear factorías de QRunnables.
          */
        template<typename T>
        static QRunnable * createRunnableInstance(int socketDescriptor)
        {
            return new T(socketDescriptor);
        }

        typedef QRunnable *(*runnable_factory_ptr)(int);

        TcpServer(QObject *parent, int i);

    public:

        /** Inicia un servidor para recibir mensajes TCP
          */
        TcpServer(QObject *parent = 0);

        /**
         * @brief Cadena de error del socket.
         *
         * @return QString
         */
        virtual QString errorString() const;

        /**
         *  @brief Establece la factoría de clases que se creará cada vez que
         *  llegue un nuevo paquete.
         *  El constructor debe aceptar un descriptor de socket de tipo entero.
         *  Ej: Para elegir la clase TcpServerThread se llamaría a:
         *       setRunnableInstance<TcpServerThread>();
         */
        template<typename R>
        void setRunnableFactory()
        {
            setRunnableFactoryInner(&TcpServer::createRunnableInstance<R>);
        }

        template<typename R, typename T>
        void setRunnableFactory(T t)
        {
            setRunnableFactoryInner(new ClassGen<int, R, T>(t));
        }

        /**
         * @brief Determina si el servidor está a la escucha.
         *
         * @return bool
         */
        virtual bool isListening() const;

    public slots:

        /**
         * @brief Escucha en la interfaz y puertos seleccionados
         *
         */
        void listen();

        /**
         * @brief Deja de escuchar nuevas conexiones.
         *
         */
        void close();

    protected:

        /** Establece la factoría de clases que se creará cada vez que llegue un nuevo paquete.
          El constructor debe aceptar un descriptor de socket de tipo entero.
          Ej: Para elegir la clase TcpServerThread se llamaría a:
                setRunnableInstance(&createRunnableInstance<TcpServerThread>;
          @param pfn_runnable_factory Variable de tipo runnable_factory_ptr que devuelve
            una instancia de un QRunnable.
          @note Puede usar la función TcpServer::setRunnableFactory<>() en lugar de esta.
          */
        void setRunnableFactoryInner(runnable_factory_ptr pfn_runnable_factory);
        void setRunnableFactoryInner(ClassGenParent<int> * pClassGen);

    };

}

#endif // SAM_TCPSERVER_H
