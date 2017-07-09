#ifndef SAM_ARCHIVE_H
#define SAM_ARCHIVE_H

#include <iostream>
#include <sstream>
#include <utility>
#include <QHostAddress>

#include "SAM_DeclarationToString.h"

#include "SAM_Log.h"
#include "SAM_Types.h"
#include "SAM_NetworkOrder.h"
#include "SAM_JVariant.h"
#include "typet.hpp"
#include "ifthenelse.hpp"
#include "get_typet.hpp"

namespace SAM_DeclarationParser
{
    using namespace std;

    /**
     * @brief Convirte cualquier estructura del SCV en su representación Json.
     *
     */
    class InJsonArchive
    {
        SAM::JVariant m_data;
    public:
        typedef InJsonArchive self_t;

        InJsonArchive() {}
        InJsonArchive(const QVariant & other) : m_data(other) {}

        const SAM::JVariant & data() const { return m_data; }

        template<typename T>
        self_t& getValue(T & t, enum_type)
        {
            T e;

            if(getEnumElemId(e, qPrintable(QString(m_data)))) {
                t = SAM::ntoh(e);
            } else {
                // Si no se encuentra el nombre del tipo enumerado
                // almacena un número 0.
                t = (T) 0;

                LOG_WARN(PARSNG) << "No se encuentra el tipo enumerado"
                                 << getTypeName<T>() << m_data.toString();
            }

            return *this;
        }

        template<typename T, typename Y>
        self_t& getValue(T & t, Y = 0)
        {
            T host = m_data;

            t = SAM::ntoh(host);

            return *this;
        }

        template<typename E, typename S, typename Y>
        self_t& getValue(Enum<E,S> & t, Y = 0)
        {
            E e;
            getValue(e, enum_type());
            t = e;

            return *this;
        }

        template<typename Y>
        self_t& getValue(SAM_timeb & t, Y = 0)
        {
            t = toSAM_timebBE(m_data);

            return *this;
        }


        template<typename Y>
        self_t& getValue(unsigned char & t, Y = 0)
        {
            t = m_data;

            return *this;
        }


        template<typename Y>
        self_t& getValue(char & t, Y = 0)
        {
            QVariant::Type type = m_data.type();

            t = '?';

            if(type == QVariant::String || type == QVariant::Char) {
                QString str = m_data.toString();
                if(str.length() > 0)
                    t = m_data.toString().at(0).toLatin1();
            } else {
                t = (int) m_data;
            }

            return *this;
        }

        template<typename T>
        self_t& operator &(T & t)
        {
            typedef typename IfThenElse<TypeT<T>::IsEnumT,
                    enum_type,
                    generic_type
                    >::ResultT Type;

            getValue(t, Type());

            return *this;
        }

        template<unsigned N>
        self_t& operator &(char (&a)[N])
        {
            mserialize(a, N, 0);

            return *this;
        }

        template<unsigned N>
        self_t& operator &(const char (&a)[N])
        {
            typedef char (&toarray)[N];

            return *this & (toarray) a;
        }


        self_t & operator &(IPv4 & t)
        {
            QHostAddress ip(m_data.toString());

            t.s_addr = SAM::ntoh(ip.toIPv4Address());

            return *this;
        }

        template<unsigned N>
        self_t& operator &(char unsigned (&a)[N])
        {
            typedef const unsigned char (&toconst)[N];
            return *this & (toconst) a;
        }

        template<typename T>
        void mserialize(T & t)
        {
            serialize(*this, t);
        }

        template<typename T, typename Y>
        void mserialize(T & t, int, Y)
        {
            serialize(*this, t);
        }

        template<typename T, unsigned N, typename Y>
        void mserialize(T (&a)[N], int, Y)
        {
            QList<QVariant> lst = m_data.toList();

            int min = qMin(lst.size(), (int) N);

            for(int i = 0; i < min; ++i)
            {
                InJsonArchive child;
                child.m_data = lst[i];
                serialize(child, a[i]);
            }
        }

        template<unsigned N, typename Y>
        void mserialize( unsigned char (&a)[N], int, Y)
        {
            QByteArray ba = m_data.toByteArray();

            int min = qMin(ba.size(), (int) N);

            for(int i = 0; i < min; ++i)
                a[i] = ba.at(i);

            if(min < (int) N)
                a[min] = 0;
        }

        template<unsigned N, typename Y>
        void mserialize( char (&a)[N], int, Y)
        {
            QString ba = m_data.toString();

            int min = qMin(ba.size(), (int) N);

            for(int i = 0; i < min; ++i)
                a[i] = ba.at(i).toLatin1();

            if(min < (int) N)
                a[min] = 0;
        }

        template<typename T, typename Y, typename F>
        self_t& operator &(const four<T *, const char*, Y, F> &p)
        {
            if(!m_data.contains(p.second))
            {
                LOG_WARN(PARSNG) << "No se encuentra el miembro"
                                 << p.second << "para rellenar la estructura";
            }
            else
            {
                InJsonArchive child(m_data[p.second]);

                typedef typename GetTypeT<T>::ResultT t_type;

                child.mserialize(*p.first, -1, t_type());
            }

            //m_data[name] = child.m_data;

            return *this;
        }
    };

    class OutJsonArchive
    {
        SAM::JVariant m_data;
    public:
        typedef OutJsonArchive self_t;

        OutJsonArchive() {}
        OutJsonArchive(const QVariant & other) : m_data(other) {}

        const SAM::JVariant & data() const { return m_data; }

        template<typename T>
        self_t& setValue(T & t, enum_type)
        {
            T host = SAM::ntoh(t);

            const char * szElemName;
            if(getEnumElemName(&szElemName, host )) {
                m_data.setValue(QString(szElemName));
            } else {
                // Si no se encuentra el nombre del tipo enumerado
                // almacena el número correspondiente
                m_data.setValue((int) host);

                LOG_WARN(PARSNG) << "No se encuentra el tipo enumerado"
                                 << getTypeName<T>() << host;
            }

            return *this;
        }

        template<typename T, typename Y>
        self_t& setValue(T & t, Y = 0)
        {
            T host = SAM::ntoh(t);

            m_data.setValue(host);

            return *this;
        }

        template<typename E, typename S, typename Y>
        self_t& setValue(Enum<E,S> & t, Y = 0)
        {
            E e = SAM::ntoh((E)t);
            return setValue(e, enum_type());
        }

        template<typename Y>
        self_t& setValue(uint16 & t, Y = 0)
        {
            uint16 host = SAM::ntoh(t);

            m_data.setValue((uint)host);

            return *this;
        }

        template<typename Y>
        self_t& setValue(int16 & t, Y = 0)
        {
            int16 host = SAM::ntoh(t);

            m_data.setValue((int)host);

            return *this;
        }

        template<typename Y>
        self_t& setValue(unsigned char & t, Y = 0)
        {
            QVariant::Type type = m_data.type();

            if(type == QVariant::UInt){
                char ch = (char) m_data.toUInt();
                m_data = QByteArray(&ch, 1); // Introduce el dato que ya había
            }

            if(m_data.type() == QVariant::ByteArray)
                m_data.asByteArray().append((char) t);
            else
                m_data.setValue((uint) t);

            return *this;
        }


        template<typename Y>
        self_t& setValue(char & t, Y = 0)
        {
            QVariant::Type type = m_data.type();

            if(type == QVariant::String || type == QVariant::Char) {
                m_data.asString().append(t);
            } else {
                QChar ch(t);
                if(ch.isPrint()) {
                    m_data.setValue(ch);
                } else {
                    m_data.setValue((int)t);
                }
            }

            return *this;
        }

        template<typename Y>
        self_t& setValue(SAM_timeb & t, Y = 0)
        {
            m_data = jFromBE(t);

            return *this;
        }

        self_t & operator &(IPv4 & t)
        {
            unsigned int host = SAM::ntoh(t.s_addr);

            m_data.setValue(QHostAddress(host).toString());

            return *this;
        }

        template<typename T>
        self_t& operator &(T & t)
        {
            typedef typename IfThenElse<TypeT<T>::IsEnumT,
                    enum_type,
                    generic_type
                    >::ResultT Type;

            setValue(t, Type());

            return *this;
        }

        template<unsigned N>
        self_t& operator &(char unsigned (&a)[N])
        {
            typedef const unsigned char (&toconst)[N];
            return *this & (toconst) a;
        }

        template<typename T>
        void mserialize(T & t)
        {
            serialize(*this, t);
        }

        template<typename T, typename Y>
        void mserialize(T & t, int, Y)
        {
            serialize(*this, t);
        }

        template<unsigned N>
        self_t& operator &(char (&a)[N])
        {
            mserialize(a, N, 0);

            return *this;
        }

        template<unsigned N>
        self_t& operator &(const char (&a)[N])
        {
            mserialize(a, N, 0);

            return *this;
        }


        template<typename T, unsigned N, typename Y>
        void mserialize(T (&a)[N], int nElem, Y)
        {
            int min = (int) N;
            if(nElem >= 0 && nElem < (int) N)
                min = nElem;

            m_data.asList().reserve(min);

            for(int i = 0; i < min; ++i)
            {
                OutJsonArchive child;

                serialize(child, a[i]);

                m_data[i] = child.m_data;
            }
        }

        template<unsigned N, typename Y>
        void mserialize( unsigned char (&a)[N], int nElem, Y)
        {
            int min = (int) N;
            if(nElem >= 0 && nElem < (int) N)
                min = nElem;

            QByteArray ba((const char*) a, min);

            // Almacenar los arrays de bytes como QByteArray
            m_data.setValue(ba);
        }

        template<unsigned N, typename Y>
        void mserialize( char (&a)[N], int nElem, Y)
        {
            int min = (int) N;
            if(nElem >= 0 && nElem < (int) N)
                min = nElem;

            //QString str = qPrintable(QString::fromLatin1(a, min));
            QString str = QString::fromLatin1(a, min).remove(QChar('\0'));

            m_data.setValue(str);
        }

        template<typename T, typename Y, typename F>
        self_t& operator &(const four<T *, const char*, Y, F> &p)
        {
            OutJsonArchive child;
            QString name(p.second);

//            if(name == "enlace") estadoConfigurado
//            {
//                logDebug();
//            }

            typedef typename GetTypeT<T>::ResultT t_type;

            Y nElem = SAM::ntoh(p.third);

            child.mserialize(*p.first, nElem, t_type());

            // Se evita que se serialice cualquier variable miembro que se llame `_dummy____`
            if(name != "_dummy____")
            {
                if(m_data.contains(name))
                {
                    SAM::JVariant & value = m_data[name];

                    // Si ya existe otro elemento con el mismo nombre...
                    if(value.type() != QVariant::List)
                    {   // Si no era una lista, la crea y rellena.
                        SAM::JVariant temp = value;
                        value.asList().append(temp);
                    }

                    value.asList().append(child.m_data);
                }
                else
                {
                    m_data[name] = child.m_data;
                }
            }

            return *this;
        }
    };

    /**
     * @brief Transforma los endianness desde el protocolo de red al de la máquina.
     *  En una máquina LittleEndian como los x86, invierte el orden de los bytes
     *  de todos los números enteros mayores de 1 byte.
     */
    class InBinInetArchive
    {
        istream & m_is;
    public:
        typedef InBinInetArchive self_t;
        InBinInetArchive(istream & is) : m_is(is) {}
        ~InBinInetArchive() {}

        istream & is()
        {
            return m_is;
        }

        template<typename T>
        istream & is(T & t)
        {
            return m_is.read((char*) &t, sizeof(T));
        }

        istream & is(unsigned short & t)
        {
            unsigned short data;
            m_is.read((char*) &data, sizeof(data));
            t = ntoh(data);

            return m_is;
        }

        istream & is(short & t)
        {
            short data;
            m_is.read((char*) &data, sizeof(data));
            t = ntoh(data);

            return m_is;
        }

        istream & is(unsigned int & t)
        {
            unsigned int data;
            m_is.read((char*) &data, sizeof(data));
            t = ntoh(data);

            return m_is;
        }

        istream & is(int & t)
        {
            int data;
            m_is.read((char*) &data, sizeof(data));
            t = ntoh(data);

            return m_is;
        }

        istream & is(unsigned long & t)
        {
            unsigned long data;
            m_is.read((char*) &data, sizeof(data));
            t = (unsigned long) ntoh(data);

            return m_is;
        }

        istream & is(IPv4 & t)
        {
            unsigned int data;
            m_is.read((char*) &data, sizeof(data));
            t.s_addr = (unsigned int) ntoh(data);

            return m_is;
        }

        istream & is(long & t)
        {
            long data;
            m_is.read((char*) &data, sizeof(data));
            t = ntoh(data);

            return m_is;
        }

        template<typename T>
        self_t& operator &(T & t)
        {
            is(t);

            return *this;
        }

        template<typename T, unsigned N>
        self_t& operator &(T (&a)[N])
        {
            for(unsigned i = 0; i < N; ++i)
                serialize(*this, a[i]);

            return *this;
        }

        template<typename T, typename Y, typename F>
        self_t& operator &(const four<T *, const char*, Y, F> &p)
        {
            serialize(*this, *p.first);

            return *this;
        }
    };

    /**
     * @brief Transforma los endianness desde el protocolo de la máquina al de red.
     *  En una máquina LittleEndian como los x86, invierte el orden de los bytes
     *  de todos los números enteros mayores de 1 byte.
     */

    class OutBinInetArchive
    {
        ostream & m_os;
    public:
        typedef OutBinInetArchive self_t;
        OutBinInetArchive(ostream & os) : m_os(os) {}
        ~OutBinInetArchive() {}

        ostream & os()
        {
            return m_os;
        }

        template<typename T>
        ostream & os(T & t)
        {
            return m_os.write((const char*) &t, sizeof(T));
        }

        ostream & os(unsigned short t)
        {
            unsigned short data = hton(t);
            return m_os.write((const char*) &data, sizeof(data));
        }

        ostream & os(short t)
        {
            short data = hton(t);
            return m_os.write((const char*) &data, sizeof(data));
        }

        ostream & os(unsigned int t)
        {
            unsigned int data = hton(t);
            return m_os.write((const char*) &data, sizeof(data));
        }

        ostream & os(int t)
        {
            unsigned int data = hton((unsigned int) t);
            return m_os.write((const char*) &data, sizeof(data));
        }

        ostream & os(unsigned long t)
        {
            unsigned long data = hton(t);
            return m_os.write((const char*) &data, sizeof(data));
        }

        ostream & os(const IPv4 & t)
        {
            return m_os << (unsigned long) t.s_addr;
        }

        ostream & os(long t)
        {
            long data = hton(t);
            return m_os.write((const char*) &data, sizeof(data));
        }

        //template<typename T>
        //self_t& operator &(T & t)
        //{
        //    os(t);

        //    return *this;
        //}

        template<typename T>
        self_t& operator &(const T & t)
        {
            os(t);

            return *this;
        }

        template<typename T, unsigned N>
        self_t& operator &(T (&a)[N])
        {
            for(unsigned i = 0; i < N; ++i)
                serialize(*this, a[i]);

            return *this;
        }

        template<typename T, typename Y, typename F>
        self_t& operator &(const four<T *, const char*, Y, F> &p)
        {
            serialize(*this, *p.first);

            return *this;
        }
    };

    class OutXmlArchive
    {
        ostream & m_os;
        stringstream m_ss;
        int m_indent;

        bool m_bRaw;
        bool m_bWrap;

    public:
        typedef OutXmlArchive self_t;
        OutXmlArchive(ostream & os, bool bRaw = false, int indent = 0, bool wrap = true)
            : m_os(os), m_indent(indent), m_bRaw(bRaw), m_bWrap(wrap)
        {
            if(!m_bRaw && m_bWrap)
                m_os << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                "<serialization>\n";

            m_ss.unsetf(ios::skipws); // Don't skip whitespaces
        }
        ~OutXmlArchive()
        {
            if(!m_bRaw && m_bWrap)
                m_os << "</serialization>\n";
        }

        void setRawMode(bool bRaw) { m_bRaw = bRaw; }
        bool getRawMode() const { return m_bRaw; }

        ostream & os()
        {
            return m_os;
        }

        template<typename T>
        ostream & os(const T & t)
        {
            if(m_bRaw)
            {
                m_os << t;
            }
            else
            {
                m_ss.clear();

                m_ss << t; // Authomatic conversion of any primitive type to string

                m_os << /*raw2xml*/(m_ss);
            }

            return m_os;
        }

        ostream & os(const char *a)
        {
            if(a)
            {
                if(m_bRaw)
                    m_os << a;
                else
                    m_os << /*raw2xml*/(a);
            }

            return m_os;
        }

        ostream & os(char t)
        {
            if(m_bRaw)
                m_os << t;
            if((unsigned char)t >= ' ')
            {
                char szIn[] = {t, 0};

                m_os << '\'' << /*raw2xml*/(szIn) << '\'';
            }
            else
                m_os << (signed int) t;

            return m_os;
        }

        ostream & os(unsigned char t)
        {
            return m_os << (unsigned int) t;
        }

        ostream & os(int t)
        {
            return m_os << t;
        }

        ostream & os(bool t)
        {
            m_os << t;

            return m_os;
        }

        ostream & os(const IPv4 & t)
        {
            m_os << qPrintable(inet_ntoa( t ));

            return m_os;
        }

        template<typename T>
        self_t& operator &(const T & t)
        {
            os(t);

            return *this;
        }

        template<typename T, unsigned N>
        self_t& operator &(T (&a)[N])
        {
            for(unsigned i = 0; i < N; ++i)
            {
                m_os << "<item>";
                serialize(*this, a[i]);
                m_os << "</item>\n";
            }

            return *this;
        }

        //template<typename T, unsigned N>
        //self_t& operator &(T (&a)[N], int limit = 1000000)
        //{
        //    if(N < limit)
        //        limit = N;

        //    for(unsigned i = 0; i < limit; ++i)
        //    {
        //        m_os << "<item>";
        //        serialize(*this, a[i]);
        //        m_os << "</item>\n";
        //    }

        //    return *this;
        //}

        template<unsigned N>
        self_t& operator &(const char (&a)[N])
        {
            m_ss.clear();

            m_os << '\"';

            //m_ss.unsetf(ios::skipws); // Don't skip whitespaces

            for(unsigned i = 0; i < N && a[i]; ++i)
                m_ss << a[i];

            if(m_bRaw)
                m_os << m_ss << '\"';
            else
                m_os << /*raw2xml*/(m_ss) << '\"';

            return *this;
        }

        template<unsigned N>
        self_t& operator &(char (&a)[N])
        {
            m_ss.clear();

            m_os << '\"';

            for(unsigned i = 0; i < N && a[i]; ++i)
                m_ss << a[i];

            if(m_bRaw)
                m_os << m_ss << '\"';
            else
                m_os << /*raw2xml*/(m_ss) << '\"';

            return *this;
            //typedef const char (&typ)[N];
            //return (*this & (typ) a); // Use the previous function
        }

        template<typename T, typename Y, typename F>
        self_t& operator &(const four<T *, const char*, Y, F> &p)
        {
            if(m_indent > 0)
                m_os << '\n';

            for(int i = 0; i < m_indent; ++i)
                m_os << ' ';

            m_indent += 2;

            if(m_bRaw)
            {
                m_os << p.second << '=';

                serialize(*this, *p.first);
            }
            else
            {
                m_os << '<' << /*raw2xml*/((const char*&) p.second) << '>';

                serialize(*this, *p.first);

                m_os << "</" << /*raw2xml*/((const char*&) p.second) << '>';
            }

            m_indent -= 2;

            if(m_indent == 0)
                m_os << '\n';

            return *this;
        }
    };



}



#endif // SAM_ARCHIVE_H
