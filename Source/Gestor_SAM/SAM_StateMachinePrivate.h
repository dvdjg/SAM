#ifndef SAM_STATEMACHINEPRIVATE_H
#define SAM_STATEMACHINEPRIVATE_H

#include "SAM_CommandObjectPrivate.h"
#include "SAM_MeasuresCount.h"
#include "SAM_DataBase.h"

class QSignalMapper;
class QStateMachine;

namespace SAM
{
    class MessageProcessor;
    class SAGE;

    class StateMachinePrivate : public CommandObjectPrivate
    {
        friend class StateMachine;

    protected:
        StateMachinePrivate(JVariant &settings, MessageProcessor * pMessageProcessor = NULL)
            : m_machine(0), m_settings(settings), m_sigMapper(0),
              m_pMessageProcessor(pMessageProcessor), m_pSAGE(NULL) {}

        ~StateMachinePrivate()
        {
        }

    protected:

        QStateMachine * m_machine;

        JVariant & m_settings; /**< Configuraciones particulares de ésta clase */

        QSignalMapper * m_sigMapper;

        SAM_DataBase m_samDB;

        MeasuresCount m_totalMeasures; ///< Cuenta internamente el número de medidas previstas en una batería.
        MeasuresCount m_requestedMeasures; ///< Lleva la cuenta de las medidas solicitadas en la batería.
        MeasuresCount m_confirmedMeasures; ///< Lleva la cuenta de las medidas confirmadas en la batería.

        MessageProcessor * m_pMessageProcessor; ///< Procesador de mensajes padre.

        /**
         *  Almacena el motivo por el que se cancela la ejecución de una batería
         *  Esta variable actúa como testigo de la petición de cancelación anticipada.
         *  Si está vacía es que no se ha solicitado la cancelación en esta ejecución.
         */
        QString m_strMotivoCancelacion;

        SAGE * m_pSAGE;
    };
}

#endif // SAM_STATEMACHINEPRIVATE_H
