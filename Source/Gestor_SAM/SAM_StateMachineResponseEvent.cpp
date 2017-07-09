#include <QMetaEnum>

#include "SAM_StateMachineResponseEvent.h"

namespace SAM
{
    const char * ResponseEvent::getEventTypeName(EventType tipoEstado)
    {
        const QMetaObject &mo = staticMetaObject;
        int index = mo.indexOfEnumerator("EventType");
        QMetaEnum metaEnum = staticMetaObject.enumerator(index);
        const char * szTipoEstado = metaEnum.valueToKey(tipoEstado);

        return szTipoEstado;
    }

}
