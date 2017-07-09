#ifndef SAM_TCPUDPSERVER_H
#define SAM_TCPUDPSERVER_H

#include <QUrl>

#include "SAM_CommandObject.h"
class QRunnable;

namespace SAM
{
    class TcpUdpServerPrivate;

    /**
     * @brief Funcionalidad común a los servidores TCP/UDP.
     *  Un servidor TcpUdpServer es un objeto que escucha tráfico en un puerto
     *  local de la máquina.
     */
    class TcpUdpServer : public CommandObject
    {
        Q_OBJECT

        Q_PROPERTY( QUrl url READ url )
        Q_PROPERTY( int readBufferSize READ readBufferSize WRITE setReadBufferSize )
        Q_PROPERTY( int retryTimeLapse READ retryTimeLapse WRITE setRetryTimeLapse )
        Q_PROPERTY( int incomingConnectionsCount READ incomingConnectionsCount )
        Q_PROPERTY( bool isListening READ isListening )

        J_DECLARE_PRIVATE(TcpUdpServer)
        Q_DISABLE_COPY(TcpUdpServer)

    protected:
        template<typename S>
        class ClassGenParent
        {
        public:
            virtual ~ClassGenParent() {}
            /** Función auxiliar para crear factorías de QRunnables.
              */
            virtual QRunnable * createRunnableInstance(S param) = 0;
        };

        template<typename S, typename R, typename T>
        class ClassGen : public ClassGenParent<S>
        {
            T m_t;

        public:
            explicit ClassGen(const T & t) : m_t(t) {}
            virtual ~ClassGen() {}

            /** Función auxiliar para crear factorías de QRunnables.
              */
            QRunnable * createRunnableInstance(S param)
            {
                return new R(param, m_t);
            }
        };

    protected:
        TcpUdpServer(QObject *parent, int);

    public:
        explicit TcpUdpServer(QObject *parent = 0);
        virtual ~TcpUdpServer();

        /**
         * @brief Devuelve el URL con la que se configuró el servidor.
         *
         * @return QUrl
         */
        virtual const QUrl & url() const;

        /**
         * @brief Devuelve el URL con la que se configuró el servidor.
         *
         * @return QUrl
         */
        virtual void setUrl(const QUrl & url);

        /**
         * @brief Tiempo en milisegundos entre reintentos de escucha. A 0 para no seguir intentándolos.
         *
         * @return int
         */
        int retryTimeLapse() const;

        /**
         * @brief Fija el tiempo en milisegundos entre reintentos de escucha. A 0 para no seguir intentándolos.
         *
         * @param retryTimeLapse
         */
        void setRetryTimeLapse(int retryTimeLapse);

        /**
         * @brief Cadena de error del socket.
         *
         * @return QString
         */
        virtual QString errorString() const = 0;

        /**
         * @brief Tamaño del buffer de la pila de recepción de datos o 0 si se
         *  utiliza el tamaño redeterminado.
         *  - UDP: Afecta a la pila del Sistema Operativo.
         *  - TCP: Afecta a la biblioteca Qt.
         */
        virtual int readBufferSize() const;

        /**
         * @brief Establece el tamaño del buffer de la pila de recepción de datos.
         *  - UDP: Afecta a la pila del Sistema Operativo.
         *  - TCP: Afecta a la biblioteca Qt.
         *
         * @param readBufferSize Usar 0 para seleccionar el tamaño predeterminado.
         */
        virtual void setReadBufferSize(int readBufferSize);

        /**
         * @brief Número de conexiones que se han atendido.
         *
         * @return int
         */
        virtual int incomingConnectionsCount() const;

        /**
         * @brief Determina si el servidor está a la escucha.
         *
         * @return bool
         */
        virtual bool isListening() const = 0;

    signals:

    public slots:
        /**
         * @brief Escucha en la interfaz y puertos seleccionados
         *
         * @return bool
         */
        virtual void listen() = 0;

        /**
         * @brief Deja de escuchar nuevas conexiones.
         *
         */
        virtual void close() = 0;

    };

}

#endif // SAM_TCPUDPSERVER_H
