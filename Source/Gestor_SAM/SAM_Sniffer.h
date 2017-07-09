#ifndef SAM_SNIFFER_H
#define SAM_SNIFFER_H

#include <QString>

#include "SAM_CommandObject.h"
#include "SAM_Packet.h"


namespace SAM
{

    class SnifferPrivate;

    /**
     * @brief Esnifa mensajes del SCV desde la red, almacenándolos en el disco duro en
     *  archivos con formato JSON.
     *  Estos archivos son compatibles con SAM::Injector, lo que permite reinyectarlos a la red.
     */
    class Sniffer : public CommandObject
    {
        Q_OBJECT

        Q_PROPERTY( QString snifferFileName READ snifferFileName )
        Q_PROPERTY( bool isSnifferFileOpen READ isSnifferFileOpen )

        J_DECLARE_PRIVATE(Sniffer)
        Q_DISABLE_COPY(Sniffer)

    protected:
        Sniffer(QObject * parent, int);

    public:
        explicit Sniffer(QObject * parent = 0);

        /** Nombre del archivo de Sniffing que se está usando. */
        QString snifferFileName() const;

        /** Determina si el archivo de Sniffing está abierto. */
        bool isSnifferFileOpen() const;

        /**
         * @brief Vuelca una copia del mensaje al disco duro.
         *
         * @param msgPacket
         */
        void sniffMessage(const Packet msgPacket);

    public slots:

        /**
         * @brief Analiza el paquete binario conteniendo uno o más mensajes
         *  de tipo MSG_TH codificados en BigEndian.
         *  Cada vez que encuentre un mensaje válido, llama a su vez a MessageProcessor::processMessage().
         * @param packet Paquete de datos con uno o más mensajes MSG_TH
         *  seguidos codificados en bigendian.
         * @return bool
         */
        int parseBinaryPacket(Packet packet);

        /**
         * @brief Procesa un mensaje SCV-SAM
         *
         * @param msgPacket Un mensaje SCV-SAM enpaquetado con el endian del anfitrión
         * @return bool Devuelve true si se ha podido procesar el mensaje.
         */
        bool processMessage(Packet msgPacket);
    };

}

#endif // SAM_SNIFFER_H
