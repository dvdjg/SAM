#ifndef SAM_SIGNALHANDLER_H
#define SAM_SIGNALHANDLER_H

#include <QObject>

class QSocketNotifier;

namespace SAM
{
    /**
     * @brief Clase auxiliar para procesar las señales de terminación del Sistema Operativo.
     */
    class SignalHandler : public QObject
    {
        Q_OBJECT
        Q_ENUMS(SIGNALS)
    public:
        SignalHandler(int mask = DEFAULT_SIGNALS, QObject * parent = 0);
        ~SignalHandler();

        enum SIGNALS
        {
            SIG_UNHANDLED   = 0,    ///< Physical signal not supported by this class
            SIG_NOOP        = 1,    ///< The application is requested to do a no-op (only a target that platform-specific signals map to when they can't be raised anyway)
            SIG_INT         = 2,    ///< Control+C (should terminate but consider that it's a normal way to do so; can delay a bit)
            SIG_TERM        = 4,    ///< Control+Break (should terminate now without regarding the consquences)
            SIG_CLOSE       = 8,    ///< Container window closed (should perform normal termination, like Ctrl^C) [Windows only; on Linux it maps to SIG_TERM]
            SIG_RELOAD      = 16,   ///< Reload the configuration [Linux only, physical signal is SIGHUP; on Windows it maps to SIG_NOOP]
            DEFAULT_SIGNALS = SIG_INT | SIG_TERM | SIG_CLOSE
        };

        static const char * enumName(SIGNALS signal);

        static const int numSignals = 6;

    public slots:
        void emitHandleSignal(int signal);

    signals:
        void handleSignal(int signal);

    private:
        int m_mask;

#ifndef _WIN32
    public:
        static int s_signalFd[2];
        QSocketNotifier *m_signal;
#endif
    };

}

#endif // SAM_SIGNALHANDLER_H
