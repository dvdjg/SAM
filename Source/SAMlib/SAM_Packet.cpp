
#include "SAM_Packet.h"

#include <QByteArray>
#include <QDateTime>
#include <QVariant>

namespace SAM
{
    /** @brief Paquete de datos para enviar o recibir por TCP/UDP
      */
    class PacketPrivate : public QSharedData
    {
        friend class Packet;

    public:
        PacketPrivate() :
            m_dateTime(QDateTime::currentDateTime())
        {}
        PacketPrivate(const PacketPrivate & other) :
            QSharedData(other),
            m_dateTime(other.m_dateTime),
            m_origin(other.m_origin),
            m_userData(other.m_userData),
            m_byteArray(other.m_byteArray)
        {}
        PacketPrivate(const QByteArray & byteArray, QString origin = QString()) :
            m_dateTime(QDateTime::currentDateTime()),
            m_origin(origin),
            m_byteArray(byteArray)
        {}
        PacketPrivate(const QString & string, QString origin = QString())  :
            m_dateTime(QDateTime::currentDateTime()),
            m_origin(origin),
            m_byteArray(string.toUtf8())
        {}

        ~PacketPrivate() {}

        const QByteArray & byteArray() const { return m_byteArray; }

        QByteArray & byteArray() { return m_byteArray; }

        const QString & origin() const { return m_origin; }

        QString & origin() { return m_origin; }

        const QVariant & userData() const { return m_userData; }

        QVariant & userData() { return m_userData; }

        const QDateTime & dateTime() const { return m_dateTime; }

    protected:
        QDateTime m_dateTime; ///< Momento en el que se creó el paquete.
        QString m_origin; ///< Indentificación del que originó el mensaje.
        QVariant m_userData; ///< Información adicional de usuario
        QByteArray m_byteArray; ///< Datos del paquete.
    };



    Packet::Packet()
    {
        m_ptr = new PacketPrivate();
    }

    Packet::Packet(const Packet & other) :
        m_ptr(other.m_ptr)
    {
    }

    Packet::Packet(const QByteArray & byteArray, QString origin)
    {
        m_ptr = new PacketPrivate(byteArray, origin);
    }

    Packet::Packet(const QString & string, QString origin)
    {
        m_ptr = new PacketPrivate(string, origin);
    }

    Packet::~Packet() {}

    Packet & Packet::operator = (const Packet & other)
    {
        m_ptr = other.m_ptr;
        return *this;
    }

    const QByteArray & Packet::byteArray() const
    {
        J_D(const Packet);
        return d->m_byteArray;
    }

    void Packet::setByteArray(const QByteArray &ba)
    {
        J_D(Packet);
        d->m_byteArray = ba;
    }

    const QString & Packet::origin() const
    {
        J_D(const Packet);
        return d->m_origin;
    }

    void Packet::setOrigin(const QString & org)
    {
        J_D(Packet);
        d->m_origin = org;
    }

    const QVariant & Packet::userData() const
    {
        J_D(const Packet);
        return d->m_userData;
    }

    void Packet::setUserData(const QVariant & data)
    {
        J_D(Packet);
        d->m_userData = data;
    }

    const QDateTime & Packet::dateTime() const
    {
        J_D(const Packet);
        return d->m_dateTime;
    }

}
