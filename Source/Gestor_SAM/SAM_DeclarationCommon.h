#ifndef SAM_DECLARATIONCOMMON_H
#define SAM_DECLARATIONCOMMON_H

#include <QtGlobal>

#ifdef _WIN32
//#include <Winsock2.h>
#define TIGHLY_PACKED
#define isWindows() (true)
#else
//#include <netinet/in.h>
//#include <arpa/inet.h>

#define TIGHLY_PACKED __attribute__ ((packed))
#define isWindows() (false)
#endif

#include "SAM_Types.h"
#include "typet.hpp"
#include "ifthenelse.hpp"
#include "get_typet.hpp"

#ifndef AR_SERIALIZATION
# define AR_SERIALIZATION_L(o, m, i) my_make_four(&o.m, #m, i, &o.m[i])
# define AR_SERIALIZATION_M(o, m) my_make_four(&o.m, #m, -1, &o.m)
# define AR_SERIALIZATION(o) my_make_four(&o, #o, -1, &o)
#endif // AR_SERIALIZATION


namespace SAM_DeclarationParser
{
    using namespace std;

    template<class T> const char * getTypeName() { return "*INVALID*"; }

    class sam_enum_type {};

    template<typename A, typename B>
    class pair
    {
    public:
        pair() : first(A()), second(B()) { }
        pair(const A & a, const B & b) : first(a), second(b) {}

        A first;
        B second;
    };

    template<typename A, typename B, typename C, typename D>
    class four
    {
    public:
        four() : first(A()), second(B()), third(B()), fourth(B()) {}
        four(const A & a, const B & b, const C & c, const D & d)
            : first(a), second(b), third(c), fourth(d) {}

        A first;
        B second;
        C third;
        D fourth;
    };

    template<class Ta, class Tb>
    inline pair<Ta, Tb> my_make_pair(Ta val1, Tb val2)
    {	// return pair composed from arguments
        return (pair<Ta, Tb>(val1, val2));
    }

    template<class Ta, class Tb, class Tc, class Td>
    inline four<Ta, Tb, Tc, Td> my_make_four(Ta val1, Tb val2, Tc val3, Td val4)
    {	// return three composed from arguments
        return (four<Ta, Tb, Tc, Td>(val1, val2, val3, val4));
    }

    /// Dirección IPv4 de 4 bytes
    struct IPv4
    {
        //IPv4(unsigned int addr) : s_addr(addr) {}

        unsigned int s_addr;

        operator unsigned int() const { return s_addr; }
    };

    template<typename enumT>
    struct EnumElemToString
    {
        enumT lElem;
        const char * szName;
    };

    /**
     * @brief Obtiene el tipo enumerado correspondiente al nombre que se le pase
     *  como argumento.
     * @param elem  El tipo enumerado que se ha encontrado con el nombre de ´szElemName´.
     * @param szElemName El nombre del tipo enumerado tal como aparece en el código C++.
     * @return bool Verdadero si se ha encontrado equivalencia.
     */
//    template<typename enumT>
//    inline bool getEnumElemId( enumT & elem, const char * szElemName )
//    {
//        Q_UNUSED(szElemName)
//        Q_UNUSED(elem)
//        return false;
//    }

    /**
     * @brief Dado un tipo enumerado, devuelve el nombre que le corresponde
     *  tal como está expresado en el código C++.
     * @param elem Tipo enumerado del que se requiere su nombre.
     * @param pszElemName Dirección de un puntero que será rellenado con
     *  el nombre del tipo enumerado solicitado.
     * @return bool Verdadero si se ha conseguido encontrar el tipo enumerado.
     */
    template<typename enumT>
    inline bool getEnumElemName( const char ** pszElemName, const enumT & elem)
    {
        Q_UNUSED(pszElemName)
        Q_UNUSED(elem)
        return false;
    }

    template<typename E, typename S>
    inline bool getEnumElemName( const char ** pszElemName, const SAM::Enum<E, S> & elem)
    {
        return getEnumElemName(pszElemName, elem.get());
    }

    /**
     * @brief Función auxiliar para serializar cualquier tipo ´T´ de datos
     *  a través de un archivador ´Archive´.
     * @param ar Archivador que realizará transformaciones de datos
     * @param t Dato a transformar.
     */
    template<typename Archive, typename T>
    void serialize(Archive & ar, T & t, int nElem, generic_type = generic_type())
    {
        Q_UNUSED(nElem)

        serialize(ar, t);
    }

    template<typename Archive, typename T>
    void serialize(Archive & ar, T & t)
    {
        ar & t;
    }

    template<typename Archive, typename T>
    void serialize(Archive & ar, T & t, int nElem, array_type)
    {
        if(nElem > 0)
        {
            for(int i = 0; i < nElem; ++i)
                serialize(ar, t[i]);
        }
    }

    template<typename Archive, typename T>
    void serialize(Archive & ar, T & t, int nElem, enum_type)
    {
        serialize(ar, t);
    }

    template<typename Archive, typename T>
    void serialize(Archive & ar, const T & t, int nElem = -1, generic_type g = generic_type())
    {
        serialize(ar, (T &) t, nElem, g);
    }


    // Serialization proxy for enums
    template<typename Archive, typename T>
    void serialize_scv_enum(Archive & ar, T & t, int nElem)
    {
        char *pCh = (char *) &t;

        for(int i = 0; i < nElem; ++i)
            ar & pCh[i];
    }

}

#endif // SAM_DECLARATIONCOMMON_H
