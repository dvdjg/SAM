#ifndef SAM_STATEMACHINERESPONSEEVENT_H
#define SAM_STATEMACHINERESPONSEEVENT_H

#include <QEvent>
#include "SAM_JVariant.h"

namespace SAM
{
    class ResponseEvent : public QEvent
    {
        Q_GADGET

        Q_ENUMS(EventType)

    public:
        enum EventType
        {
            Nulo,
            Desconocido,
            SalirBateria, // Salir de la batería de pruebas
            Salir, // Sale del estado a otro de desactivación
            EjecutaBateria, ///< Incluye: idSesion, idBateria
            EstadoPatronOn, ///< Incluye: ipPatron
            EstadoPatronOff, ///< Incluye: ipPatron, motivo
            EstadoMedicionPuestoOn, ///< Incluye: ipEquipo
            EstadoMedicionPuestoOff, ///< Incluye: ipEquipo, motivo
            EstadoMedicionGatewayOn, ///< Incluye: ipEquipo
            EstadoMedicionGatewayOff, ///< Incluye: ipEquipo, motivo
            EstadoMedicionInterfazOn, ///< Incluye: ipEquipo
            EstadoMedicionInterfazOff, ///< Incluye: ipEquipo, motivo
            EstadoMedicionPatronOn, ///< Incluye: ipEquipo,ipPatron
            EstadoMedicionPatronOff, ///< Incluye: ipEquipo,ipPatron, motivo
            EstadoBucleOn, ///< Incluye: ipEquipo,tipoBucle
            EstadoBucleOff, ///< Incluye: ipEquipo,tipoBucle, motivo
            EstadoCanalOn, ///< Incluye: ipEquipo,tipoBucle,tipoCanal. En el SAGE
            EstadoCanalOff, ///< Incluye: ipEquipo,tipoBucle,tipoCanal, motivo. En el SAGE
            InformeCanal ///< Incluye: ipEquipo,tipoBucle,tipoCanal,valorMedida. En el SAGE: Se ha recibido el informe de la medición del canal de audio que se almacenará.
        };

        ResponseEvent(EventType tipo = Nulo)
            : QEvent(QEvent::Type(QEvent::User+1)),
              type(tipo) {}

        ResponseEvent(EventType tipo, const JVariant & value)
            : QEvent(QEvent::Type(QEvent::User+1)),
              type(tipo), data(value) {}

        static const char * getEventTypeName(EventType tipoEstado);

    public:

        EventType type;
        JVariant data;
    };
}

#endif // SAM_STATEMACHINERESPONSEEVENT_H
