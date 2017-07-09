
#include <QStateMachine>

#include "SAM_Archive.h"
#include "SAM_Log.h"
#include "SAM_MessageProcessorPrivate.h"

namespace SAM
{
    MessageProcessorPrivate::MessageProcessorPrivate()
        : m_receivedMsgCount(0),
          m_processedMsgCount(0),
          m_nRejectedBytes(0),
          m_nReceivedBytes(0),
          m_sessionCounter(0),
          m_batteryCounter(0),
          m_sessionOpened(0),
          m_stateMachine(NULL),
          m_timerPresWatchdog(NULL),
          m_timerPresEstad(NULL),
          m_pWatchdogClient(NULL)
    {
    }

    E_MODO_SAM MessageProcessorPrivate::modoSAM() const
    {
        TRACE(COMS);

        // Buscar el modo de SAM en el nombre del estado seleccionado
        EnumHolder<E_MODO_SAM> modo = REPOSO;

        QSet<QAbstractState*> configuration = stateMachineInternal().configuration();
        foreach(const QAbstractState* pState, configuration)
        {
            const QAbstractState *pParent = pState;
            while((pParent = pParent->parentState()))
            {
                QString name = pParent->objectName();

                if(name == StateMachine::getStateTypeName(StateMachine::eAudio)) {
                    modo = MED_AUDIO;
                    break;
                } else if(name == StateMachine::getStateTypeName(StateMachine::eRed)) {
                    modo = MED_RED;
                    break;
                }
            }
        }

        return modo;
    }

}
