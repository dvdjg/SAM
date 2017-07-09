#ifndef SAM_INJECTOR_H
#define SAM_INJECTOR_H

#include <QString>

#include "SAM_CommandObject.h"


namespace SAM
{

    class InjectorPrivate;

    /**
     * @brief Inyecta a la red archivos previamente esnifados y almacenados en formato JSON
     *  tal como los genera SAM::Sniffer.
     */
    class Injector : public CommandObject
    {
        Q_OBJECT

        Q_PROPERTY( qint64 bytesInjectedCount READ bytesInjectedCount )
        Q_PROPERTY( uint packetInjectedCount READ packetInjectedCount )

        J_DECLARE_PRIVATE(Injector)
        Q_DISABLE_COPY(Injector)

    protected:
        Injector(QObject * parent, int);

    public:
        explicit Injector(QObject * parent = 0);

        /**
         * @brief Establece el nombre del archivo a esnifar. Si su extensión es .gz
         *  lo descomprime automáticamente.
         *
         * @param fileName
         */
        void setFileName(QString fileName);

        /**
         * @brief Número de paquetes inyectados
         *
         * @return uint
         */
        uint packetInjectedCount() const;

        /**
         * @brief Número de bytes inyectados
         *
         * @return qint64
         */
        qint64 bytesInjectedCount() const;

    signals:
        /**
         * @brief Notifica que el archivo completo ha sido inyectado.
         */
        void fileInjected();

    public slots:
        /**
         * @brief Da la orden para empezar a inyectar el archivo a la red.
         */
        void injectFile();

    protected slots:
        /**
         * @brief Inyecta a la red el siguiente paquete.
         */
        void injectPacket();

    protected:

        void injectPacketInner();
    };

}

#endif // SAM_INJECTOR_H
