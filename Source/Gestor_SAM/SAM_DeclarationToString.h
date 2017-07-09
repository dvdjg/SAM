#ifndef SAM_DECLARATIONTOSTRING_H
#define SAM_DECLARATIONTOSTRING_H

#include <QVariant>
#include <QDateTime>
#include <cstring>

#include "SAM_NetworkOrder.h"
#include "SAM_DeclarationParser.h"

namespace SAM
{
    class Debug;
}

namespace SAM_DeclarationParser
{

    /**
     * @brief Función genérica para obtener el nombre de un elemento de un tipo enumerado.
     *
     * @param elem
     * @return QString
     */
    template<typename enumT>
    QString getEnumElemName(const enumT & elem)
    {
        static QString strNull("NULL");
        QString ret;

        const char * szElemName;
        if(getEnumElemName(&szElemName, elem)) {
            ret = szElemName;
        } else {
            if((int) elem < 0) {
                ret = strNull;
            } else {
                ret.setNum((int) elem);
            }
        }

        return ret;
    }

    template<typename E, typename S>
    QString getEnumElemName(const SAM::Enum<E,S> & elem)
    {
        return getEnumElemName(elem.get());
    }

    /**
     * @brief Obtiene una fecha en formato Qt a partir de la estructura
     *  usada en comunicaciones SAM_timeb.
     *
     * @param timeBE
     * @return QDateTime
     */
    inline QDateTime jFromBE(const SAM_timeb & timeBE)
    {
        qint64 msecs = SAM::ntoh(timeBE.time) * 1000 + SAM::ntoh(timeBE.millitm);

        return QDateTime::fromMSecsSinceEpoch(msecs);
    }

    /**
     * @brief Convierte una fecha QDateTime al tipo
     *  SAM_timeb usado en comunicaciones.
     *
     * @param dt
     * @return SAM_timeb
     */
    inline SAM_timeb toSAM_timebBE(const QDateTime & dt)
    {
        SAM_timeb ret;
        std::memset(&ret, 0, sizeof(ret));

        ret.time = SAM::ntoh<time_t>(dt.toTime_t());
        ret.millitm = SAM::ntoh<uint16>(dt.toMSecsSinceEpoch() % 1000);

        return ret;
    }

    /**
     * @brief Convierte una fecha QDateTime encapsulada en un QVariant al tipo
     *  SAM_timeb usado en comunicaciones.
     *
     * @param time
     * @return SAM_timeb
     */
    inline SAM_timeb toSAM_timebBE(const QVariant & time)
    {
        QDateTime dt = time.toDateTime();

        return toSAM_timebBE(dt);
    }

    /**
     * @brief Clase auxiliar para ayudar a convertir entre tipos enumerados y
     *  sus representaciones en forma de cadenas de caracteres.
     */
    template <typename E, typename T = E>
    class EnumHolder : public Enum<E, T>
    {
    public:
        typedef Enum<E, T> parent_type;
        typedef EnumHolder<E> self_type;

        //E data; /**< Tipo enumerado encapsulado */

        template <typename S>
        EnumHolder(const Enum<E, S> & eType)  { parent_type::set(eType.get()); }
        EnumHolder(const E eType = (E)-1)  { parent_type::set(eType); }
        EnumHolder(const QString & strType)
        {
            E d = (E)-1;
            getEnumElemId(d, qPrintable(strType));
            set(d);
        }

        EnumHolder(const QVariant & varType)
        {
            E d = E();
            QString strType(varType.toString());
            getEnumElemId(d, qPrintable(strType));
            parent_type::set(d);
        }

        QString toString() const
        {
            return getEnumElemName(parent_type::get());
        }

        EnumHolder<E> & operator = (const E & d) { parent_type::set(d); return *this; }
        template <typename S>
        EnumHolder<E> & operator = (const Enum<E, S> & d) { parent_type::set(d.get()); return *this; }

        EnumHolder & operator = (const QString & strType)
        {
            E en = E();

            if(getEnumElemId(en, qPrintable(strType)))
                parent_type::set(en);

            return *this;
        }

        EnumHolder & operator = (const QVariant & varType)
        {
            return *this = varType.toString();
        }

        operator parent_type () const
        {
            self_type t;
            t.set(parent_type::get());
            return t;
        }

        operator parent_type& ()
        {
            return *(parent_type*) this;
        }

        operator QString () const
        {
            return toString();
        }

        operator QVariant () const
        {
            return toString();
        }

        bool operator ==(const QString & strType) const
        {
            E e;
            bool bRet = getEnumElemId(e, qPrintable(strType)) && e == parent_type::get();
            return bRet;
        }

        bool operator ==(const char * szElemName) const
        {
            E e;
            bool bRet = getEnumElemId(e, szElemName) && e == parent_type::get();
            return bRet;
        }

        bool operator !=(const QString & strType) const
        {
            return !(*this == strType);
        }

        bool operator !=(const char * szElemName) const
        {
            return !(*this == szElemName);
        }

    };

    /// Función auxiliar para serializar EnumHolder<> hacia SAM::Debug
    template <typename E>
    SAM::Debug & operator<<(SAM::Debug& os, const EnumHolder<E>& var)
    {
        return os << var.toString();
    }
}

#endif // SAM_DECLARATIONTOSTRING_H
