#include "SAM_Log.h"

#ifdef Q_OS_WIN
#  include <winsock2.h>
#  include <Mstcpip.h> /* struct tcp_keepalive */
#endif
#if defined (Q_OS_UNIX)
#  include <sys/types.h>
#  include <sys/socket.h>
//#  include <netinet/in.h>
#endif

#include "keepSocketAlive.h"

#ifdef Q_OS_WIN
bool keepSocketAlive(int fd, int enableKeepAlive, int maxIdle, int interval, int)
{
    TRACE(COMS);
    int r;
    DWORD dw;
    struct tcp_keepalive ka;

    /* enable or disable (same as SO_KEEPALIVE) */
    ka.onoff = enableKeepAlive;

    /* milliseconds (same as TCP_KEEPIDLE) */
    ka.keepalivetime = maxIdle * 1000;

    /* milliseconds (same as TCP_KEEPINTVL) */
    ka.keepaliveinterval = interval * 1000;

    /* configure keep-alives for 'fd' */
    r = WSAIoctl(fd, SIO_KEEPALIVE_VALS,
       &ka, sizeof(ka), NULL, 0, &dw, NULL, NULL);

    if( r == SOCKET_ERROR )
    {
        // Uh oh!  Something bad happened.  Let's
        // get the error code...
        int errCode = WSAGetLastError();

        // ..and the human readable error string!!
        // Interesting:  Also retrievable by net helpmsg 10060
        LPTSTR errString = NULL;  // will be allocated and filled by FormatMessage

        int size = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                  FORMAT_MESSAGE_FROM_SYSTEM, // use windows internal message table
                                  0,       // 0 since source is internal message table
                                  errCode, // this is the error code returned by WSAGetLastError()
                                  // Could just as well have been an error code from generic
                                  // Windows errors from GetLastError()
                                  0,       // auto-determine language to use
                                  (LPTSTR)&errString, // this is WHERE we want FormatMessage
                                  // to plunk the error string.  Note the
                                  // peculiar pass format:  Even though
                                  // errString is already a pointer, we
                                  // pass &errString (which is really type LPSTR* now)
                                  // and then CAST IT to (LPSTR).  This is a really weird
                                  // trip up.. but its how they do it on msdn:
                                  // http://msdn.microsoft.com/en-us/library/ms679351(VS.85).aspx
                                  0,                 // min size for buffer
                                  0 );               // 0, since getting message from system tables
        //printf( "Error code %d:  %s\n\nMessage was %d bytes, in case you cared to know this.\n\n", errCode, errString, size ) ;
        if(!size) {
            errString = TEXT("No se puede formatear el mensaje de error de socket.");
        }

        LOG_WARN(COMS) << "No se pueden configurar los mensajes de keep-alive para el socket"
                       << fd << ":" << errString;
        LocalFree( errString ) ; // if you don't do this, you will get an
        // ever so slight memory leak, since we asked
        // FormatMessage to FORMAT_MESSAGE_ALLOCATE_BUFFER,
        // and it does so using LocalAlloc
        // Gotcha!  I guess.

        return false;
    }
    else if(enableKeepAlive)
    {
        LOG_DEBUG(COMS) << "Configurados los mensajes de keep-alive para el socket"
                        << fd << ": maxIdle =" << maxIdle << ", interval=" << interval;
    }

    return true;
}


#endif
#if defined (Q_OS_UNIX)

bool keepSocketAlive(int fd, int enableKeepAlive, int maxIdle, int interval, int)
{
    TRACE(COMS);

    int retOpt = ::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,
                              (const char*)&enableKeepAlive, sizeof(enableKeepAlive));
    int maxIdle = 5; // seconds
    ::setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, (const char*)&maxIdle, sizeof(maxIdle));

    int count = 3;  // send up to 3 keepalive packets out, then disconnect if no response
    ::setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (const char*)&count, sizeof(count));

    int interval = 2;   // send a keepalive packet out every 2 seconds (after the 5 second idle period)
    ::setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (const char*)&interval, sizeof(interval));

    return true;
}

#endif
