
#include <assert.h>

#ifndef _WIN32

#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <QSocketNotifier>

#else

#include <windows.h>
#include <set>

#endif // !_WIN32

#include "SAM_QEnumToString.h"
#include "SAM_Log.h"
#include "SAM_SignalHandler.h"

namespace SAM
{
    // There can be only ONE SignalHandler per process
    static SignalHandler* g_handler(NULL);
#ifdef _WIN32

    BOOL WINAPI WIN32_handleFunc(DWORD);
    int WIN32_physicalToLogical(DWORD);
    DWORD WIN32_logicalToPhysical(int);
    std::set<int> g_registry;

#else //_WIN32
    int SignalHandler::s_signalFd[2];

    void POSIX_handleFunc(int);
    int POSIX_physicalToLogical(int);
    int POSIX_logicalToPhysical(int);

#endif //_WIN32

    SignalHandler::SignalHandler(int mask, QObject * parent) : QObject(parent), m_mask(mask)
    {
        assert(g_handler == NULL);
        g_handler = this;

#ifdef _WIN32
        SetConsoleCtrlHandler(WIN32_handleFunc, TRUE);
#else //_WIN32
        if (::socketpair(AF_UNIX, SOCK_STREAM, 0, s_signalFd))
            LOG_ERROR(SERV) <<  "No se puede iniciar el socketpair.";

        m_signal = new QSocketNotifier(s_signalFd[1], QSocketNotifier::Read, this);
        connect(m_signal, SIGNAL(activated(int)), this, SLOT(emitHandleSignal(int)));

        // Dar un comportamiento seguro (ignorar) a las señales Unix más habituales
        ::signal(SIGCHLD, SIG_IGN); /* ignore child */
        ::signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
        ::signal(SIGTTOU, SIG_IGN);
        ::signal(SIGTTIN, SIG_IGN);
        ::signal(SIGHUP,  SIG_IGN); /* ignore hangup signal */

#endif

        for (int i=0;i<numSignals;i++)
        {
            int logical = 0x1 << i;
            if (m_mask & logical)
            {
#ifdef _WIN32
                g_registry.insert(logical);
#else
                int sig = POSIX_logicalToPhysical(logical);
                if(sig != -1)
                    assert(::signal(sig, POSIX_handleFunc) != SIG_ERR);
#endif //_WIN32
            }
        }

    }

    SignalHandler::~SignalHandler()
    {
#ifdef _WIN32
        SetConsoleCtrlHandler(WIN32_handleFunc, FALSE);
#else
        for (int i=0;i<numSignals;i++)
        {
            int logical = 0x1 << i;
            if (m_mask & logical)
            {
                signal(POSIX_logicalToPhysical(logical), SIG_DFL);
            }
        }
#endif //_WIN32
    }

    void SignalHandler::emitHandleSignal(int signal)
    {
        LOG_INFO(LOCAL) <<  "Recibida la señal del SO #" << signal;

#ifndef _WIN32
        m_signal->setEnabled(false);
        int signo = 0;
        ::read(s_signalFd[1], &signo, sizeof(signo));

        // do Qt stuff
        emit handleSignal(signo);

        m_signal->setEnabled(true);
#else
        emit handleSignal(signal);
#endif
    }

    const char * SignalHandler::enumName(SIGNALS signal)
    {
        QEnumToString<SignalHandler> es("SIGNALS");
        const char * szSignal = es(signal);
        return szSignal;
    }

#ifdef _WIN32
    DWORD WIN32_logicalToPhysical(int signal)
    {
        switch (signal)
        {
        case SignalHandler::SIG_INT: return CTRL_C_EVENT;
        case SignalHandler::SIG_TERM: return CTRL_BREAK_EVENT;
        case SignalHandler::SIG_CLOSE: return CTRL_CLOSE_EVENT;
        default:
            return ~(unsigned int)0; // SIG_ERR = -1
        }
    }
#else
    int POSIX_logicalToPhysical(int signal)
    {
        switch (signal)
        {
        case SignalHandler::SIG_INT: return SIGINT;
        case SignalHandler::SIG_TERM: return SIGTERM;
            // In case the client asks for a SIG_CLOSE handler, accept and
            // bind it to a SIGTERM. Anyway the signal will never be raised
        case SignalHandler::SIG_CLOSE: return SIGTERM;
        case SignalHandler::SIG_RELOAD: return SIGHUP;
        default:
            return -1; // SIG_ERR = -1
        }
    }
#endif //_WIN32


#ifdef _WIN32
    int WIN32_physicalToLogical(DWORD signal)
    {
        switch (signal)
        {
        case CTRL_C_EVENT: return SignalHandler::SIG_INT;
        case CTRL_BREAK_EVENT: return SignalHandler::SIG_TERM;
        case CTRL_CLOSE_EVENT: return SignalHandler::SIG_CLOSE;
        default:
            return SignalHandler::SIG_UNHANDLED;
        }
    }
#else
    int POSIX_physicalToLogical(int signal)
    {
        switch (signal)
        {
        case SIGINT: return SignalHandler::SIG_INT;
        case SIGTERM: return SignalHandler::SIG_TERM;
        case SIGHUP: return SignalHandler::SIG_RELOAD;
        default:
            return SignalHandler::SIG_UNHANDLED;
        }
    }
#endif //_WIN32



#ifdef _WIN32
    BOOL WINAPI WIN32_handleFunc(DWORD signal)
    {
        if (g_handler)
        {
            int signo = WIN32_physicalToLogical(signal);
            // The std::set is thread-safe in const reading access and we never
            // write to it after the program has started so we don't need to
            // protect this search by a mutex
            std::set<int>::const_iterator found = g_registry.find(signo);
            if (signo != -1 && found != g_registry.end())
            {
                g_handler->emitHandleSignal(signo);
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
#else
    void POSIX_handleFunc(int signal)
    {
        if (g_handler)
        {
            int signo = POSIX_physicalToLogical(signal);

            ::write(SignalHandler::s_signalFd[0], &signo, sizeof(signo));
        }
    }
#endif //_WIN32


}

