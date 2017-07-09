
#include <QStateMachine>

#include "SAM_MessageProcessor.h"
#include "SAM_DataBase.h"
#include "SAM_Log.h"
#include "SAM_MeasuresCount.h"
#include "SAM_StateMachine.h"
#include "SAM_StateMachinePrivate.h"

namespace SAM
{

    void StateMachine::stateTimeout(StateType tipoEstado)
    {
        TRACE(STATE);

        const char * szTipoEstado = StateMachine::getStateTypeName(tipoEstado);

        LOG_ALERT(STATE) << "Ha expirado el temporizador del estado" << szTipoEstado;
    }

    void StateMachine::stateEntered(StateType tipoEstado)
    {
        TRACE(STATE);

        Q_D(StateMachine);

        short lng = 0;
        MSG_TH msg;

        SAM_DataBase & estadoSAM = samDataBase();

        MeasuresState & estado = estadoSAM.estado;

        // Recupera la tabla que vincula IPs con IDs de equipo.
        JVariant T_EQIP = appSettings()["CONFIG"]["HW"]["T_EQIP"];

        Battery * pBattery = estadoSAM.getBattery();

        if(!pBattery) {
            LOG_ERROR_LN(STATE) << "No existe una batería seleccionada.";
            return;
        }

        MeasuresCount & requestedMeasures = getRequestedMeasures();
        unsigned int sumRequestedMeasures = requestedMeasures.sum();

        const char * szTipoEstado = StateMachine::getStateTypeName(tipoEstado);

        LOG_INFO(STATE) << "Se ha entrado en el estado" >> szTipoEstado;

        // Comprueba si se había solicitado la cancelación de la ejecución de la batería.
        if(!d->m_strMotivoCancelacion.isEmpty())
        {
            if(tipoEstado == eReposo)
            {
                /**
                 *  5.2.2.1.2 Cancelación de una Batería de Pruebas en Ejecución
                 *
                 *  Al finalizar la cancelación de la Ejecución de una Batería de Pruebas,
                 *  el servidor SAM debe informar al cliente SAM.
                 */
                // Se debe notificar al TMCS la causa de la cancelación
                lng = sizeof(CAB_MSG_TH) + sizeof(MSG_BATERIA_POR_ID);

                // Crea el mensaje directamente en BigEndian
                MSG_TH msgME;
                msgME.cab.lng = lng;
                msgME.cab.idMsg = M_CANCELA_BATERIA_SAM;
                msgME.cab.portDestino = 0;
                messageProcessor().initResponseMessageBE(msg, msgME);

                msg.mCancelaBateria.idSesion = estado.idSesion;
                msg.mCancelaBateria.idBateria = estado.idBateria;

                MessageProcessor::sendToRemoteSupervisionServers(msg);

                LOG_INFO(STATE) << "Cancelada la batería de pruebas" << estado.idBateria
                                << "de la sesión" << estado.idSesion << "por"
                                >> d->m_strMotivoCancelacion;

                d->m_strMotivoCancelacion.clear();
            }
            else
            {
                // Envía el mensaje de salir del estado actual.
                d->m_machine->postEvent(new ResponseEvent(ResponseEvent::Salir));
            }
        }

        switch(tipoEstado)
        {
        case eDesconocido:
            break;
        case eReposo:
            estado.clear();
            break;
        case eSolicitaPatronOn:
        case eSolicitaPatronOff:
        {
            //JVariant T_SUPER = appSettings()["CONFIG"]["HW"]["T_SUPER"][1];
            //QString ipTMCS = T_SUPER["IFZ_VOIP1"]; // Obtener la IP del TMCS

            // Envía el mensaje de activación/desactivación de la posición patrón
            lng = sizeof(CAB_MSG_TH) + sizeof(MSG_POSICION_SAM);

            // Crea el mensaje directamente en BigEndian
            MSG_TH msgME;
            msgME.cab.lng = lng;
            msgME.cab.idMsg = M_ACTIVACION_PATRON;
            msgME.cab.portDestino = 0;
            messageProcessor().initResponseMessageBE(msg, msgME);

            QString ipPatron = estado.ipPatron;
            //uint32 idPatron = T_EQIP[ipPatron]["ID"];

            //msg.cab.idDestino.id = idPatron;
            //msg.cab.idDestino.tipoEq = EQ_PUESTO;
            //msg.cab.dirDestino.s_addr = inet_addr(ipTMCS); // Se envía al TMCS

            msg.mActivacionPatron.puesto.s_addr = inet_addr(ipPatron);
            msg.mActivacionPatron.abonado = PATRON;

            if(tipoEstado == StateMachine::eSolicitaPatronOff) {
                LOG_INFO(STATE) << "No quedan equipos por medir.";

                // Limpia los estados que ya no se van a usar
                estado.clear();
                // ...
                msg.mActivacionPatron.activarSam = ntoh(OFF);
            } else {
                msg.mActivacionPatron.activarSam = ntoh(ON);
                requestedMeasures.clear(); // Inicia el contador de avance
            }

            // Espera al evento ResponseEvent::EstadoPatronOn/Off
            MessageProcessor::sendToRemoteSupervisionServers(msg);

            break;
        }
        case eSolicitaMedicionEquipoOff:
        case eSolicitaMedicionEquipoOn:
        {
            MeasureDevice measureDevice;

            if(!estadoSAM.currentBatteryMeasureDevice(&measureDevice))
            {
                // No quedan más equipos que medir.
                // Se sale del bucle de mediciones de puestos.
                stateMachineInternal().postEvent(new ResponseEvent(ResponseEvent::Salir));
            }
            else
            {
                JVariant T_SUPER = appSettings()["CONFIG"]["HW"]["T_SUPER"][1];
                QString ipTMCS = T_SUPER["IFZ_VOIP1"]; // Obtener la IP del TMCS

                QString ipEquipo = measureDevice.equipment()->ip;

                // Recuperar el ID actual del equipo (no el que tenía cuando se creó la batería).
                uint32 idEquipo = T_EQIP[ipEquipo]["ID"];

                if(idEquipo == 0) {
                    LOG_WARN(STATE) << "El equipo" >> ipEquipo << "no cuenta con un ID "
                                    << "en la configuración actual ( ID_Config ="
                                    << pBattery->idConfig << '.' << pBattery->subIdConfig << ").";

                    //stateMachine().postEvent(new ResponseEvent(ResponseEvent::Salir));
                    //break;
                }

                // Crea el mensaje en BigEndian
                short lng = sizeof(CAB_MSG_TH) + sizeof(MSG_POSICION_SAM);

                MSG_TH msgME;
                msgME.cab.lng = lng;
                msgME.cab.idMsg = M_POSICION_MEDICION_SAM;
                msgME.cab.portDestino = 0;
                messageProcessor().initResponseMessageBE(msg, msgME);

                msg.cab.idDestino.id = hton(idEquipo);
                msg.cab.idDestino.tipoEq = EQ_PUESTO;
                msg.cab.dirDestino.s_addr = inet_addr(ipTMCS); // Se envía al TMCS
                msg.cab.portDestino = hton((unsigned short)7002);

                msg.mPosicionMedicion.puesto.s_addr = inet_addr(ipEquipo);
                msg.mPosicionMedicion.abonado = MEDIDAS;

                if(tipoEstado == StateMachine::eSolicitaMedicionEquipoOff) {
                    estadoSAM.nextBatteryMeasureEquipment();
                    msg.mPosicionMedicion.activarSam = hton(OFF);
                } else {
                    if(tipoEstado == StateMachine::eSolicitaMedicionEquipoOn) {
                        requestedMeasures.nPositions++;
                    }
                    msg.mPosicionMedicion.activarSam = hton(ON);
                }

                // Espera al evento ResponseEvent::EstadoMedicionPuestoOn
                MessageProcessor::sendToRemoteSupervisionServers(msg);
            }
            break;
        }
        case eSolicitaMedicionPatronOff:
        case eSolicitaMedicionPatronOn:
        case eSolicitaMedicionPatronOn1intento:
        case eSolicitaMedicionPatronOn2intento:
        {
            // Envía el mensaje de activación/desactivación de medición de la posición patrón
            lng = sizeof(CAB_MSG_TH) + sizeof(MSG_SET_LLAMADA_SAM);

            // Crea el mensaje directamente en BigEndian
            MSG_TH msgME;
            msgME.cab.lng = lng;
            msgME.cab.idMsg = M_SET_LLAMADA_SAM;
            msgME.cab.portDestino = 0;
            messageProcessor().initResponseMessageBE(msg, msgME);

            QString ipPatron = estado.ipPatron;
            QString ipEquipo = estado.ipEquipo;
            uint32 idPatron = T_EQIP[ipPatron]["ID"];
            //uint32 idEquipo = T_EQIP[ipEquipo]["ID"];

            if(idPatron == 0) {
                LOG_WARN(STATE) << "El equipo patrón" >> ipPatron << "no cuenta con un ID en "
                                << "la configuración actual ( ID_Config =" << pBattery->idConfig
                                << '.' << pBattery->subIdConfig << ").";

                //stateMachine().postEvent(new ResponseEvent(ResponseEvent::Salir));
                //break;
            }

            msg.cab.idDestino.id = idPatron;
            msg.cab.idDestino.tipoEq = EQ_PUESTO;
            msg.cab.dirDestino.s_addr = inet_addr(ipPatron);

            msg.mSetLlamadaSAM.destino.s_addr = inet_addr(ipEquipo);

            if(tipoEstado == StateMachine::eSolicitaMedicionPatronOff) {
                estadoSAM.nextBatteryMeasureEquipment();

                msg.mSetLlamadaSAM.estado = hton(OFF);
            } else {
                msg.mSetLlamadaSAM.estado = hton(ON);
            }

            // Espera al evento ResponseEvent::EstadoMedicionPatronOn
            MessageProcessor::sendToRemoteSupervisionServers(msg);

            break;
        }
        case eSolicitaBucleOff:
        case eSolicitaBucleOn:
        case eSolicitaBucleOn1intento:
        case eSolicitaBucleOn2intento:
        {
            MeasureDevice measureDevice;

            if(!estadoSAM.currentBatteryMeasureDevice(&measureDevice))
            {
                // No quedan más bucles que medir en éste equipo.
                // Se sale del bucle de mediciones de bucles de puestos.
                stateMachineInternal().postEvent(new ResponseEvent(ResponseEvent::Salir));

                break;
            }

            QString ipEquipo = estado.ipEquipo;

            // Recuperar el ID actual del equipo
            // (no el que tenía cuando se creó la batería: equipo["ID"]).
            uint32 idEquipo = T_EQIP[ipEquipo]["ID"];

            if(idEquipo == 0) {
                LOG_WARN(STATE) << "El equipo" >> ipEquipo << "no cuenta con un ID en la "
                                << "configuración actual ( ID_Config=" << pBattery->idConfig
                                << '.' << pBattery->subIdConfig << ").";

                //stateMachine().postEvent(new ResponseEvent(ResponseEvent::Salir));
                //break;
            }

            JVariant HW = appSettings()["CONFIG"]["HW"];
            JVariant PUES = HW["T_PUES"][idEquipo];

            /**
             *  5.2.2.9 Bucles de la UGA
             *
             * El servidor SAM no debe mandar a una posición que no pertenezca a una UCS,
             * el mensaje M_SET_BUCLE con el campo `NBUCLE` con valor `CASCOS_2` o `MICRO_2`.
             */
            EnumHolder<E_ID_CANAL_BUCLE> tipoCanalBucle = estadoSAM.estado.tipoCanalBucle;

            bool isUCS = PUES.contains("UCS_EJ") || PUES.contains("UCS_PL");

            if(!isUCS && (tipoCanalBucle == CASCOS_2 || tipoCanalBucle == MICRO_2))
            {
                estadoSAM.nextBatteryMeasure();

                LOG_ALERT(STATE) << "Se evita la medición de" >> tipoCanalBucle.toString()
                                 << "en la posición IP=" << ipEquipo << ", ID=" << idEquipo
                                 << "ya que pertenece a una UCS: " << PUES.toJson();

                estadoSAM.nextBatteryMeasureEquipment();

                break;
            }

            // Crea el mensaje en BigEndian
            short lng = sizeof(CAB_MSG_TH) + sizeof(MSG_SET_BUCLE_SAM);

            MSG_TH msgME;
            msgME.cab.lng = lng;
            msgME.cab.idMsg = M_SET_BUCLE_SAM;
            msgME.cab.portDestino = 0;
            messageProcessor().initResponseMessageBE(msg, msgME);

            msg.cab.idDestino.id = hton(idEquipo);
            msg.cab.idDestino.tipoEq = EQ_PUESTO;
            msg.cab.dirDestino.s_addr = inet_addr(ipEquipo);
            msg.cab.portDestino = hton((unsigned short)7002);

            if(tipoEstado == StateMachine::eSolicitaBucleOff) {
                msg.mSetBucle.estado = hton(OFF);
            } else {
                if(tipoEstado == StateMachine::eSolicitaBucleOn) {
                    requestedMeasures.nLoops++;
                }
                msg.mSetBucle.estado = hton(ON);
            }

            msg.mSetBucle.nbucle = ntoh(tipoCanalBucle);

            // Espera al evento ResponseEvent::EstadoBucleOn/Off
            MessageProcessor::sendToRemoteSupervisionServers(msg);
        }
            break;
        case eSolicitaMedidaOn:
            /// @todo SAGE
            if(tipoEstado == StateMachine::eSolicitaMedidaOn) {
                requestedMeasures.nMeasures++;
            }

            break;
        case eSolicitaInformeCanal:
            /// @todo SAGE
            break;
        case eSiguienteMedida:
            /// @todo SAGE
            break;
        case eSiguienteBucle:
        {
            estadoSAM.nextBatteryMeasureChannelLoop();
            break;
        }
        case eSiguienteEquipo:
        {
            estadoSAM.nextBatteryMeasureEquipment();

            break;
        }
        default:
            ;
        }

        // Determinar si ha variado el avance en las mediciones
        if(requestedMeasures.sum() != sumRequestedMeasures) {
            /**
             *  6.1.7 EvtSAM_PorcentajeAvanceMedida
             *  El servidor SAM envía un mensaje con el porcentage de avance en la
             *  realización de una batería de pruebas o medidas de red.
             */
            // Si ha habido algún avance en la ejecución de la batería se envían al TMCS
            messageProcessor().sendMeasureAdvance();
        }
    }

}

