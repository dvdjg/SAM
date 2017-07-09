#ifndef SAM_STATEMACHINERESPONSETRANSITION_H
#define SAM_STATEMACHINERESPONSETRANSITION_H

#include <QAbstractTransition>

#include "SAM_StateMachineResponseEvent.h"

namespace SAM
{
    class StateMachine;

    class ResponseTransition : public QAbstractTransition
    {
        Q_OBJECT

        Q_PROPERTY(SAM::ResponseEvent::EventType eventType READ eventType WRITE setEventType )
        Q_PROPERTY(SAM::ResponseEvent::EventType preferredEventType READ preferredEventType WRITE setPreferredEventType )

    public:
        ResponseTransition(ResponseEvent::EventType tipoMensaje,
                QState *sourceState = 0,
                StateMachine *stateMachine = 0);

        void setEventType(ResponseEvent::EventType eventType);
        ResponseEvent::EventType eventType() const;

        void setPreferredEventType(ResponseEvent::EventType preferredEventType);
        ResponseEvent::EventType preferredEventType() const;

    protected:
         virtual bool eventTest(QEvent *e);

         virtual void onTransition(QEvent *e);

    protected:
        ResponseEvent::EventType m_eventType;
        ResponseEvent::EventType m_preferredEventType;
        StateMachine * m_stateMachine;
    };

}

#endif // SAM_STATEMACHINERESPONSETRANSITION_H
