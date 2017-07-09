#ifndef SAM_PACKET_H
#define SAM_PACKET_H

#include <QMetaType>
#include <QSharedData>
#include <QString>
#include "declare_private.h"

class QByteArray;
class QDateTime;
class QVariant;


namespace SAM
{
    class PacketPrivate;

    /** @brief Paquete de datos para enviar o recibir por TCP/UDP
      */
    class Packet
    {
        Q_PROPERTY(QByteArray byteArray READ byteArray WRITE setByteArray)
        Q_PROPERTY(QString origin READ origin WRITE setOrigin)
        Q_PROPERTY(QVariant userData READ userData WRITE setUserData)
        Q_PROPERTY(QDateTime dateTime READ dateTime)

        J_DECLARE_PRIVATE(Packet)

    public:
        Packet();
        Packet(const Packet & other);
        Packet(const QByteArray & byteArray, QString origin = QString());
        Packet(const QString & string, QString origin = QString());
        ~Packet();

        Packet & operator = (const Packet & other);

        /**
         * @brief Acceso constante al array contenido.
         *
         * @return QByteArray
         */
        const QByteArray & byteArray() const;

        /**
         * @brief Establece el array contenido.
         */
        void setByteArray(const QByteArray &ba);

        /**
         * @brief Devuelve el identificador del originador del paquete.
         *
         * @return QString
         */
        const QString & origin() const;

        /**
         * @brief Establece el identificador del originador del paquete.
         */
        void setOrigin(const QString & org);

        /**
         * @brief Devuelve los datos de usuario.
         *
         * @return QVariant
         */
        const QVariant & userData() const;

        /**
         * @brief Establece unos datos de usuario.
         */
        void setUserData(const QVariant & data);

        /**
         * @brief Devuelve el momento en que el paquete fue creado.
         *
         * @return QTime
         */
        const QDateTime & dateTime() const;

    protected:
        QSharedDataPointer<PacketPrivate> m_ptr;
    };

}

Q_DECLARE_METATYPE(SAM::Packet)

#endif // SAM_PACKET_H
