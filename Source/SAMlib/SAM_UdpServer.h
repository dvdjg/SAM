#ifndef SAM_UDPSERVER_H
#define SAM_UDPSERVER_H

#include "SAM_Packet.h"
#include "SAM_TcpUdpServer.h"

class QByteArray;
class QUdpSocket;
class QRunnable;

namespace SAM
{
    class Packet;
    class UdpServerPrivate;

    /**
     * @brief Servidor para recibir mensajes UDP.
     *
     */
    class UdpServer : public TcpUdpServer
    {
        Q_OBJECT

        J_DECLARE_PRIVATE(UdpServer)
        Q_DISABLE_COPY(UdpServer)

        Q_PROPERTY(int unprocessedBytes READ unprocessedBytes)
        Q_PROPERTY(quint64 rejectedRepeatedDatagrams READ rejectedRepeatedDatagrams)
        Q_PROPERTY(int rejectRepeatedInterval READ rejectRepeatedInterval)

    protected:
        /**
         * @brief Tipo para almacenar un puntero a función que devuelve una instancia de
         *  una clase derivada de QRunable.
         */
        typedef QRunnable *(*runnable_factory_ptr)(Packet);

        /**
         * @brief Función auxiliar para crear factorías de TcpServerThreads.
         *
         * @param packet Datos recibidos que se procesarán por la clase
         * @return QRunnable * Puntero con la nueva clase procesadora.
         */
        template<typename T>
        static QRunnable * createRunnableInstance(const Packet packet)
        {
            return new T(packet);
        }

        UdpServer(QObject *parent, int i);


    public:

        UdpServer(QObject *parent = 0);

        ~UdpServer();

        /**
         * @brief Cadena de error del socket.
         *
         * @return QString
         */
        virtual QString errorString() const;

        /**
         * @brief Establece la factoría de clases que se creará cada vez que llegue un nuevo paquete.
         * El constructor debe aceptar un QByteArray que recibirá el paquete UDP recién leido.
         * Ej: Para elegir la clase UdpServerThread se llamaría a:
         *       setRunnableInstance<UdpServerThread>();
         */
        template<typename T>
        void setRunnableFactory()
        {
            setRunnableFactoryInner(&UdpServer::createRunnableInstance<T>);
        }

        template<typename R, typename T>
        void setRunnableFactory(T t)
        {
            setRunnableFactoryInner(new ClassGen<Packet, R, T>(t));
        }

        /**
         * @brief Determina si el servidor está a la escucha.
         *
         * @return bool
         */
        virtual bool isListening() const;

        /**
         * @brief Datos binarios recibidos y no procesados en el buffer interno actual.
         */
        int unprocessedBytes() const;

        /**
         * @brief Cantidad de datagramas que se han desechado por haber detectado que es
         *  igual que otro que llegó en menos de 2 segundos.
         *
         *  @note Se ha desarrollado un sistema de detección de datagramas repetidos debido
         *  a que se utilizan aparatos de instrumentación como el SAGE que inunda la red
         *  con clones de mensajes, lo que afectaba al procesamiento normal del sistema.
         */
        quint64 rejectedRepeatedDatagrams() const;

        /**
         * @brief Se rechazan todos los paquetes repetidos que se hayan recibido en el intervalo
         *  indicado. Si vale 0 no se rechazan datagramas repetidos.
         * @return Milisegundos
         */
        int rejectRepeatedInterval() const;

        /**
         * @brief Establece el tiempo en milisegundos que se considerará a la hora de rechazar
         *  datagramas repetidos. Usar 0 si no se desea eliminar datagramas repetidos.
         *  Por defecto vale 2000 ms.
         */
        void setRepeatedInterval(int msec);

    public slots:

        /**
         * @brief Escucha en la interfaz y puertos seleccionados
         */
        void listen();

        /**
         * @brief Deja de escuchar nuevas conexiones.
         */
        void close();

    private slots:
        /**
         * @brief Acumula los datagramas recibidos en un buffer.
         */
        void incomingConnection();

    protected:
        /**
         * @brief Procesa los datagramas que se acumularon en el buffer mediante incomingConnection().
         */
        void processDatagram();

        /** Establece la factoría de clases que se creará cada vez que llegue un nuevo paquete.
          El constructor debe aceptar un QByteArray que recibirá el paquete UDP recién leido.
          Ej: Para elegir la clase UdpServerThread se llamaría a:
                setRunnableInstance(&createRunnableInstance<UdpServerThread>);
          @param pfn_runnable_factory Variable de tipo runnable_factory_ptr que devuelve una
                instancia de un QRunnable.
          @note Puede usar la función UdpServer::setRunnableFactory<>() en lugar de esta.
          */
        void setRunnableFactoryInner(runnable_factory_ptr pfn_runnable_factory);
        void setRunnableFactoryInner(ClassGenParent<Packet> * pClassGen);
    };

}

#endif // SAM_UDPSERVER_H
