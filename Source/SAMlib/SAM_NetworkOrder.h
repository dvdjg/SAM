
#ifndef NetworkOrder_h__
#define NetworkOrder_h__

#include <QString>

namespace SAM
{
    /**
     * @brief Clase auxiliar para determinar el Endianness del anfitrión.
     *
     *  @sa http://www.realtime.bc.ca/articles/endian-safe
     */
    union HostEndianness
    {
        int i;
        char c[sizeof(int)];

        HostEndianness() : i(1) { }

        bool isBig() const { return c[0] == 0; }
        bool isLittle() const { return c[0] != 0; }
    };

    /**
     * @brief Intercambia los Bytes si el anfitrión es Little Endian
     *
     * @param arg
     * @return T
     */
    template<typename T>
    static T ntoh(const T& arg)
    {
        if (HostEndianness().isBig())
            return arg;
        else
        {
            T ret;

            char* dst = reinterpret_cast<char*>(&ret);
            const char* src = reinterpret_cast<const char*>(&arg + 1);

            for (size_t i = 0; i < sizeof(T); i++)
                *dst++ = *--src;

            return ret;
        }
    }

    template<typename T>
    inline T hton(const T& arg)
    {
        return ntoh(arg);
    }

    /* Código modificado de: https://groups.google.com/forum/?hl=en&fromgroups=#!msg/comp.lang.c++/E-JDskfQPM8/XyV6eb6xHQYJ
     */

    static inline bool IsBigEndianbool()
    {
        unsigned x = 1;
        return ! ( * ( char * )( & x ) );
    }

    template <class base_type>
    base_type OrderRead(const base_type & i_val)
    {
        base_type i_destination;
        unsigned char * src = ( unsigned char * ) & i_val;
        unsigned char * dst = ( unsigned char * ) & i_destination;

        if (( sizeof( base_type ) == 1 ) || IsBigEndianbool())
        {
            //
            // Alignment is an issue some architectures so
            // even for non-swapping we read a byte at a time

            if ( sizeof( base_type ) == 1 ) {
                dst[0] = src[0];
            } else if ( sizeof( base_type ) == 2 ) {
                dst[0] = src[0];
                dst[1] = src[1];
            } else if ( sizeof( base_type ) == 4 ) {
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                dst[3] = src[3];
            } else {

                for (
                     int i = sizeof( base_type );
                     i > 0;
                     i --
                     ) {
                    * ( dst ++ ) = * ( src ++ );
                }
            }

        } else {

            if ( sizeof( base_type ) == 2 ) {
                dst[1] = src[0];
                dst[0] = src[1];
            } else if ( sizeof( base_type ) == 4 ) {
                dst[3] = src[0];
                dst[2] = src[1];
                dst[1] = src[2];
                dst[0] = src[3];
            } else {
                dst += sizeof( base_type ) -1;
                for ( int i = sizeof( base_type ); i > 0; i -- ) {
                    * ( dst -- ) = * ( src ++ );
                }
            }
        }

        return i_destination;
    }

    template <class base_type>
    base_type OrderWrite(const base_type & i_val)
    {
        // for the time being this is the same as OrderRead
        return OrderRead( i_val );
    }

    unsigned int inet_addr(const QString &addr);
    QString inet_ntoa(unsigned int in);
}

#endif
