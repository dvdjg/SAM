/** @file SAM_Types.h
*   @brief Basic C/C++ types.
*   @date   21-04-2008 (dd-mm-yyyy)
*   @author David Jurado Gonzalez
*   @version 1.0
*/

#ifndef SAM_TYPES_H
#define SAM_TYPES_H


// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#  pragma once
#endif

#ifdef _WIN32
#  define DO_PACK(decl) /*# pragma pack(1)*/ decl
#else
#  define DO_PACK(decl) decl __attribute__((__packed__))
#endif

namespace SAM
{
    typedef char                int8;   ///< Tipo equivalente a `char`. Usado para tener un nombre inequívoco para el parsing.
    typedef unsigned char       uint8;  ///< Tipo equivalente a `unsigned char`. Usado para tener un nombre inequívoco para el parsing.
    typedef short               int16;  ///< Tipo equivalente a `short`. Usado para tener un nombre inequívoco para el parsing.
    typedef unsigned short      uint16; ///< Tipo equivalente a `unsigned short`. Usado para tener un nombre inequívoco para el parsing.
    typedef int                 int32;  ///< Tipo equivalente a `int`. Usado para tener un nombre inequívoco para el parsing.
    typedef unsigned int        uint32; ///< Tipo equivalente a `unsigned int`. Usado para tener un nombre inequívoco para el parsing.
    typedef long long           int64;  ///< Tipo equivalente a `long long`. Usado para tener un nombre inequívoco para el parsing.
    typedef unsigned long long  uint64; ///< Tipo equivalente a `unsigned long long`. Usado para tener un nombre inequívoco para el parsing.

    template<typename E, typename S>
    struct Enum
    {
    public:
        typedef Enum<E, S> self_type;
        typedef E enum_type;
        typedef S storage_type;

        S data; /**< Tipo enumerado encapsulado */

        self_type & operator = (const E & d) { set(d); return *this; }
        self_type & operator = (const S & d) { data = d; return *this; }

        enum_type get() const { return (enum_type) data; }
        void set(const E & d) { data = (S) d; }

        operator S() const { return data; }
        operator E() const { return (E) get(); }

        bool operator ==(const self_type & eType) const { return eType.get() == get(); }
        bool operator !=(const self_type & eType) const { return eType.get() != get(); }
        bool operator ==(E eType) const { return eType == get(); }
        bool operator !=(E eType) const { return eType != get(); }
        bool operator <(E eType) const { return (int)get() < (int)eType; }
        void clear() { set(E(-1)); }
        bool isClear() const { return get() == E(-1); }
    };

    /**
     *  Especialización parcial por si se quiere usar el propio tipo enumerado
     *  nativo para almacenarlo.
     */
    template<typename E>
    struct Enum<E, E>
    {
    public:
        typedef Enum< E, E> self_type;
        typedef E enum_type;
        typedef E storage_type;

        E data; /**< Tipo enumerado encapsulado */

        self_type & operator = (const E & d) { set(d); return *this; }

        enum_type get() const { return (enum_type) data; }

        operator E() const { return (E) get(); }
        void set(const E & d) { data = d; }

        bool operator ==(const self_type & eType) const { return eType.get() == get(); }
        bool operator !=(const self_type & eType) const { return eType.get() != get(); }
        bool operator ==(E eType) const { return eType == get(); }
        bool operator !=(E eType) const { return eType != get(); }
        bool operator <(E eType) const { return (int)get() < (int)eType; }
        void clear() { set(E(-1)); }
        bool isClear() const { return get() == E(-1); }
    };

} // namespace SAM




#endif // SAM_TYPES_H
