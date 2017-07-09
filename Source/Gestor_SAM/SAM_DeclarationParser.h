/// @file SAM_DeclarationParser.h
/// @note Vendor: Microsoft
/// @date
//

#ifndef SAM_DeclarationParser_h__
#define SAM_DeclarationParser_h__

#include "SAM_Declaration.h"


#ifndef member_size
#define member_size(type, member) sizeof(((type *)0)->member)
#endif

/**
* @brief Declaraciones de funciones y plantillas para facilitar el acceso a las estructuras y tipos enumerados usados en la comunicación UDP del SAM.
*/
namespace SAM_DeclarationParser
{
    using namespace SAM_Declaration;
    /// Intermediario para serializar E_TIP_MSG enum
    template<typename Archive>
    void serialize(Archive & ar, E_TIP_MSG & t)
    {

        typedef char (*toarray)[4];
        ar & *(toarray) &t;
    }

    /// Intermediario para serializar E_ID_MSG enum
    template<typename Archive>
    void serialize(Archive & ar, E_ID_MSG & t)
    {

        typedef char (*toarray)[4];
        ar & *(toarray) &t;
    }

    /// Intermediario para serializar E_SUBID_MSG enum
    template<typename Archive>
    void serialize(Archive & ar, E_SUBID_MSG & t)
    {

        typedef char (*toarray)[4];
        ar & *(toarray) &t;
    }

    /// Intermediario para serializar E_TIPO_EQUIPO enum
    template<typename Archive>
    void serialize(Archive & ar, E_TIPO_EQUIPO & t)
    {

        typedef char (*toarray)[4];
        ar & *(toarray) &t;
    }

    /// Intermediario para serializar E_SISTEMA_ABC enum
    template<typename Archive>
    void serialize(Archive & ar, E_SISTEMA_ABC & t)
    {

        typedef char (*toarray)[1];
        ar & *(toarray) &t;
    }

    /// Intermediario para serializar MSG_EST_CONFIG struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_EST_CONFIG & t)
    {
        { ar & AR_SERIALIZATION_M(t, idConfig);  }
        { ar & AR_SERIALIZATION_M(t, subIdConfig);  }
        { ar & AR_SERIALIZATION_M(t, nombreConfig);  } ///< Nombre de la configuración
        { ar & AR_SERIALIZATION_M(t, version);  } ///< Versión de la configuración
        { ar & AR_SERIALIZATION_M(t, estado);  }
    }

    /// Intermediario para serializar IDENT_EQUIPO struct
    template<typename Archive>
    void serialize(Archive & ar, IDENT_EQUIPO & t)
    {
        { ar & AR_SERIALIZATION_M(t, tipoEq);  }
        { ar & AR_SERIALIZATION_M(t, id);  }
    }

    /// Intermediario para serializar CAB_MSG_TH struct
    template<typename Archive>
    void serialize(Archive & ar, CAB_MSG_TH & t)
    {
        { ar & AR_SERIALIZATION_M(t, lng);  } ///< Longitud mensaje incluyendo este campo. (2 bytes)
        { ar & AR_SERIALIZATION_M(t, tipoMsg);  } ///< Peticion, respuesta, notificacion (4 bytes)
        { ar & AR_SERIALIZATION_M(t, idMsg);  } ///< Identificacion de mensaje (4 bytes)
        { ar & AR_SERIALIZATION_M(t, dirOrigen);  } ///< Direccion IP del servidor SAM (4 bytes)
        { ar & AR_SERIALIZATION_M(t, dirDestino);  } ///< Direccion de Destino (4 bytes)
        { ar & AR_SERIALIZATION_M(t, idOrigen);  } ///< Identificador equipo origen
        { ar & AR_SERIALIZATION_M(t, idDestino);  } ///< Identificador equipo destino
        { ar & AR_SERIALIZATION_M(t, portDestino);  } ///< Puerto destino (2 bytes)
        { ar & AR_SERIALIZATION_M(t, numMensaje);  } ///< Identifica a cada mensaje(este numero es distinto para cada mensaje) (4 bytes)
    }

    /// Intermediario para serializar SUP_PUES struct
    template<typename Archive>
    void serialize(Archive & ar, SUP_PUES & t)
    {
        { ar & AR_SERIALIZATION_M(t, idAbonado);  } ///< Id del abonado
        { ar & AR_SERIALIZATION_M(t, nomAbonado);  } ///< Nombre del abonado
        { ar & AR_SERIALIZATION_M(t, estJacksCont);  } ///< Estado jacks controlador (SI/NO) ///< [Toma los valores desde E_SINO]
        { ar & AR_SERIALIZATION_M(t, estJacksCoord);  } ///< Estado jacks coordinador (SI/NO) ///< [Toma los valores desde E_SINO]
        { ar & AR_SERIALIZATION_M(t, estPttCont);  } ///< Estado PTT controlador (SI/NO) ///< [Toma los valores desde E_SINO]
        { ar & AR_SERIALIZATION_M(t, estPttCoord);  } ///< Estado PTT coordinador (SI/NO) ///< [Toma los valores desde E_SINO]
        { ar & AR_SERIALIZATION_M(t, servRad);  } ///< Servicio radio (SI/NO) ///< [Toma los valores desde E_SINO]
        { ar & AR_SERIALIZATION_M(t, servTlf);  } ///< Servicio telefonía (SI/NO) ///< [Toma los valores desde E_SINO]
        { ar & AR_SERIALIZATION_M(t, servLC);  } ///< Servicio linea caliente (SI/NO) ///< [Toma los valores desde E_SINO]
        { ar & AR_SERIALIZATION_M(t, sectorizado);  } ///< Indica si el puesto esta sectorizado (SI/NO) ///< [Toma los valores desde E_SINO]
    }

    /// Intermediario para serializar ESTADO_HW_GENERAL struct
    template<typename Archive>
    void serialize(Archive & ar, ESTADO_HW_GENERAL & t)
    {
        { ar & AR_SERIALIZATION_M(t, tipoGW_68);  } ///< Tipo procesador 68000 (A = 0 o B = 1)
        { ar & AR_SERIALIZATION_M(t, numProc);  } ///< Numero de procesador del 68000
        { ar & AR_SERIALIZATION_M(t, estadoGW_68);  } ///< Estado del procesador 68000
        { ar & AR_SERIALIZATION_M(t, estadoGW_68Dual);  } ///< Estado del procesador 68000 dual
    }

    /// Intermediario para serializar ESTADO_HW_ENLACE struct
    template<typename Archive>
    void serialize(Archive & ar, ESTADO_HW_ENLACE & t)
    {
        { ar & AR_SERIALIZATION_M(t, tipo);  } ///< Tipo de enlace ///< [Toma los valores desde E_TIPO_ENLACE]
        { ar & AR_SERIALIZATION_M(t, estado);  } ///< Estado del enlace ///< [Toma los valores desde E_ESTADO_ENLACE]
    }

    /// Intermediario para serializar ESTADO_SUPERVISION_ENLACE struct
    template<typename Archive>
    void serialize(Archive & ar, ESTADO_SUPERVISION_ENLACE & t)
    {
        { ar & AR_SERIALIZATION_M(t, id);  } ///< Id de enlace
        { ar & AR_SERIALIZATION_M(t, estado);  } ///< Estado del enlace
    }

    /// Intermediario para serializar SUP_GW struct
    template<typename Archive>
    void serialize(Archive & ar, SUP_GW & t)
    {
        { ar & AR_SERIALIZATION_M(t, ppal_rsva);  } ///< Estado principal / reserva
        { ar & AR_SERIALIZATION_M(t, proc);  } ///< Estado procesador (Ver + abajo su descripción)
        { ar & AR_SERIALIZATION_M(t, alarm);  } ///< Estado alarmas procesador (Ver + abajo su descripción)
        { ar & AR_SERIALIZATION_M(t, estHw);  }
        { ar & AR_SERIALIZATION_M(t, enlace);  } ///< Estado de enlaces del GW (Solo para presencias GW)
    }

    /// Intermediario para serializar SUP_TMCS struct
    template<typename Archive>
    void serialize(Archive & ar, SUP_TMCS & t)
    {
        { ar & AR_SERIALIZATION_M(t, idConfig);  } ///< ID_CONFIG (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, subIdConfig);  } ///< SUB_ID_CONFIG (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, idEmerge);  } ///< ID_EMERGENCY (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, idSectorisation);  } ///< ID_SECTORISATION (2 Bytes)
    }

    /// Intermediario para serializar MSG_CONFIG_IMPLANTADA struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_CONFIG_IMPLANTADA & t)
    {
        { ar & AR_SERIALIZATION_M(t, numSesion);  } ///< Numero de sesion
        { ar & AR_SERIALIZATION_M(t, nomConfig);  } ///< Nombre de la configuración
        { ar & AR_SERIALIZATION_M(t, id);  } ///< id de la configuración
        { ar & AR_SERIALIZATION_M(t, subId);  } ///< subid de la configuración
        { ar & AR_SERIALIZATION_M(t, actual);  } ///< (1) indica configuración actual, (0) indica configuración de emergencia
    }

    /// Intermediario para serializar SUP_FICH_CONF struct
    template<typename Archive>
    void serialize(Archive & ar, SUP_FICH_CONF & t)
    {
        { ar & AR_SERIALIZATION_M(t, id);  } ///< identificador de la configuración
        { ar & AR_SERIALIZATION_M(t, subId);  } ///< identificador auxiliar de la configuración
        { ar & AR_SERIALIZATION_M(t, CRC);  } ///< CRC del fichero de configuración descomprimido
    }

    /// Intermediario para serializar INF_CRC_FICHEROS struct
    template<typename Archive>
    void serialize(Archive & ar, INF_CRC_FICHEROS & t)
    {
        { ar & AR_SERIALIZATION_M(t, supFichConfHw);  } ///< Información de ID, subID y CRC del fichero de config
        { ar & AR_SERIALIZATION_M(t, supFichConfLog);  } ///< Información de ID, subID y CRC del fichero de config
        { ar & AR_SERIALIZATION_M(t, supFichConfLogSectA);  } ///< Información de ID, subID y CRC del fichero de config. LogSectA
        { ar & AR_SERIALIZATION_M(t, supFichConfLogSectB);  } ///< Información de ID, subID y CRC del fichero de config. LogSectB
    }

    /// Intermediario para serializar MSG_SUPER struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_SUPER & t)
    {
        MSG_TH * pMSG_TH = (MSG_TH *)(((char*) &t) - offsetof(MSG_TH, super));

        { ar & AR_SERIALIZATION_M(t, id);  } ///< Id de puesto, gateway o enlace
        { ar & AR_SERIALIZATION_M(t, nomPuesto);  } ///< Nombre del puesto
        { ar & AR_SERIALIZATION_M(t, estado);  } ///< Estado TMCS A(Activo) R(Reserva) C(Cliente) 00h(Activo)
        if( pMSG_TH->cab.idOrigen.tipoEq == EQ_GATEWAY )
        { ar & AR_SERIALIZATION_M(t, gw);
            uint8 _dummy____[member_size(MSG_SUPER, enlace)-member_size(MSG_SUPER, gw)];
            ar & AR_SERIALIZATION(_dummy____);
        } ///< Parte de supervision especifica gateway
        ///< CAB_MSG_TH::idOrigen.tipoEq
        else if( pMSG_TH->cab.idOrigen.tipoEq == EQ_NULO )
        { ar & AR_SERIALIZATION_M(t, enlace);  } ///< Parte de supervision especifica puestos
        else if( pMSG_TH->cab.idOrigen.tipoEq == EQ_SUPERV /*&& !isWindows()*/ )
        { ar & AR_SERIALIZATION_M(t, tmcs);
            uint8 _dummy____[member_size(MSG_SUPER, enlace)-member_size(MSG_SUPER, tmcs)];
            ar & AR_SERIALIZATION(_dummy____);
        } ///< Parte de supervision especifica del TMCS !isWindows()
        else if( pMSG_TH->cab.idOrigen.tipoEq == EQ_SUPERR /*&& !isWindows()*/ )
        { ar & AR_SERIALIZATION_M(t, tmcsr);
            uint8 _dummy____[member_size(MSG_SUPER, enlace)-member_size(MSG_SUPER, tmcsr)];
            ar & AR_SERIALIZATION(_dummy____);
        } ///< Parte de supervision especifica del TMCS -- !isWindows()
        else
        { ar & AR_SERIALIZATION_M(t, pues);
            uint8 _dummy____[member_size(MSG_SUPER, enlace)-member_size(MSG_SUPER, pues)];
            ar & AR_SERIALIZATION(_dummy____);
        } ///< Parte de supervision especifica puestos
        { ar & AR_SERIALIZATION_M(t, sistRd);  } ///< Valores: SISTEMA_A = 'A', SISTEMA_B = 'B', SISTEMA_AYB = 'C'
        { ar & AR_SERIALIZATION_M(t, sistTf);  } ///< Valores: SISTEMA_A = 'A', SISTEMA_B = 'B', SISTEMA_AYB = 'C'
        { ar & AR_SERIALIZATION_M(t, mantenimiento);  } ///< Estado puesto en mantenimiento - 1 (Si), 0 (No) ///< [Toma los valores desde E_SINO]
        { ar & AR_SERIALIZATION_M(t, estadoConfigurado);  } ///< Indica que tipo de TMCS es. Puede ser ‘S’ (para servidor, sea principal o reserva) o ‘C’ (para cliente).
        { ar & AR_SERIALIZATION_M(t, estadoSectorizado);  } ///< Indica si el gw ha recibido la sectoriz o no - 1(si) 0(no) ///< [Toma los valores desde E_SINO]
        { ar & AR_SERIALIZATION_M(t, estadoSelFreqEmer);  } ///< Est de seleccion de frecuencias de emergencia - 1(si) 0(no) ///< [Toma los valores desde E_SINO]
        { ar & AR_SERIALIZATION_M(t, idGwSimulado);  } ///< 0 (No gwsimu) Los puestos y los gateways dicen si hace de gwSimulados y el id del gwSimulado
        { ar & AR_SERIALIZATION_M(t, infCrcEmer);  }
        { ar & AR_SERIALIZATION_M(t, infCrcAct);  }
        { ar & AR_SERIALIZATION_M(t, estEthernet0);  } ///< 0 Estado de la boca de LAN 0(2 -> conectado, !=2 -> desconectado)
        { ar & AR_SERIALIZATION_M(t, estEthernet1);  } ///< 0 Estado de la boca de LAN 1(2 -> conectado, !=2 -> desconectado)
    }

    /// Intermediario para serializar PUESTOS_INTEGRANTES struct
    template<typename Archive>
    void serialize(Archive & ar, PUESTOS_INTEGRANTES & t)
    {

        // Límite del array
        uint32 numero = t.numero;
        { ar & AR_SERIALIZATION_M(t, numero);  } ///< Numero puestos que integran el ejercicio
        { ar & AR_SERIALIZATION_L(t, idPuesto, numero);
        } ///< Puestos
    }

    /// Intermediario para serializar GATEWAYS_INTEGRANTES struct
    template<typename Archive>
    void serialize(Archive & ar, GATEWAYS_INTEGRANTES & t)
    {

        // Límite del array
        uint32 numero = t.numero;
        { ar & AR_SERIALIZATION_M(t, numero);  } ///< Numero gateways que integran el ejercicio
        { ar & AR_SERIALIZATION_L(t, idGateway, numero);
        } ///< gateways
    }

    /// Intermediario para serializar DAT_FICH_CONFIG struct
    template<typename Archive>
    void serialize(Archive & ar, DAT_FICH_CONFIG & t)
    {
        { ar & AR_SERIALIZATION_M(t, numBloques);  } ///< Numero total de bloques (n)
        { ar & AR_SERIALIZATION_M(t, numBytes);  } ///< Numero de bytes del fichero de configuración
        { ar & AR_SERIALIZATION_M(t, bloque);  } ///< Numero de bloque tx/rx (1 .... n)
        { ar & AR_SERIALIZATION_M(t, numBytesBloque);  } ///< Numero de bytes del bloque tx/rx (por defecto 256 bytes, excepto el ultimo)
        { ar & AR_SERIALIZATION_M(t, nombreFichero);  }
        { ar & AR_SERIALIZATION_M(t, crcFichero);  } ///< CRC del fichero que se va a enviar
        { ar & AR_SERIALIZATION_M(t, idFichero);  }
        { ar & AR_SERIALIZATION_M(t, subidFichero);  }
    }

    /// Intermediario para serializar MSG_CONFIG_INF_FICH_CONFIG struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_CONFIG_INF_FICH_CONFIG & t)
    {
        { ar & AR_SERIALIZATION_M(t, numSesion);  } ///< Número de sesion
        { ar & AR_SERIALIZATION_M(t, fich);  }
        { ar & AR_SERIALIZATION_M(t, puestosIntegrantes);  }
        { ar & AR_SERIALIZATION_M(t, gatewaysIntegrantes);  }
    }

    /// Intermediario para serializar MSG_CONFIG_FICH_CONFIG struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_CONFIG_FICH_CONFIG & t)
    {
        { ar & AR_SERIALIZATION_M(t, numSesion);  } ///< Numero de sesion
        { ar & AR_SERIALIZATION_M(t, fichCorrecto);  } ///< En la respuesta del fichero de configuración: * True (1): Bien, se ha recibido bien el fichero * False (0): Mal, se ha recibido mal el fichero
        { ar & AR_SERIALIZATION_M(t, fich);  }
        { ar & AR_SERIALIZATION_M(t, byte);  } ///< Bytes del fichero
    }

    /// Intermediario para serializar MSG_ENVIA_TABLA struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_ENVIA_TABLA & t)
    {
        { ar & AR_SERIALIZATION_M(t, nombreTabla);  }
        { ar & AR_SERIALIZATION_M(t, sistema);  }
        { ar & AR_SERIALIZATION_M(t, numBloques);  } ///< Numero de bloques total del mensaje
        { ar & AR_SERIALIZATION_M(t, bloque);  } ///< Numero que identifica a este mensaje
        { ar & AR_SERIALIZATION_M(t, bufferTabla);  }
    }

    /// Intermediario para serializar MSG_ARRANQUE_CONFIG struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_ARRANQUE_CONFIG & t)
    {
        { ar & AR_SERIALIZATION_M(t, numSesion);  } ///< Numero de sesion
        { ar & AR_SERIALIZATION_M(t, infCrc);  } ///< Información de Id y CRC de los ficheros
        { ar & AR_SERIALIZATION_M(t, puestosIntegrantes);  }
        { ar & AR_SERIALIZATION_M(t, gatewaysIntegrantes);  }
        { ar & AR_SERIALIZATION_M(t, sistRd);  }
        { ar & AR_SERIALIZATION_M(t, sistTf);  }
    }

    /// Intermediario para serializar MSG_SECTORIZA struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_SECTORIZA & t)
    {
        { ar & AR_SERIALIZATION_M(t, idSectorizacion);  } ///< Numero que identifica la sectorizacion implantada
        { ar & AR_SERIALIZATION_M(t, procedencia);
            t.procedencia = (E_PROCEDENCIA_SECTORIZ)ntoh((unsigned long) t.procedencia); } ///< Procedencia sectorizacion
        { ar & AR_SERIALIZATION_M(t, motivo);
            t.motivo = (E_MOTIVO_SECTORIZ)ntoh((unsigned long) t.motivo); } ///< Motivo por el que se envía la sectorización
        { ar & AR_SERIALIZATION_M(t, Estado);  } ///< Sectorizacion Aceptada(1) o Rechazada (0)
        { ar & AR_SERIALIZATION_M(t, causa);  } ///< Causa de rechazo de la sectorización
    }

    /// Intermediario para serializar SAM_timeb struct
    template<typename Archive>
    void serialize(Archive & ar, SAM_timeb & t)
    {
        { ar & AR_SERIALIZATION_M(t, time);  } ///< In Windows, define _USE_32BIT_TIME_T for the type to be a 32 bits integer
        { ar & AR_SERIALIZATION_M(t, millitm);  }
        { ar & AR_SERIALIZATION_M(t, timezone);  }
        { ar & AR_SERIALIZATION_M(t, dstflag);  }
    }

    /// Intermediario para serializar MSG_DATETIME_SG struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_DATETIME_SG & t)
    {
        { ar & AR_SERIALIZATION_M(t, datetime);  }
    }

    /// Intermediario para serializar ABO_PUE_TEL_EST struct
    template<typename Archive>
    void serialize(Archive & ar, ABO_PUE_TEL_EST & t)
    {
        { ar & AR_SERIALIZATION_M(t, tipoEq);  } ///< TIPO EQUIPO (PUES o GW) (1 Byte) ///< [Toma los valores desde E_TIPO_EQUIPO_EST]
        if( t.tipoEq == EQ_EST_GATEWAY )
        { ar & AR_SERIALIZATION_M(t, tipoGw);  } ///< TIPO de GATEWAY (1 Byte) ///< [Toma los valores desde E_TIPO_GATEWAY]
        else
        { ar & AR_SERIALIZATION_M(t, tipoPues);  } ///< TIPO de PUESTO (1 Byte) ///< [Toma los valores desde E_TIPO_PUESTO]
        { ar & AR_SERIALIZATION_M(t, idEq);  } ///< ID EQUIPO(T_PUESTOS/T_GATEWAYS) (2 Bytes)
        if( t.tipoEq == EQ_EST_GATEWAY )
        { ar & AR_SERIALIZATION_M(t, NumMicroSwitchGw);  } ///< NUMERO MICROSWITH para GW (1 Byte)
        else
        { ar & AR_SERIALIZATION_M(t, tipoAbon);  } ///< TIPO ABONADO para puestos (1 Byte) ///< [Toma los valores desde E_TIPO_ABONADO_INTERNO]
        if( t.tipoEq == EQ_EST_GATEWAY )
        { ar & AR_SERIALIZATION_M(t, idEnl);  } ///< TARJETA GATEWAY (2 Bytes)
        else
        { ar & AR_SERIALIZATION_M(t, idAbon);  } ///< ID ABONADO (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, strAbon);  } ///< NOMBRE ABONADO (20 Bytes)
        { ar & AR_SERIALIZATION_M(t, slotGw);  } ///< SLOT TARJETA GATEWAY (1 Byte)
        if( t.tipoEq == EQ_EST_GATEWAY )
        { ar & AR_SERIALIZATION_M(t, tipoTarjetaGw);  } ///< TIPO DE TARJETA GW (1 Byte)
        ///< Con el tipo de tarjeta se determinará el tipo de red a través del que se hace una llamada:
        ///< - Redes PSTN (públicas) -> EYMDTMF, PABX, E1, BRI, FXO, ISDN_ST
        ///< - Redes AGVN (red entre centros de control) -> R2, QSIG, N5
        ///< - Redes AGVN punto a punto (TFU/FXS, BL, BC, EYM, LCE) ///< [Toma los valores desde E_TIPO_ENLACE]
        else
        { ar & AR_SERIALIZATION_M(t, tipoAbonSimple_Integrado);  } ///< ABONADO SIMPLE/INTEGRADO (1 Byte)
        { ar & AR_SERIALIZATION_M(t, idCentro);  } ///< ID CENTRO (2 Bytes). Identificador del centro(o red) origen/destino de la llamada
        { ar & AR_SERIALIZATION_M(t, pref);  } ///< PREFIJO (5 Bytes). Prefijo abonado origen/destino de la llamada (Puede ser un campo vacio)
        { ar & AR_SERIALIZATION_M(t, codigo);  } ///< CODIGO (20 Bytes). Código abonado origen/destino de la llamada (puede ser el ATS o el público)
        { ar & AR_SERIALIZATION_M(t, estComu);  } ///< ESTADO DE LA COMUNICACION (1 Byte) ///< [Toma los valores desde E_EST_TF]
        { ar & AR_SERIALIZATION_M(t, estComuAnt);  } ///< ESTADO DE LA COMUNICACION ANTERIOR (1 Byte) ///< [Toma los valores desde E_EST_TF]
        { ar & AR_SERIALIZATION_M(t, subEst);  } ///< SUBESTADO ORIGEN (1 Byte) ///< [Toma los valores desde E_SUBESTADO]
    }

    /// Intermediario para serializar MSG_EST_COMU_TLF struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_EST_COMU_TLF & t)
    {
        { ar & AR_SERIALIZATION_M(t, subIdMens);  } ///< SUBIDENTIFICADOR MENSAJE(1 Byte) ///< [Toma los valores desde E_SUB_ID_MSJ_TLF]
        { ar & AR_SERIALIZATION_M(t, sistema);  } ///< SISTEMA (A/B/C) (1 Byte) ///< [Toma los valores desde E_SISTEMA_ABC]
        { ar & AR_SERIALIZATION_M(t, idConfig);  } ///< ID_CONFIG (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, subIdConfig);  } ///< SUB_ID_CONFIG (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, sesion);  } ///< SESIÓN (1 Byte)
        { ar & AR_SERIALIZATION_M(t, org);  } ///< Origen llamada
        { ar & AR_SERIALIZATION_M(t, des);  } ///< Destino llamada
        { ar & AR_SERIALIZATION_M(t, servicio);  } ///< (AD/AI/LC) (1 Byte) ///< [Toma los valores desde E_SERVICIO]
        { ar & AR_SERIALIZATION_M(t, prioridad);  } ///< [Toma los valores desde E_TIPO_PRIORIDAD_R2]
        { ar & AR_SERIALIZATION_M(t, tipoRuta);  } ///< [Toma los valores desde E_TIPO_RUTA_EST]
    }

    /// Intermediario para serializar ABO_PUE_RD_EST struct
    template<typename Archive>
    void serialize(Archive & ar, ABO_PUE_RD_EST & t)
    {
        { ar & AR_SERIALIZATION_M(t, tipoEq);  } ///< TIPO EQUIPO (PUES o GW) (1 Byte) ///< [Toma los valores desde E_TIPO_EQUIPO_EST]
        if( t.tipoEq == EQ_EST_GATEWAY )
        { ar & AR_SERIALIZATION_M(t, tipoGw);  } ///< TIPO de GATEWAY (1 Byte) ///< [Toma los valores desde E_TIPO_GATEWAY]
        else
        { ar & AR_SERIALIZATION_M(t, tipoPues);  } ///< TIPO de PUESTO (1 Byte) ///< [Toma los valores desde E_TIPO_PUESTO]
        { ar & AR_SERIALIZATION_M(t, idEq);  } ///< ID EQUIPO(PUESTO/GATEWAY) (2 Bytes)
        if( t.tipoEq == EQ_EST_GATEWAY )
        { ar & AR_SERIALIZATION_M(t, NumMicroSwitchGw);  } ///< NUMERO MICROSWITH para GW (1 Byte)
        else
        { ar & AR_SERIALIZATION_M(t, tipoAbon);  } ///< TIPO ABONADO para puestos (1 Byte) ///< [Toma los valores desde E_TIPO_ABONADO_INTERNO]
        { ar & AR_SERIALIZATION_M(t, idAbon);  } ///< ID ABONADO (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, tipoAbonSimple_Integrado);  } ///< TIPO DE TARJETA GW/ABONADO SIMPLE/INTEGRADO (1 Byte)
        { ar & AR_SERIALIZATION_M(t, strAbon);  } ///< NOMBRE ABONADO (20 Bytes)
    }

    /// Intermediario para serializar CANAL_EST struct
    template<typename Archive>
    void serialize(Archive & ar, CANAL_EST & t)
    {
        { ar & AR_SERIALIZATION_M(t, idFrec);  } ///< ID FRECUENCIA n (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, idEmpl);  } ///< ID EMPLAZAMIENTO n (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, idGw);  } ///< ID GATEWAY n (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, idTarj);  } ///< ID TARJETA n (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, slot);  } ///< SLOT TARJETA n (1 Byte)
        { ar & AR_SERIALIZATION_M(t, tipoTarj);  } ///< TIPO TARJETA n (CR, CR_BSS, CR_BSS2) (1 Byte) ///< [Toma los valores desde E_TIPO_ENLACE]
        { ar & AR_SERIALIZATION_M(t, estado1);  } ///< ESTADO1 FRECUENCIA n (1 Byte)
        { ar & AR_SERIALIZATION_M(t, estado2);  } ///< ESTADO2 FRECUENCIA n (1 Byte)
    }

    /// Intermediario para serializar CANAL_BSS_EST struct
    template<typename Archive>
    void serialize(Archive & ar, CANAL_BSS_EST & t)
    {
        { ar & AR_SERIALIZATION_M(t, idFrec);  } ///< ID FRECUENCIA n (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, idEmpl);  } ///< ID EMPLAZAMIENTO n (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, idTarj);  } ///< ID TARJETA n (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, slot);  } ///< SLOT TARJETA n (1 Byte)
        { ar & AR_SERIALIZATION_M(t, tipoTarj);  } ///< TIPO TARJETA n (CR, CR_BSS, CR_BSS2) ///< [Toma los valores desde E_TIPO_ENLACE]
        { ar & AR_SERIALIZATION_M(t, calFrec);  } ///< CALIDAD FRECUENCIA n (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, RelSenalRuido);  } ///< RELACION SNR FRECUENCIA n (2 Bytes)
    }

    /// Intermediario para serializar MSG_EST_COMU_RAD struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_EST_COMU_RAD & t)
    {

        // Límite del array
        uint8 numCanales = t.numCanales;
        { ar & AR_SERIALIZATION_M(t, subIdMens);  } ///< SUBIDENTIFICADOR MENSAJE RADIO(1 Byte) ///< [Toma los valores desde E_SUB_ID_MSJ_RAD]
        { ar & AR_SERIALIZATION_M(t, sistema);  } ///< SISTEMA (A/B/C) (1 Byte) ///< [Toma los valores desde E_SISTEMA_ABC]
        { ar & AR_SERIALIZATION_M(t, idConfig);  } ///< ID_CONFIG (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, subIdConfig);  } ///< SUB_ID_CONFIG (2 Bytes)
        { ar & AR_SERIALIZATION_M(t, sesion);  } ///< SESIÓN (1 Byte)
        { ar & AR_SERIALIZATION_M(t, org);  } ///< Origen llamada
        { ar & AR_SERIALIZATION_M(t, estFuncion);  } ///< ESTADO FUNCIÓN (1 Byte)
        { ar & AR_SERIALIZATION_M(t, numCanales);  } ///< NÚMERO DE CANALES rellenados de la estructura siguiente(1 Byte)
        if( t.subIdMens == CALIDAD_BSS )
        { ar & AR_SERIALIZATION_L(t, canalBSS, numCanales);
        }
        else
        { ar & AR_SERIALIZATION_L(t, canal, numCanales);
        }
    }

    /// Intermediario para serializar MSG_CONFIG_SESION struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_CONFIG_SESION & t)
    {
        { ar & AR_SERIALIZATION_M(t, numSesion);  }
        { ar & AR_SERIALIZATION_M(t, infCrc);  }
    }

    /// Intermediario para serializar MSG_ENLACE_SACTA struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_ENLACE_SACTA & t)
    {
        { ar & AR_SERIALIZATION_M(t, estado);  }
    }

    /// Intermediario para serializar MSG_MEDIDAS_RED struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_MEDIDAS_RED & t)
    {
        { ar & AR_SERIALIZATION_M(t, ipdvDigital);  } ///< Medida IPDV Digital
        { ar & AR_SERIALIZATION_M(t, paquetesPerdidos);  } ///< Medida Perdida de paquetes
    }

    /// Intermediario para serializar MSG_ESTADO_RED struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_ESTADO_RED & t)
    {
        { ar & AR_SERIALIZATION_M(t, estadoLanGestion);  } ///< Estado de la LAN de Gestión (1 byte)
        { ar & AR_SERIALIZATION_M(t, estadoLanVoz);  } ///< Estado de la LAN de Voz (1 byte)
    }

    /// Intermediario para serializar RES_MEDIDA struct
    template<typename Archive>
    void serialize(Archive & ar, RES_MEDIDA & t)
    {
        { ar & AR_SERIALIZATION_M(t, id);  } ///< Identificador de la medida
        { ar & AR_SERIALIZATION_M(t, valor);  } ///< Valor de la medida para el canal/bucle
        { ar & AR_SERIALIZATION_M(t, medida);  } ///< Clasificación del resultado
    }

    /// Intermediario para serializar MEDIDAS_CANAL_BUCLE struct
    template<typename Archive>
    void serialize(Archive & ar, MEDIDAS_CANAL_BUCLE & t)
    {

        // Límite del array
        uint8 numMedidas = t.numMedidas;
        { ar & AR_SERIALIZATION_M(t, tipoBucle);  } ///< Tipo de bucle o canal
        { ar & AR_SERIALIZATION_M(t, numMedidas);  } ///< Numero de medidas que se van a hacer en el bucle o canal
        { ar & AR_SERIALIZATION_L(t, medidas, numMedidas);
        } ///< Numero de medidas que se van a hacer en el bucle o canal
    }

    /// Intermediario para serializar MSG_INTERFAZ_BATERIA struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_INTERFAZ_BATERIA & t)
    {

        // Límite del array
        uint8 numCanales = t.numCanales;
        { ar & AR_SERIALIZATION_M(t, idSesion);  } ///< Identificador de la sesión del usuario
        { ar & AR_SERIALIZATION_M(t, idBateria);  } ///< Identificador de la batería de pruebas a la que estan asociadas las pruebas de la posición
        { ar & AR_SERIALIZATION_M(t, idGW);  } ///< Identificador del GW al que pertenece el interfaz (2 bytes)
        { ar & AR_SERIALIZATION_M(t, idInterfaz);  } ///< Identificador del interfaz (2 bytes)
        { ar & AR_SERIALIZATION_M(t, slot);  } ///< Slot que ocupa el interfaz en el GW (1 byte)
        { ar & AR_SERIALIZATION_M(t, tipo);  } ///< Tipo de interfaz sobre el que se realizó la medida (1 byte) ///< [Toma los valores desde E_TIPO_ENLACE]
        { ar & AR_SERIALIZATION_M(t, numCanales);  } ///< Número de canales a medir en el interfaz
        { ar & AR_SERIALIZATION_L(t, canales, numCanales);
        } ///< Estructura MED_CANAL repetida numCanales veces
    }

    /// Intermediario para serializar MSG_POSICION_BATERIA struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_POSICION_BATERIA & t)
    {

        // Límite del array
        uint8 numBucles = t.numBucles;
        { ar & AR_SERIALIZATION_M(t, idSesion);  } ///< Identificador de la sesión del usuario
        { ar & AR_SERIALIZATION_M(t, idBateria);  } ///< Identificador de la batería de pruebas a la que estan asociadas las pruebas de la posición
        { ar & AR_SERIALIZATION_M(t, idPosicion);  } ///< Identificador de la posición
        { ar & AR_SERIALIZATION_M(t, numBucles);  } ///< Numero de bucles para los que se definen pruebas
        { ar & AR_SERIALIZATION_L(t, bucles, numBucles);
        } ///< Estructura que contiene las medidas que se haran sobre los bucles
    }

    /// Intermediario para serializar MSG_RED_SAM struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_RED_SAM & t)
    {

        // Límite del array
        uint8 numMedidas = t.numMedidas;
        { ar & AR_SERIALIZATION_M(t, fecha);  } ///< Fecha de inicio de la ejecución de las pruebas
        { ar & AR_SERIALIZATION_M(t, idCentro);  } ///< Identificador del centro en el que se ejecutó la BdP
        { ar & AR_SERIALIZATION_M(t, idConfig);  } ///< ID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        { ar & AR_SERIALIZATION_M(t, subIdConfig);  } ///< SubID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        { ar & AR_SERIALIZATION_M(t, numMedidas);  } ///< Número de medidas de red realizadas
        { ar & AR_SERIALIZATION_L(t, medida, numMedidas);
        } ///< Estructura RES_MEDIDA repetida numMedidas veces
    }

    /// Intermediario para serializar MED_CANAL_BUCLE struct
    template<typename Archive>
    void serialize(Archive & ar, MED_CANAL_BUCLE & t)
    {

        // Límite del array
        uint8 numMedidas = t.numMedidas;
        { ar & AR_SERIALIZATION_M(t, id);  } ///< Id. del bucle
        { ar & AR_SERIALIZATION_M(t, numMedidas);  } ///< Número de medidas realizadas sobre el bucle
        { ar & AR_SERIALIZATION_L(t, medida, numMedidas);
        } ///< Estructura RES_MEDIDA repetida numMedidas veces
    }

    /// Intermediario para serializar MED_POSICION struct
    template<typename Archive>
    void serialize(Archive & ar, MED_POSICION & t)
    {

        // Límite del array
        uint8 numBucles = t.numBucles;
        { ar & AR_SERIALIZATION_M(t, id);  } ///< Identificador de la posición (2 bytes)
        { ar & AR_SERIALIZATION_M(t, ip);  } ///< Dirección IP de la posición (4 bytes)
        { ar & AR_SERIALIZATION_M(t, resultadoPos);  } ///< Clasificación del resultado general de la posición
        { ar & AR_SERIALIZATION_M(t, numBucles);  } ///< Número de bucles medidos en la posición
        { ar & AR_SERIALIZATION_L(t, bucle, numBucles);
        } ///< Estructura MED_CANAL_BUCLE repetida numBucles veces
    }

    /// Intermediario para serializar MED_INTERFAZ struct
    template<typename Archive>
    void serialize(Archive & ar, MED_INTERFAZ & t)
    {

        // Límite del array
        uint8 numCanales = t.numCanales;
        { ar & AR_SERIALIZATION_M(t, idInterfaz);  } ///< Identificador del interfaz (2 bytes)
        { ar & AR_SERIALIZATION_M(t, idGW);  } ///< Identificador de usuario del GW al que pertenece el interfaz (2 bytes)
        { ar & AR_SERIALIZATION_M(t, slot);  } ///< Slot que ocupa el interfaz en el GW (1 byte)
        { ar & AR_SERIALIZATION_M(t, tipo);  } ///< Tipo de interfaz sobre el que se realizó la medida (1 byte)
        { ar & AR_SERIALIZATION_M(t, resultadoInt);  } ///< Clasificación del resultado general de la interfaz
        { ar & AR_SERIALIZATION_M(t, numCanales);  } ///< Número de canales medidos en el interfaz
        { ar & AR_SERIALIZATION_L(t, canal, numCanales);
        } ///< Estructura MED_CANAL_BUCLE repetida numCanales veces
    }

    /// Intermediario para serializar MSG_AUDIO_SAM_POSICION struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_AUDIO_SAM_POSICION & t)
    {
        { ar & AR_SERIALIZATION_M(t, id);  } ///< Identificador de la Batería de Pruebas
        { ar & AR_SERIALIZATION_M(t, fecha);  } ///< Fecha de inicio de la ejecución de las pruebas
        { ar & AR_SERIALIZATION_M(t, idCentro);  } ///< Identificador del centro en el que se ejecutó la BdP
        { ar & AR_SERIALIZATION_M(t, idConfig);  } ///< ID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        { ar & AR_SERIALIZATION_M(t, subIdConfig);  } ///< SubID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        { ar & AR_SERIALIZATION_M(t, posicion);  } ///< Estructura con el resultado de las medidas realizadas para una posición
    }

    /// Intermediario para serializar MSG_AUDIO_SAM_INTERFAZ struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_AUDIO_SAM_INTERFAZ & t)
    {
        { ar & AR_SERIALIZATION_M(t, id);  } ///< Identificador de la Batería de Pruebas
        { ar & AR_SERIALIZATION_M(t, fecha);  } ///< Fecha de inicio de la ejecución de las pruebas
        { ar & AR_SERIALIZATION_M(t, idCentro);  } ///< Identificador del centro en el que se ejecutó la BdP
        { ar & AR_SERIALIZATION_M(t, idConfig);  } ///< ID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        { ar & AR_SERIALIZATION_M(t, subIdConfig);  } ///< SubID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        { ar & AR_SERIALIZATION_M(t, interfaz);  } ///< Estructura con el resultado de las medidas sobre los interfaces
    }

    /// Intermediario para serializar MSG_PRES_VRP struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_PRES_VRP & t)
    {
        { ar & AR_SERIALIZATION_M(t, estadoFTP);  } ///< Estado del servidor FTP (1 byte)
        { ar & AR_SERIALIZATION_M(t, espacioTotal);  } ///< Espacio total en en MB (4 bytes)
        { ar & AR_SERIALIZATION_M(t, espacioOcupado);  } ///< Espacio ocupado en disco en MB (4 bytes)
    }

    /// Intermediario para serializar MSG_PRES_ESTAD struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_PRES_ESTAD & t)
    {
        { ar & AR_SERIALIZATION_M(t, estadoWeb);  } ///< Estado del servidor Web (1 byte)
        { ar & AR_SERIALIZATION_M(t, estadoBd);  } ///< Estado del servidor BD (1 byte)
        { ar & AR_SERIALIZATION_M(t, estadoServer);  } ///< Estado del servidor PHP (1 byte)
        { ar & AR_SERIALIZATION_M(t, estadoCluster);  } ///< Estado del cluster de servidores (1 byte)
        { ar & AR_SERIALIZATION_M(t, estadoBackup);  } ///< Estado de los backups (1 byte)
        { ar & AR_SERIALIZATION_M(t, espacioTotal);  } ///< Espacio total en disco local o cabina de discos en MB (4 bytes)
        { ar & AR_SERIALIZATION_M(t, espacioOcupado);  } ///< Espacio ocupado en disco local o cabina de discos en MB (4 bytes)
    }

    /// Intermediario para serializar DAT_INICIO_SESION_SAM struct
    template<typename Archive>
    void serialize(Archive & ar, DAT_INICIO_SESION_SAM & t)
    {
        { ar & AR_SERIALIZATION_M(t, rolUsuario);  } ///< Rol del ususario que inicia la sesion
    }

    /// Intermediario para serializar MSG_BATERIA_POR_ID struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_BATERIA_POR_ID & t)
    {
        { ar & AR_SERIALIZATION_M(t, idSesion);  } ///< Identificador de la sesion del usuario que hace la consulta de la bateria de pruebas
        { ar & AR_SERIALIZATION_M(t, idBateria);  } ///< Identificador de la bateria de pruebas que se quiere consultar
    }

    /// Intermediario para serializar MSG_ENVIA_BATERIA_VACIA struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_ENVIA_BATERIA_VACIA & t)
    {
        { ar & AR_SERIALIZATION_M(t, idSesion);  } ///< Identificador de la sesion del usuario que hace la consulta de la bateria de pruebas
        { ar & AR_SERIALIZATION_M(t, nombre);  } ///< Nombre de la bateria de pruebas a crear
        { ar & AR_SERIALIZATION_M(t, fechaCreacion);  } ///< Fecha de creacion de la bateria de pruebas
        { ar & AR_SERIALIZATION_M(t, idCentro);  } ///< Identificador del centro en el que se ejecutó la BdP
        { ar & AR_SERIALIZATION_M(t, idConfig);  } ///< ID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        { ar & AR_SERIALIZATION_M(t, subIdConfig);  } ///< SubID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
    }

    /// Intermediario para serializar MSG_ID_SESION struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_ID_SESION & t)
    {
        { ar & AR_SERIALIZATION_M(t, idSesion);  } ///< Identificador de la sesion del usuario para envio de peticiones al servidor SAM
    }

    /// Intermediario para serializar M_UMBRAL_INTERVALO struct
    template<typename Archive>
    void serialize(Archive & ar, M_UMBRAL_INTERVALO & t)
    {
        { ar & AR_SERIALIZATION_M(t, rMin);  } ///< Umbral inferior. Usar qNan para un valor nulo.
        { ar & AR_SERIALIZATION_M(t, rMax);  } ///< Umbral superior. Usar qNan para un valor nulo.
    }

    /// Intermediario para serializar M_UMBRALES_MEDIDAS struct
    template<typename Archive>
    void serialize(Archive & ar, M_UMBRALES_MEDIDAS & t)
    {
        { ar & AR_SERIALIZATION_M(t, atenuacion);  } ///< Atenuacion en funcion de la frecuencia
        { ar & AR_SERIALIZATION_M(t, ruido);  } ///< Ruido del canal en reposo
        { ar & AR_SERIALIZATION_M(t, distorsion);  } ///< Distorsion
        { ar & AR_SERIALIZATION_M(t, retardo);  } ///< Retardo
        { ar & AR_SERIALIZATION_M(t, pesq);  } ///< Indice electrico de calidad de voz
        { ar & AR_SERIALIZATION_M(t, rtemporal);  } ///< Recorte temporal
        { ar & AR_SERIALIZATION_M(t, eco);  } ///< Eco
        { ar & AR_SERIALIZATION_M(t, diafonia);  } ///< Diafonia
        { ar & AR_SERIALIZATION_M(t, ipdvAnalogico);  } ///< IPDV (Inter Packet Delay Variation) analogico
        { ar & AR_SERIALIZATION_M(t, ipdvDigital);  } ///< IPDV (Inter Packet Delay Variation) digital
        { ar & AR_SERIALIZATION_M(t, paquetes);  } ///< Paquetes perdidos
    }

    /// Intermediario para serializar MSG_UMBRALES struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_UMBRALES & t)
    {
        { ar & AR_SERIALIZATION_M(t, idSesion);  } ///< Identificador de la sesion del usuario
        { ar & AR_SERIALIZATION_M(t, umbrales);  } ///< Numero de medidas de red a realizar
    }

    /// Intermediario para serializar MSG_EJECUTA_LISTA_MEDIDAS_RED struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_EJECUTA_LISTA_MEDIDAS_RED & t)
    {

        // Límite del array
        uint8 numMedidasRed = t.numMedidasRed;
        { ar & AR_SERIALIZATION_M(t, idSesion);  } ///< Identificador de la sesion del usuario
        { ar & AR_SERIALIZATION_M(t, numMedidasRed);  } ///< Numero de medidas de red a realizar
        { ar & AR_SERIALIZATION_L(t, medidas, numMedidasRed);
        } ///< Lista de medidas de red a realizar
    }

    /// Intermediario para serializar MSG_MODIFICA_PARAMETROS struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_MODIFICA_PARAMETROS & t)
    {

        // Límite del array
        uint8 numParametros = t.numParametros;
        { ar & AR_SERIALIZATION_M(t, idSesion);  } ///< Identificador de la sesion del usuario
        { ar & AR_SERIALIZATION_M(t, numParametros);  } ///< Numero de parámetros a modificar
        { ar & AR_SERIALIZATION_L(t, parametros, numParametros);
        } ///< Parámetros de configuración con sus valores para su modificacion
    }

    /// Intermediario para serializar MSG_POSICION_SAM struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_POSICION_SAM & t)
    {
        { ar & AR_SERIALIZATION_M(t, activarSam);  } ///< Peticion de activacion o desactivacion de la posición o el patrón en modo medición
        { ar & AR_SERIALIZATION_M(t, puesto);  } ///< Direccion IP del puesto o patrón (4 bytes)
        { ar & AR_SERIALIZATION_M(t, abonado);  } ///< Tipo de abonado SAM: MEDIDAS o PATRÓN
    }

    /// Intermediario para serializar MSG_RESP_POSICION struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_RESP_POSICION & t)
    {
        { ar & AR_SERIALIZATION_M(t, resultado);  } ///< Estado de activacion de la posición
        { ar & AR_SERIALIZATION_M(t, abonado);  } ///< Tipo de abonado SAM: MEDIDAS o PATRÓN
        { ar & AR_SERIALIZATION_M(t, motivoRechazo);  } ///< Error por el que la posición no se ha podido activar
    }

    /// Intermediario para serializar MSG_INTERFAZ_MEDICION_SAM struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_INTERFAZ_MEDICION_SAM & t)
    {
        { ar & AR_SERIALIZATION_M(t, activarMedicionSam);  } ///< ACTIVAR_MEDICION_SAM.- Peticion de activacion o desactivacion del interfaz en modo medición
        { ar & AR_SERIALIZATION_M(t, slot);  } ///< INTERFAZ_DESTINO.- Numero de slot del gw donde se encuentra la interfaz a medir (en caso de ISDN o QSIG se suma el número de canal 0, 1 ó 2)
    }

    /// Intermediario para serializar MSG_RESP_MEDICION_SAM struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_RESP_MEDICION_SAM & t)
    {
        { ar & AR_SERIALIZATION_M(t, estado);  } ///< Estado en que ha quedado el interfaz tras la peticion de activacion o desactivacion del modo medición
        { ar & AR_SERIALIZATION_M(t, slot);  } ///< Numero de slot del gw donde se encuentra la interfaz a medir (en caso de ISDN o QSIG se suma el número de canal 0, 1 ó 2)
    }

    /// Intermediario para serializar MSG_SET_LLAMADA_SAM struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_SET_LLAMADA_SAM & t)
    {
        { ar & AR_SERIALIZATION_M(t, estado);  } ///< ON: establecer la llamada, OFF: finalizar la llamada
        { ar & AR_SERIALIZATION_M(t, destino);  } ///< Posición o interfaz con el que la posición patrón debe establecer la llamada
    }

    /// Intermediario para serializar MSG_RESP_LLAMADA_SAM struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_RESP_LLAMADA_SAM & t)
    {
        { ar & AR_SERIALIZATION_M(t, estado);  } ///< ON: llamada establecida, OFF: llamada no establecida
        { ar & AR_SERIALIZATION_M(t, destino);  } ///< Posición o interfaz con el que la posición patrón ha establecido la llamada
    }

    /// Intermediario para serializar MSG_SET_BUCLE_SAM struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_SET_BUCLE_SAM & t)
    {
        { ar & AR_SERIALIZATION_M(t, nbucle);  } ///< Dispositvo de la UGA cuyo bucle se quiere activar
        { ar & AR_SERIALIZATION_M(t, estado);  } ///< ON: activar el bucle (o bucle activado), OFF: desactivar el bucle (o bucle desactivado)
    }

    /// Intermediario para serializar MSG_PRES_SAM struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_PRES_SAM & t)
    {
        { ar & AR_SERIALIZATION_M(t, modoSAM);  } ///< Estado de las mediciones
    }

    /// Intermediario para serializar MSG_PRES_WATCHDOG_SAM struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_PRES_WATCHDOG_SAM & t)
    {
        { ar & AR_SERIALIZATION_M(t, subtipo);  }
        { ar & AR_SERIALIZATION_M(t, longId);  }
        { ar & AR_SERIALIZATION_M(t, proceso);  }
    }

    /// Intermediario para serializar MSG_TH struct
    template<typename Archive>
    void serialize(Archive & ar, MSG_TH & t)
    {
        { ar & AR_SERIALIZATION_M(t, cab);  } ///< VER PUNTO CABECERA DE TODOS LOS MENSAJES
        if( t.cab.idMsg == M_PRESENCIA )
        { ar & AR_SERIALIZATION_M(t, super);  } ///< Mensajes presencia
        else if( t.cab.idMsg == M_INICIO_SESION )
        { ar & AR_SERIALIZATION_M(t, sesion);  } ///< Para solicitar ficheros al TMCS
        else if( t.cab.idMsg == M_INF_FICH_CONFIG )
        { ar & AR_SERIALIZATION_M(t, infFichConfig);  } ///< Para mensajes M_INF_FICH_CONFIG
        else if( t.cab.idMsg == M_FICH_CONFIG )
        { ar & AR_SERIALIZATION_M(t, fichConf);  } ///< Para mensajes M_FICH_CONFIG
        else if( t.cab.idMsg == M_ARRANQUE )
        { ar & AR_SERIALIZATION_M(t, arranque);  } ///< Para mensajes M_ARRANQUE
        else if( t.cab.idMsg == M_CONFIG_IMPLANTADA )
        { ar & AR_SERIALIZATION_M(t, configImplantada);  } ///< Para mensajes M_CONFIG_IMPLANTADA
        else if( t.cab.idMsg == M_ENVIA_TABLA )
        { ar & AR_SERIALIZATION_M(t, enviatabla);  } ///< Mensajes tablas sectorizacion
        else if( t.cab.idMsg == M_DATETIME_SG )
        { ar & AR_SERIALIZATION_M(t, m_datetime_sg);  } ///< Mensajes fecha y hora
        else if( t.cab.idMsg == M_ESTADISTICAS_TF )
        { ar & AR_SERIALIZATION_M(t, mEstadisComuTlf);  } ///< Mensajes estadisticas de comunicaciones telefonía
        else if( t.cab.idMsg == M_ESTADISTICAS_RD )
        { ar & AR_SERIALIZATION_M(t, mEstadisComuRad);  } ///< Mensajes estadisticas de comunicaciones radio
        else if( t.cab.idMsg == M_SECTORIZA )
        { ar & AR_SERIALIZATION_M(t, sectoriza);  } ///< Mensajes sectorización (no en especificaciones 1.9-)
        else if( t.cab.idMsg == M_ESTADO_CONFIG )
        { ar & AR_SERIALIZATION_M(t, mEstadoConfig);  } ///< Mensajes de estado de las configuraciones
        else if( t.cab.idMsg == M_ENLACE_SACTA )
        { ar & AR_SERIALIZATION_M(t, mEnlaceSacta);  } ///< Subestructura de mensaje de información de estado del enlace con SACTA
        else if( t.cab.idMsg == M_PRES_SAM )
        { ar & AR_SERIALIZATION_M(t, mPresenciaSAM);  } ///< EvtSAM_Presencia Tipo de mensaje de información de estado del servidor SAM a estadísticas (Presencias)
        else if( t.cab.idMsg == M_PRES_WATCHDOG_SAM )
        { ar & AR_SERIALIZATION_M(t, mPresenciaWatchdogSAM);  } ///< Presencia del servidor SAM al Watchdog
        else if( t.cab.idMsg == M_ACK_FICHERO )
        { ar & AR_SERIALIZATION_M(t, mAckFichero);  } ///< Tipo de mensaje de maximo numero de sesiones alcanzado en el servidor SAM
        else if( t.cab.idMsg == M_MAX_SESIONES_SAM )
        { ar & AR_SERIALIZATION_M(t, mDirIP);  } ///< Tipo de mensaje de maximo numero de sesiones alcanzado en el servidor SAM
        else if( t.cab.idMsg == M_PORCENTAJE_AVANCE_SAM )
        { ar & AR_SERIALIZATION_M(t, mAvance);  } ///< Tipo de mensaje de porcentaje de avance en medidas SAM
        else if( t.cab.idMsg == M_INICIO_SESION_SAM )
        { ar & AR_SERIALIZATION_M(t, mInicioSesionSAM);  } ///< Tipo de mensaje de inicio de sesion en el servidor SAM
        else if( t.cab.idMsg == M_FIN_SESION_SAM )
        { ar & AR_SERIALIZATION_M(t, mIdSesion);  } ///< Identificador de sesion
        else if( t.cab.idMsg == M_LISTA_BATERIAS_SAM )
        { ar & AR_SERIALIZATION_M(t, mListaBaterias);  } ///< Tipo de mensaje para la consulta de la lista de baterias de pruebas
        else if( t.cab.idMsg == M_CONSULTA_BATERIA_SAM )
        { ar & AR_SERIALIZATION_M(t, mConsultaBateria);  } ///< Tipo de mensaje para la consulta de una bateria de pruebas
        else if( t.cab.idMsg == M_CREACION_BATERIA_SAM )
        { ar & AR_SERIALIZATION_M(t, mCreaBateria);  } ///< Tipo de mensaje para la creacion de una bateria de pruebas
        else if( t.cab.idMsg == M_POSICION_BATERIA_SAM )
        { ar & AR_SERIALIZATION_M(t, mPosicionBateria);  } ///< Tipo de mensaje para el alta, la modificacion o el borrado de las pruebas de una posición en una bateria de pruebas
        else if( t.cab.idMsg == M_INTERFAZ_BATERIA_SAM )
        { ar & AR_SERIALIZATION_M(t, mInterfazBateria);  } ///< Tipo de mensaje para el alta, la modificacion o el borrado de las pruebas de un interfaz en una bateria de pruebas
        else if( t.cab.idMsg == M_BORRADO_BATERIA_SAM )
        { ar & AR_SERIALIZATION_M(t, mBorraBateria);  } ///< Tipo de mensaje para el borrado de una bateria de pruebas
        else if( t.cab.idMsg == M_RESULTADO_BATERIA_SAM )
        { ar & AR_SERIALIZATION_M(t, mResultadoBateria);  } ///< Tipo de mensaje para la consulta del resultado de una bateria de pruebas
        else if( t.cab.idMsg == M_EJECUTA_BATERIA_SAM )
        { ar & AR_SERIALIZATION_M(t, mEjecutaBateria);  } ///< Tipo de mensaje para la ejecucion de una bateria de pruebas
        else if( t.cab.idMsg == M_CANCELA_BATERIA_SAM )
        { ar & AR_SERIALIZATION_M(t, mCancelaBateria);  } ///< Tipo de mensaje para la cancelacion de la ejecucion de una bateria de pruebas
        else if( t.cab.idMsg == M_LISTA_MEDIDAS_RED )
        { ar & AR_SERIALIZATION_M(t, mMedidasRed);  } ///< Tipo de mensaje para la consulta de las medidas RED
        else if( t.cab.idMsg == M_EJECUTA_MEDIDAS_RED )
        { ar & AR_SERIALIZATION_M(t, mEjecutaMedidasRed);  } ///< Tipo de mensaje para la ejecución de las medidas RED
        else if( t.cab.idMsg == M_CANCELA_MEDIDAS_RED )
        { ar & AR_SERIALIZATION_M(t, mCancelaMedidasRed);  } ///< Tipo de mensaje para la ejecución de las medidas RED
        else if( t.cab.idMsg == M_CONSULTA_UMBRALES )
        { ar & AR_SERIALIZATION_M(t, mConsultaUmbrales);  } ///< Tipo de mensaje para la ejecución de las medidas RED
        else if( t.cab.idMsg == M_MODIFICA_UMBRALES )
        { ar & AR_SERIALIZATION_M(t, mModificaUmbrales);  } ///< Tipo de mensaje para la ejecución de las medidas RED
        else if( t.cab.idMsg == M_CONSULTA_PARAMETROS )
        { ar & AR_SERIALIZATION_M(t, mConsultaParametros);  } ///< Tipo de mensaje para la consulta de los paramertos de configuración del SAM
        else if( t.cab.idMsg == M_MODIFICA_PARAMETROS )
        { ar & AR_SERIALIZATION_M(t, mModificaParametros);  } ///< Tipo de mensaje para la modificacion de los paramertos de configuración del SAM
        else if( t.cab.idMsg == M_AUDIO_SAM_POSICION )
        { ar & AR_SERIALIZATION_M(t, mAudioSAMPosicion);  } ///< Tipo de mensaje para enviar el resultado de la medición en una posición
        else if( t.cab.idMsg == M_AUDIO_SAM_INTERFAZ )
        { ar & AR_SERIALIZATION_M(t, mAudioSAMInterfaz);  } ///< Tipo de mensaje para enviar el resultado de la medición en un interfaz
        else if( t.cab.idMsg == M_RED_SAM )
        { ar & AR_SERIALIZATION_M(t, mRedSAM);  } ///< Tipo de mensaje para enviar el resultado de las pruebas de red
        else if( t.cab.idMsg == M_ACTIVACION_PATRON )
        { ar & AR_SERIALIZATION_M(t, mActivacionPatron);  } ///< Tipo de mensaje para solicitar que una posición se active o desactive en modo patrón
        else if( t.cab.idMsg == M_POSICION_MEDICION_SAM )
        { ar & AR_SERIALIZATION_M(t, mPosicionMedicion);  } ///< Tipo de mensaje para que la posición se active o desactive en modo medición
        else if( t.cab.idMsg == M_RESP_ACTIVACION_PATRON )
        { ar & AR_SERIALIZATION_M(t, mRespActivacionPatron);  } ///< Tipo de mensaje para la respuesta de la peticion de activar o desactivar la posición patrón
        else if( t.cab.idMsg == M_RESP_POSICION_MEDICION_SAM )
        { ar & AR_SERIALIZATION_M(t, mRespPosicionMedicion);  } ///< Tipo de mensaje para la respuesta a la petición para que la posición se active o desactive en modo medición
        else if( t.cab.idMsg == M_INTERFAZ_MEDICION_SAM )
        { ar & AR_SERIALIZATION_M(t, mInterfazMedicion);  } ///< Tipo de mensaje para la respuesta a la petición para que la posición se active o desactive en modo medición
        else if( t.cab.idMsg == M_RESP_MEDICION_SAM )
        { ar & AR_SERIALIZATION_M(t, mRespInterfazMedicion);  } ///< Tipo de mensaje para la respuesta a la petición para que un interfaz se active o desactive en modo medición
        else if( t.cab.idMsg == M_SET_LLAMADA_SAM )
        { ar & AR_SERIALIZATION_M(t, mSetLlamadaSAM);  } ///< Tipo de mensaje para la activacion de la llamada SAM para poder realizar la medición
        else if( t.cab.idMsg == M_RESP_LLAMADA_SAM )
        { ar & AR_SERIALIZATION_M(t, mRespSetLlamadaSAM);  } ///< Tipo de mensaje para la respuesta de la activacion / desactivacion de la llamada SAM
        else if( t.cab.idMsg == M_SET_BUCLE_SAM )
        { ar & AR_SERIALIZATION_M(t, mSetBucle);  } ///< Tipo de mensaje para la peticion de la activacion / desactivacion del bucle de la UGA.
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Enums a strings:

    /** Declarado en otro sitio
struct EnumElemToString
{
    long lElem;
    const char * szName;
};
*/

    static EnumElemToString<E_TIP_MSG> g_aEnumElemToString_E_TIP_MSG[] =
    {
        { PETI, "PETI" },
        { RESP, "RESP" },
        { NOTIF, "NOTIF" },
        { (E_TIP_MSG)  ~0, NULL }
    };

    inline bool getEnumElemId( E_TIP_MSG & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_TIP_MSG> * it =  g_aEnumElemToString_E_TIP_MSG; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_TIP_MSG & elem )
    {
        for(const EnumElemToString<E_TIP_MSG> * it =  g_aEnumElemToString_E_TIP_MSG; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_TIP_MSG > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_TIP_MSG) / sizeof(* g_aEnumElemToString_E_TIP_MSG);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_TIP_MSG[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_ID_MSG> g_aEnumElemToString_E_ID_MSG[] =
    {
        { M_PRESENCIA, "M_PRESENCIA" },
        { M_FICH_CONFIG, "M_FICH_CONFIG" },
        { M_INF_FICH_CONFIG, "M_INF_FICH_CONFIG" },
        { M_ARRANQUE, "M_ARRANQUE" },
        { M_CONFIG_VALIDADA, "M_CONFIG_VALIDADA" },
        { M_ENVIA_TABLA, "M_ENVIA_TABLA" },
        { M_SECTORIZA, "M_SECTORIZA" },
        { M_DATETIME_SG, "M_DATETIME_SG" },
        { M_ESTADISTICAS_TF, "M_ESTADISTICAS_TF" },
        { M_ESTADISTICAS_RD, "M_ESTADISTICAS_RD" },
        { M_CONFIG_IMPLANTADA, "M_CONFIG_IMPLANTADA" },
        { M_SALIR_APLICACION, "M_SALIR_APLICACION" },
        { M_COMENZAR_APLICACION, "M_COMENZAR_APLICACION" },
        { M_ESTADO_CONFIG, "M_ESTADO_CONFIG" },
        { M_ENLACE_SACTA, "M_ENLACE_SACTA" },
        { M_INICIO_SESION, "M_INICIO_SESION" },
        { M_PRES_SAM, "M_PRES_SAM" },
        { M_ACK_FICHERO, "M_ACK_FICHERO" },
        { M_BATERIA_CANCELADA, "M_BATERIA_CANCELADA" },
        { M_AUDIO_SAM_POSICION, "M_AUDIO_SAM_POSICION" },
        { M_AUDIO_SAM_INTERFAZ, "M_AUDIO_SAM_INTERFAZ" },
        { M_RED_SAM, "M_RED_SAM" },
        { M_PRES_VRP, "M_PRES_VRP" },
        { M_PRES_ESTAD, "M_PRES_ESTAD" },
        { M_ESTADO_RED, "M_ESTADO_RED" },
        { M_MAX_SESIONES_SAM, "M_MAX_SESIONES_SAM" },
        { M_PORCENTAJE_AVANCE_SAM, "M_PORCENTAJE_AVANCE_SAM" },
        { M_INICIO_SESION_SAM, "M_INICIO_SESION_SAM" },
        { M_FIN_SESION_SAM, "M_FIN_SESION_SAM" },
        { M_LISTA_BATERIAS_SAM, "M_LISTA_BATERIAS_SAM" },
        { M_CONSULTA_BATERIA_SAM, "M_CONSULTA_BATERIA_SAM" },
        { M_CREACION_BATERIA_SAM, "M_CREACION_BATERIA_SAM" },
        { M_POSICION_BATERIA_SAM, "M_POSICION_BATERIA_SAM" },
        { M_INTERFAZ_BATERIA_SAM, "M_INTERFAZ_BATERIA_SAM" },
        { M_BORRADO_BATERIA_SAM, "M_BORRADO_BATERIA_SAM" },
        { M_RESULTADO_BATERIA_SAM, "M_RESULTADO_BATERIA_SAM" },
        { M_EJECUTA_BATERIA_SAM, "M_EJECUTA_BATERIA_SAM" },
        { M_CANCELA_BATERIA_SAM, "M_CANCELA_BATERIA_SAM" },
        { M_LISTA_MEDIDAS_RED, "M_LISTA_MEDIDAS_RED" },
        { M_EJECUTA_MEDIDAS_RED, "M_EJECUTA_MEDIDAS_RED" },
        { M_CANCELA_MEDIDAS_RED, "M_CANCELA_MEDIDAS_RED" },
        { M_CONSULTA_UMBRALES, "M_CONSULTA_UMBRALES" },
        { M_MODIFICA_UMBRALES, "M_MODIFICA_UMBRALES" },
        { M_CONSULTA_PARAMETROS, "M_CONSULTA_PARAMETROS" },
        { M_MODIFICA_PARAMETROS, "M_MODIFICA_PARAMETROS" },
        { M_RESP_ACTIVACION_PATRON, "M_RESP_ACTIVACION_PATRON" },
        { M_ACTIVACION_PATRON, "M_ACTIVACION_PATRON" },
        { M_POSICION_MEDICION_SAM, "M_POSICION_MEDICION_SAM" },
        { M_RESP_POSICION_MEDICION_SAM, "M_RESP_POSICION_MEDICION_SAM" },
        { M_INTERFAZ_MEDICION_SAM, "M_INTERFAZ_MEDICION_SAM" },
        { M_RESP_MEDICION_SAM, "M_RESP_MEDICION_SAM" },
        { M_SET_LLAMADA_SAM, "M_SET_LLAMADA_SAM" },
        { M_RESP_LLAMADA_SAM, "M_RESP_LLAMADA_SAM" },
        { M_SET_BUCLE_SAM, "M_SET_BUCLE_SAM" },
        { M_PRES_WATCHDOG_SAM, "M_PRES_WATCHDOG_SAM" },
        { (E_ID_MSG)  ~0, NULL }
    };

    inline bool getEnumElemId( E_ID_MSG & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_ID_MSG> * it =  g_aEnumElemToString_E_ID_MSG; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_ID_MSG & elem )
    {
        for(const EnumElemToString<E_ID_MSG> * it =  g_aEnumElemToString_E_ID_MSG; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_ID_MSG > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_ID_MSG) / sizeof(* g_aEnumElemToString_E_ID_MSG);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_ID_MSG[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_SUBID_MSG> g_aEnumElemToString_E_SUBID_MSG[] =
    {
        { M_PING, "M_PING" },
        { M_STOP, "M_STOP" },
        { (E_SUBID_MSG)  ~0, NULL }
    };

    inline bool getEnumElemId( E_SUBID_MSG & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_SUBID_MSG> * it =  g_aEnumElemToString_E_SUBID_MSG; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_SUBID_MSG & elem )
    {
        for(const EnumElemToString<E_SUBID_MSG> * it =  g_aEnumElemToString_E_SUBID_MSG; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_SUBID_MSG > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_SUBID_MSG) / sizeof(* g_aEnumElemToString_E_SUBID_MSG);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_SUBID_MSG[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_TIPO_EQUIPO> g_aEnumElemToString_E_TIPO_EQUIPO[] =
    {
        { EQ_NULO, "EQ_NULO" },
        { EQ_PUESTO, "EQ_PUESTO" },
        { EQ_GATEWAY, "EQ_GATEWAY" },
        { EQ_GESTOR_MC, "EQ_GESTOR_MC" },
        { EQ_SUPERV, "EQ_SUPERV" },
        { EQ_SUPERR, "EQ_SUPERR" },
        { EQ_EQUI_GEN, "EQ_EQUI_GEN" },
        { EQ_SAM, "EQ_SAM" },
        { EQ_VRP, "EQ_VRP" },
        { EQ_VRP_SACTA, "EQ_VRP_SACTA" },
        { EQ_PROXY, "EQ_PROXY" },
        { EQ_ESTAD, "EQ_ESTAD" },
        { EQ_WATCHDOG, "EQ_WATCHDOG" },
        { (E_TIPO_EQUIPO)  ~0, NULL }
    };

    inline bool getEnumElemId( E_TIPO_EQUIPO & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_TIPO_EQUIPO> * it =  g_aEnumElemToString_E_TIPO_EQUIPO; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_TIPO_EQUIPO & elem )
    {
        for(const EnumElemToString<E_TIPO_EQUIPO> * it =  g_aEnumElemToString_E_TIPO_EQUIPO; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_TIPO_EQUIPO > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_TIPO_EQUIPO) / sizeof(* g_aEnumElemToString_E_TIPO_EQUIPO);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_TIPO_EQUIPO[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_EST_OPERATIVO> g_aEnumElemToString_E_EST_OPERATIVO[] =
    {
        { E_NULO, "E_NULO" },
        { E_PRINCIPAL, "E_PRINCIPAL" },
        { E_RESERVA, "E_RESERVA" },
        { E_FALLO, "E_FALLO" },
        { (E_EST_OPERATIVO)  ~0, NULL }
    };

    inline bool getEnumElemId( E_EST_OPERATIVO & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_EST_OPERATIVO> * it =  g_aEnumElemToString_E_EST_OPERATIVO; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_EST_OPERATIVO & elem )
    {
        for(const EnumElemToString<E_EST_OPERATIVO> * it =  g_aEnumElemToString_E_EST_OPERATIVO; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_EST_OPERATIVO > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_EST_OPERATIVO) / sizeof(* g_aEnumElemToString_E_EST_OPERATIVO);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_EST_OPERATIVO[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_SINO> g_aEnumElemToString_E_SINO[] =
    {
        { NO, "NO" },
        { SI, "SI" },
        { (E_SINO)  ~0, NULL }
    };

    inline bool getEnumElemId( E_SINO & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_SINO> * it =  g_aEnumElemToString_E_SINO; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_SINO & elem )
    {
        for(const EnumElemToString<E_SINO> * it =  g_aEnumElemToString_E_SINO; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_SINO > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_SINO) / sizeof(* g_aEnumElemToString_E_SINO);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_SINO[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_TIPO_PROC> g_aEnumElemToString_E_TIPO_PROC[] =
    {
        { E_PROC_A, "E_PROC_A" },
        { E_PROC_B, "E_PROC_B" },
        { (E_TIPO_PROC)  ~0, NULL }
    };

    inline bool getEnumElemId( E_TIPO_PROC & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_TIPO_PROC> * it =  g_aEnumElemToString_E_TIPO_PROC; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_TIPO_PROC & elem )
    {
        for(const EnumElemToString<E_TIPO_PROC> * it =  g_aEnumElemToString_E_TIPO_PROC; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_TIPO_PROC > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_TIPO_PROC) / sizeof(* g_aEnumElemToString_E_TIPO_PROC);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_TIPO_PROC[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_MOTIVO_SECTORIZ> g_aEnumElemToString_E_MOTIVO_SECTORIZ[] =
    {
        { E_SECT_POR_ENVIO_CONFIG_NUEVA_TODOS, "E_SECT_POR_ENVIO_CONFIG_NUEVA_TODOS" },
        { E_SECT_POR_ENVIO_CONFIG_ARRANQUE_PUES_GW, "E_SECT_POR_ENVIO_CONFIG_ARRANQUE_PUES_GW" },
        { E_SECT_POR_SECT_LOCAL_TMCS, "E_SECT_POR_SECT_LOCAL_TMCS" },
        { E_SECT_POR_RECONF_UCS, "E_SECT_POR_RECONF_UCS" },
        { E_SECT_POR_SECT_SACTA, "E_SECT_POR_SECT_SACTA" },
        { E_NOSECT_POR_SECT_ERRONEA, "E_NOSECT_POR_SECT_ERRONEA" },
        { E_NOSECT_POR_SECT_EN_PROCESO, "E_NOSECT_POR_SECT_EN_PROCESO" },
        { E_NOSECT_POR_CONFIG_EN_PROCESO, "E_NOSECT_POR_CONFIG_EN_PROCESO" },
        { E_NOSECT_POR_SAM_SECTORIZADA, "E_NOSECT_POR_SAM_SECTORIZADA" },
        { E_SECT_SAM, "E_SECT_SAM" },
        { E_SECT_POR_RECONF_UCS_JACKS, "E_SECT_POR_RECONF_UCS_JACKS" },
        { (E_MOTIVO_SECTORIZ)  ~0, NULL }
    };

    inline bool getEnumElemId( E_MOTIVO_SECTORIZ & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_MOTIVO_SECTORIZ> * it =  g_aEnumElemToString_E_MOTIVO_SECTORIZ; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_MOTIVO_SECTORIZ & elem )
    {
        for(const EnumElemToString<E_MOTIVO_SECTORIZ> * it =  g_aEnumElemToString_E_MOTIVO_SECTORIZ; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_MOTIVO_SECTORIZ > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_MOTIVO_SECTORIZ) / sizeof(* g_aEnumElemToString_E_MOTIVO_SECTORIZ);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_MOTIVO_SECTORIZ[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_SUB_ID_MSJ_TLF> g_aEnumElemToString_E_SUB_ID_MSJ_TLF[] =
    {
        { COMUNICACION, "COMUNICACION" },
        { INTERVENCION, "INTERVENCION" },
        { REDIAL, "REDIAL" },
        { PRIO, "PRIO" },
        { RELL_AUTO, "RELL_AUTO" },
        { CAPT_LLAM, "CAPT_LLAM" },
        { REDIR, "REDIR" },
        { TRANSFER_LLAM, "TRANSFER_LLAM" },
        { (E_SUB_ID_MSJ_TLF)  ~0, NULL }
    };

    inline bool getEnumElemId( E_SUB_ID_MSJ_TLF & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_SUB_ID_MSJ_TLF> * it =  g_aEnumElemToString_E_SUB_ID_MSJ_TLF; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_SUB_ID_MSJ_TLF & elem )
    {
        for(const EnumElemToString<E_SUB_ID_MSJ_TLF> * it =  g_aEnumElemToString_E_SUB_ID_MSJ_TLF; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_SUB_ID_MSJ_TLF > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_SUB_ID_MSJ_TLF) / sizeof(* g_aEnumElemToString_E_SUB_ID_MSJ_TLF);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_SUB_ID_MSJ_TLF[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_SUB_ID_MSJ_RAD> g_aEnumElemToString_E_SUB_ID_MSJ_RAD[] =
    {
        { PULSACION_PTT, "PULSACION_PTT" },
        { SQUELCH, "SQUELCH" },
        { SELECCION_TX, "SELECCION_TX" },
        { SELECCION_RX, "SELECCION_RX" },
        { FACILIDAD_GRUPO_RTX, "FACILIDAD_GRUPO_RTX" },
        { FACILIDAD_CASCOS_ALTAVOZ, "FACILIDAD_CASCOS_ALTAVOZ" },
        { FACILIDAD_CAMBIO_EMPLAZAMIENTO, "FACILIDAD_CAMBIO_EMPLAZAMIENTO" },
        { FACILIDAD_INB_CASCOS_ALTAVOZ, "FACILIDAD_INB_CASCOS_ALTAVOZ" },
        { FACILIDAD_ULTIMA_LLAMADA_RADIO, "FACILIDAD_ULTIMA_LLAMADA_RADIO" },
        { FACILIDAD_BSS_OVER, "FACILIDAD_BSS_OVER" },
        { FACILIDAD_CLIMAX, "FACILIDAD_CLIMAX" },
        { RECEPECION_AUDIO_EN_POSICION, "RECEPECION_AUDIO_EN_POSICION" },
        { CALIDAD_BSS, "CALIDAD_BSS" },
        { FACILIDAD_ANNADIR_FREC, "FACILIDAD_ANNADIR_FREC" },
        { FACILIDAD_BORRAR_FREC, "FACILIDAD_BORRAR_FREC" },
        { FACILIDAD_CAMBIO_MAIN_STANDBY, "FACILIDAD_CAMBIO_MAIN_STANDBY" },
        { FACILIDAD_CAMBIO_PAGINA_RADIO, "FACILIDAD_CAMBIO_PAGINA_RADIO" },
        { FACILIDAD_SIDETONE, "FACILIDAD_SIDETONE" },
        { FACILIDAD_JACKS_CONT, "FACILIDAD_JACKS_CONT" },
        { FACILIDAD_JACKS_COORD, "FACILIDAD_JACKS_COORD" },
        { CAPT_COORD_RADIO, "CAPT_COORD_RADIO" },
        { (E_SUB_ID_MSJ_RAD)  ~0, NULL }
    };

    inline bool getEnumElemId( E_SUB_ID_MSJ_RAD & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_SUB_ID_MSJ_RAD> * it =  g_aEnumElemToString_E_SUB_ID_MSJ_RAD; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_SUB_ID_MSJ_RAD & elem )
    {
        for(const EnumElemToString<E_SUB_ID_MSJ_RAD> * it =  g_aEnumElemToString_E_SUB_ID_MSJ_RAD; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_SUB_ID_MSJ_RAD > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_SUB_ID_MSJ_RAD) / sizeof(* g_aEnumElemToString_E_SUB_ID_MSJ_RAD);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_SUB_ID_MSJ_RAD[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_SISTEMA_ABC> g_aEnumElemToString_E_SISTEMA_ABC[] =
    {
        { SISTEMA_A, "SISTEMA_A" },
        { SISTEMA_B, "SISTEMA_B" },
        { SISTEMA_AYB, "SISTEMA_AYB" },
        { (E_SISTEMA_ABC)  ~0, NULL }
    };

    inline bool getEnumElemId( E_SISTEMA_ABC & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_SISTEMA_ABC> * it =  g_aEnumElemToString_E_SISTEMA_ABC; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_SISTEMA_ABC & elem )
    {
        for(const EnumElemToString<E_SISTEMA_ABC> * it =  g_aEnumElemToString_E_SISTEMA_ABC; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_SISTEMA_ABC > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_SISTEMA_ABC) / sizeof(* g_aEnumElemToString_E_SISTEMA_ABC);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_SISTEMA_ABC[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_TIPO_EQUIPO_EST> g_aEnumElemToString_E_TIPO_EQUIPO_EST[] =
    {
        { EQ_EST_NULO, "EQ_EST_NULO" },
        { EQ_EST_PUESTO, "EQ_EST_PUESTO" },
        { EQ_EST_GATEWAY, "EQ_EST_GATEWAY" },
        { EQ_EST_GESTOR_MC, "EQ_EST_GESTOR_MC" },
        { EQ_EST_SUPERV, "EQ_EST_SUPERV" },
        { EQ_EST_EQUI_GEN, "EQ_EST_EQUI_GEN" },
        { (E_TIPO_EQUIPO_EST)  ~0, NULL }
    };

    inline bool getEnumElemId( E_TIPO_EQUIPO_EST & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_TIPO_EQUIPO_EST> * it =  g_aEnumElemToString_E_TIPO_EQUIPO_EST; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_TIPO_EQUIPO_EST & elem )
    {
        for(const EnumElemToString<E_TIPO_EQUIPO_EST> * it =  g_aEnumElemToString_E_TIPO_EQUIPO_EST; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_TIPO_EQUIPO_EST > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_TIPO_EQUIPO_EST) / sizeof(* g_aEnumElemToString_E_TIPO_EQUIPO_EST);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_TIPO_EQUIPO_EST[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_TIPO_PUESTO> g_aEnumElemToString_E_TIPO_PUESTO[] =
    {
        { PUESTO_APOYO, "PUESTO_APOYO" },
        { TELEFONO_IP, "TELEFONO_IP" },
        { NO_PUESTO, "NO_PUESTO" },
        { PUESTO_EJ, "PUESTO_EJ" },
        { PUESTO_PL, "PUESTO_PL" },
        { (E_TIPO_PUESTO)  ~0, NULL }
    };

    inline bool getEnumElemId( E_TIPO_PUESTO & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_TIPO_PUESTO> * it =  g_aEnumElemToString_E_TIPO_PUESTO; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_TIPO_PUESTO & elem )
    {
        for(const EnumElemToString<E_TIPO_PUESTO> * it =  g_aEnumElemToString_E_TIPO_PUESTO; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_TIPO_PUESTO > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_TIPO_PUESTO) / sizeof(* g_aEnumElemToString_E_TIPO_PUESTO);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_TIPO_PUESTO[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_TIPO_GATEWAY> g_aEnumElemToString_E_TIPO_GATEWAY[] =
    {
        { GW_NOIDEN, "GW_NOIDEN" },
        { GW_INDRA, "GW_INDRA" },
        { GW_OTRO, "GW_OTRO" },
        { GW_POSICION, "GW_POSICION" },
        { (E_TIPO_GATEWAY)  ~0, NULL }
    };

    inline bool getEnumElemId( E_TIPO_GATEWAY & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_TIPO_GATEWAY> * it =  g_aEnumElemToString_E_TIPO_GATEWAY; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_TIPO_GATEWAY & elem )
    {
        for(const EnumElemToString<E_TIPO_GATEWAY> * it =  g_aEnumElemToString_E_TIPO_GATEWAY; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_TIPO_GATEWAY > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_TIPO_GATEWAY) / sizeof(* g_aEnumElemToString_E_TIPO_GATEWAY);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_TIPO_GATEWAY[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_TIPO_ABONADO_INTERNO> g_aEnumElemToString_E_TIPO_ABONADO_INTERNO[] =
    {
        { ABO_APOYO, "ABO_APOYO" },
        { ABO_PILOTO, "ABO_PILOTO" },
        { ABO_TELEFONO, "ABO_TELEFONO" },
        { ABO_EJECUTIVO, "ABO_EJECUTIVO" },
        { ABO_PLANIFICADOR, "ABO_PLANIFICADOR" },
        { NO_ABONADO, "NO_ABONADO" },
        { (E_TIPO_ABONADO_INTERNO)  ~0, NULL }
    };

    inline bool getEnumElemId( E_TIPO_ABONADO_INTERNO & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_TIPO_ABONADO_INTERNO> * it =  g_aEnumElemToString_E_TIPO_ABONADO_INTERNO; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_TIPO_ABONADO_INTERNO & elem )
    {
        for(const EnumElemToString<E_TIPO_ABONADO_INTERNO> * it =  g_aEnumElemToString_E_TIPO_ABONADO_INTERNO; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_TIPO_ABONADO_INTERNO > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_TIPO_ABONADO_INTERNO) / sizeof(* g_aEnumElemToString_E_TIPO_ABONADO_INTERNO);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_TIPO_ABONADO_INTERNO[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_ESTADO_ENLACE> g_aEnumElemToString_E_ESTADO_ENLACE[] =
    {
        { M_C, "M_C" },
        { M_U, "M_U" },
        { PROTOCOLO_OK, "PROTOCOLO_OK" },
        { PROTOCOLO_MAL, "PROTOCOLO_MAL" },
        { IFGW_TINEX, "IFGW_TINEX" },
        { (E_ESTADO_ENLACE)  ~0, NULL }
    };

    inline bool getEnumElemId( E_ESTADO_ENLACE & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_ESTADO_ENLACE> * it =  g_aEnumElemToString_E_ESTADO_ENLACE; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_ESTADO_ENLACE & elem )
    {
        for(const EnumElemToString<E_ESTADO_ENLACE> * it =  g_aEnumElemToString_E_ESTADO_ENLACE; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_ESTADO_ENLACE > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_ESTADO_ENLACE) / sizeof(* g_aEnumElemToString_E_ESTADO_ENLACE);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_ESTADO_ENLACE[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_TIPO_ENLACE> g_aEnumElemToString_E_TIPO_ENLACE[] =
    {
        { EYMDTMF, "EYMDTMF" },
        { LCN, "LCN" },
        { CR, "CR" },
        { N5, "N5" },
        { EYM, "EYM" },
        { CRI, "CRI" },
        { LCE, "LCE" },
        { BL, "BL" },
        { PABX, "PABX" },
        { R2, "R2" },
        { QSIG, "QSIG" },
        { TFU, "TFU" },
        { VACIO, "VACIO" },
        { ISDN_ST, "ISDN_ST" },
        { E1, "E1" },
        { BRI, "BRI" },
        { FXO, "FXO" },
        { FXS, "FXS" },
        { LC_AUX1, "LC_AUX1" },
        { CR_AUX1, "CR_AUX1" },
        { CR_BSS, "CR_BSS" },
        { NO_ENL, "NO_ENL" },
        { (E_TIPO_ENLACE)  ~0, NULL }
    };

    inline bool getEnumElemId( E_TIPO_ENLACE & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_TIPO_ENLACE> * it =  g_aEnumElemToString_E_TIPO_ENLACE; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_TIPO_ENLACE & elem )
    {
        for(const EnumElemToString<E_TIPO_ENLACE> * it =  g_aEnumElemToString_E_TIPO_ENLACE; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_TIPO_ENLACE > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_TIPO_ENLACE) / sizeof(* g_aEnumElemToString_E_TIPO_ENLACE);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_TIPO_ENLACE[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_EST_TF> g_aEnumElemToString_E_EST_TF[] =
    {
        { E_TF_SINESTADO, "E_TF_SINESTADO" },
        { E_TF_NULL, "E_TF_NULL" },
        { E_TF_INICIAL, "E_TF_INICIAL" },
        { E_TF_LLAMADO, "E_TF_LLAMADO" },
        { E_TF_CONECTANDO, "E_TF_CONECTANDO" },
        { E_TF_LLAMANDO, "E_TF_LLAMANDO" },
        { E_TF_CONVERSACION, "E_TF_CONVERSACION" },
        { E_TF_RETENIDA, "E_TF_RETENIDA" },
        { E_TF_HOLD, "E_TF_HOLD" },
        { E_TF_TRANSITO, "E_TF_TRANSITO" },
        { E_TF_ESCUCHANDO, "E_TF_ESCUCHANDO" },
        { E_TF_ESCUCHADO, "E_TF_ESCUCHADO" },
        { E_TF_PARK_ACT, "E_TF_PARK_ACT" },
        { E_TF_PARK_PAS, "E_TF_PARK_PAS" },
        { E_TF_INTERVENIDO, "E_TF_INTERVENIDO" },
        { E_TF_ANULANDO, "E_TF_ANULANDO" },
        { E_TF_ANULADO, "E_TF_ANULADO" },
        { E_TF_INTRUA, "E_TF_INTRUA" },
        { E_TF_INTRUP, "E_TF_INTRUP" },
        { E_TF_LLACON, "E_TF_LLACON" },
        { E_TF_CONCON, "E_TF_CONCON" },
        { E_TF_OCUPADO, "E_TF_OCUPADO" },
        { E_TF_INTERVENCION, "E_TF_INTERVENCION" },
        { E_TF_INTVDO, "E_TF_INTVDO" },
        { E_TF_FACIL_ACTIVA, "E_TF_FACIL_ACTIVA" },
        { E_TF_REDIR_ACTIVA, "E_TF_REDIR_ACTIVA" },
        { E_TF_REDIR_PASIVA, "E_TF_REDIR_PASIVA" },
        { E_TF_CI_CIPL_A, "E_TF_CI_CIPL_A" },
        { E_TF_CI_CIPL_P, "E_TF_CI_CIPL_P" },
        { E_TF_CI_NOTI_A, "E_TF_CI_NOTI_A" },
        { E_TF_CI_NOTI_P, "E_TF_CI_NOTI_P" },
        { E_TF_CI_CONV_A, "E_TF_CI_CONV_A" },
        { E_TF_CI_CONV_B, "E_TF_CI_CONV_B" },
        { E_TF_PET_CIPL, "E_TF_PET_CIPL" },
        { E_TF_IA_TRANSTXLC, "E_TF_IA_TRANSTXLC" },
        { E_TF_IA_TXLC, "E_TF_IA_TXLC" },
        { E_TF_IA_RXLC, "E_TF_IA_RXLC" },
        { (E_EST_TF)  ~0, NULL }
    };

    inline bool getEnumElemId( E_EST_TF & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_EST_TF> * it =  g_aEnumElemToString_E_EST_TF; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_EST_TF & elem )
    {
        for(const EnumElemToString<E_EST_TF> * it =  g_aEnumElemToString_E_EST_TF; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_EST_TF > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_EST_TF) / sizeof(* g_aEnumElemToString_E_EST_TF);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_EST_TF[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_SUBESTADO> g_aEnumElemToString_E_SUBESTADO[] =
    {
        { NINGUNO, "NINGUNO" },
        { OCUP, "OCUP" },
        { CONG, "CONG" },
        { FS, "FS" },
        { RECH, "RECH" },
        { INTV, "INTV" },
        { RELLAMADA, "RELLAMADA" },
        { OCU_EXT, "OCU_EXT" },
        { CCAN_ATS, "CCAN_ATS" },
        { REEN_ATS, "REEN_ATS" },
        { PROG_ATS, "PROG_ATS" },
        { ALERT_ATS, "ALERT_ATS" },
        { INTRUS_ATS, "INTRUS_ATS" },
        { EXPCIT5_ATS, "EXPCIT5_ATS" },
        { INTRUS_FAIL, "INTRUS_FAIL" },
        { INTRUS_COMP, "INTRUS_COMP" },
        { TXLC, "TXLC" },
        { RXLC, "RXLC" },
        { OCUTXLC, "OCUTXLC" },
        { ANUTXLC, "ANUTXLC" },
        { ANURXLC, "ANURXLC" },
        { OCURXLC, "OCURXLC" },
        { NOTECLC, "NOTECLC" },
        { (E_SUBESTADO)  ~0, NULL }
    };

    inline bool getEnumElemId( E_SUBESTADO & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_SUBESTADO> * it =  g_aEnumElemToString_E_SUBESTADO; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_SUBESTADO & elem )
    {
        for(const EnumElemToString<E_SUBESTADO> * it =  g_aEnumElemToString_E_SUBESTADO; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_SUBESTADO > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_SUBESTADO) / sizeof(* g_aEnumElemToString_E_SUBESTADO);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_SUBESTADO[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_SERVICIO> g_aEnumElemToString_E_SERVICIO[] =
    {
        { S_TLF, "S_TLF" },
        { S_LC, "S_LC" },
        { S_AD, "S_AD" },
        { S_AI, "S_AI" },
        { (E_SERVICIO)  ~0, NULL }
    };

    inline bool getEnumElemId( E_SERVICIO & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_SERVICIO> * it =  g_aEnumElemToString_E_SERVICIO; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_SERVICIO & elem )
    {
        for(const EnumElemToString<E_SERVICIO> * it =  g_aEnumElemToString_E_SERVICIO; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_SERVICIO > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_SERVICIO) / sizeof(* g_aEnumElemToString_E_SERVICIO);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_SERVICIO[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_TIPO_PRIORIDAD_R2> g_aEnumElemToString_E_TIPO_PRIORIDAD_R2[] =
    {
        { PRIORIDAD_PRIO, "PRIORIDAD_PRIO" },
        { PRIORIDAD_ALTA, "PRIORIDAD_ALTA" },
        { PRIORIDAD_MEDIA, "PRIORIDAD_MEDIA" },
        { PRIORIDAD_BAJA, "PRIORIDAD_BAJA" },
        { PRIORIDAD_PRIO_TRANS, "PRIORIDAD_PRIO_TRANS" },
        { PRIORIDAD_ALTA_TRANS, "PRIORIDAD_ALTA_TRANS" },
        { PRIORIDAD_MEDIA_TRANS, "PRIORIDAD_MEDIA_TRANS" },
        { PRIORIDAD_BAJA_TRANS, "PRIORIDAD_BAJA_TRANS" },
        { (E_TIPO_PRIORIDAD_R2)  ~0, NULL }
    };

    inline bool getEnumElemId( E_TIPO_PRIORIDAD_R2 & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_TIPO_PRIORIDAD_R2> * it =  g_aEnumElemToString_E_TIPO_PRIORIDAD_R2; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_TIPO_PRIORIDAD_R2 & elem )
    {
        for(const EnumElemToString<E_TIPO_PRIORIDAD_R2> * it =  g_aEnumElemToString_E_TIPO_PRIORIDAD_R2; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_TIPO_PRIORIDAD_R2 > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_TIPO_PRIORIDAD_R2) / sizeof(* g_aEnumElemToString_E_TIPO_PRIORIDAD_R2);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_TIPO_PRIORIDAD_R2[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_TIPO_RUTA_EST> g_aEnumElemToString_E_TIPO_RUTA_EST[] =
    {
        { SIN_RUTA_EST, "SIN_RUTA_EST" },
        { RUTA_LOCAL_EST, "RUTA_LOCAL_EST" },
        { RUTA_ATSDIRECTA_EST, "RUTA_ATSDIRECTA_EST" },
        { RUTA_ATSALTERNATIVA_EST, "RUTA_ATSALTERNATIVA_EST" },
        { RUTA_TRANSITO_EST, "RUTA_TRANSITO_EST" },
        { (E_TIPO_RUTA_EST)  ~0, NULL }
    };

    inline bool getEnumElemId( E_TIPO_RUTA_EST & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_TIPO_RUTA_EST> * it =  g_aEnumElemToString_E_TIPO_RUTA_EST; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_TIPO_RUTA_EST & elem )
    {
        for(const EnumElemToString<E_TIPO_RUTA_EST> * it =  g_aEnumElemToString_E_TIPO_RUTA_EST; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_TIPO_RUTA_EST > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_TIPO_RUTA_EST) / sizeof(* g_aEnumElemToString_E_TIPO_RUTA_EST);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_TIPO_RUTA_EST[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_ESTADO_CONFIG> g_aEnumElemToString_E_ESTADO_CONFIG[] =
    {
        { E_ESTADO_NO_VALIDADA, "E_ESTADO_NO_VALIDADA" },
        { E_ESTADO_VALIDADA, "E_ESTADO_VALIDADA" },
        { E_ESTADO_VERSIONADA, "E_ESTADO_VERSIONADA" },
        { E_ESTADO_BORRADA, "E_ESTADO_BORRADA" },
        { (E_ESTADO_CONFIG)  ~0, NULL }
    };

    inline bool getEnumElemId( E_ESTADO_CONFIG & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_ESTADO_CONFIG> * it =  g_aEnumElemToString_E_ESTADO_CONFIG; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_ESTADO_CONFIG & elem )
    {
        for(const EnumElemToString<E_ESTADO_CONFIG> * it =  g_aEnumElemToString_E_ESTADO_CONFIG; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_ESTADO_CONFIG > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_ESTADO_CONFIG) / sizeof(* g_aEnumElemToString_E_ESTADO_CONFIG);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_ESTADO_CONFIG[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_PROCEDENCIA_SECTORIZ> g_aEnumElemToString_E_PROCEDENCIA_SECTORIZ[] =
    {
        { E_SECT_LOCAL, "E_SECT_LOCAL" },
        { E_SECT_REMOTA, "E_SECT_REMOTA" },
        { (E_PROCEDENCIA_SECTORIZ)  ~0, NULL }
    };

    inline bool getEnumElemId( E_PROCEDENCIA_SECTORIZ & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_PROCEDENCIA_SECTORIZ> * it =  g_aEnumElemToString_E_PROCEDENCIA_SECTORIZ; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_PROCEDENCIA_SECTORIZ & elem )
    {
        for(const EnumElemToString<E_PROCEDENCIA_SECTORIZ> * it =  g_aEnumElemToString_E_PROCEDENCIA_SECTORIZ; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_PROCEDENCIA_SECTORIZ > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_PROCEDENCIA_SECTORIZ) / sizeof(* g_aEnumElemToString_E_PROCEDENCIA_SECTORIZ);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_PROCEDENCIA_SECTORIZ[idx];
        }

        return nElems;
    }

    static EnumElemToString<TIPO_TMCS> g_aEnumElemToString_TIPO_TMCS[] =
    {
        { TIPO_CLIENTE, "TIPO_CLIENTE" },
        { TIPO_SERVIDOR, "TIPO_SERVIDOR" },
        { (TIPO_TMCS)  ~0, NULL }
    };

    inline bool getEnumElemId( TIPO_TMCS & elem, const char * szElemName)
    {
        for(const EnumElemToString<TIPO_TMCS> * it =  g_aEnumElemToString_TIPO_TMCS; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const TIPO_TMCS & elem )
    {
        for(const EnumElemToString<TIPO_TMCS> * it =  g_aEnumElemToString_TIPO_TMCS; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  TIPO_TMCS > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_TIPO_TMCS) / sizeof(* g_aEnumElemToString_TIPO_TMCS);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_TIPO_TMCS[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_ESTADO_ENLACE_SACTA> g_aEnumElemToString_E_ESTADO_ENLACE_SACTA[] =
    {
        { E_ESTADO_ENLACE_SACTA_CONECTADO, "E_ESTADO_ENLACE_SACTA_CONECTADO" },
        { E_ESTADO_ENLACE_SACTA_DESCONECTADO, "E_ESTADO_ENLACE_SACTA_DESCONECTADO" },
        { (E_ESTADO_ENLACE_SACTA)  ~0, NULL }
    };

    inline bool getEnumElemId( E_ESTADO_ENLACE_SACTA & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_ESTADO_ENLACE_SACTA> * it =  g_aEnumElemToString_E_ESTADO_ENLACE_SACTA; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_ESTADO_ENLACE_SACTA & elem )
    {
        for(const EnumElemToString<E_ESTADO_ENLACE_SACTA> * it =  g_aEnumElemToString_E_ESTADO_ENLACE_SACTA; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_ESTADO_ENLACE_SACTA > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_ESTADO_ENLACE_SACTA) / sizeof(* g_aEnumElemToString_E_ESTADO_ENLACE_SACTA);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_ESTADO_ENLACE_SACTA[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_ESTADO_LAN> g_aEnumElemToString_E_ESTADO_LAN[] =
    {
        { E_ESTADO_LAN_CONECTADO, "E_ESTADO_LAN_CONECTADO" },
        { E_ESTADO_LAN_DESCONECTADO, "E_ESTADO_LAN_DESCONECTADO" },
        { (E_ESTADO_LAN)  ~0, NULL }
    };

    inline bool getEnumElemId( E_ESTADO_LAN & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_ESTADO_LAN> * it =  g_aEnumElemToString_E_ESTADO_LAN; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_ESTADO_LAN & elem )
    {
        for(const EnumElemToString<E_ESTADO_LAN> * it =  g_aEnumElemToString_E_ESTADO_LAN; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_ESTADO_LAN > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_ESTADO_LAN) / sizeof(* g_aEnumElemToString_E_ESTADO_LAN);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_ESTADO_LAN[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_MODO_SAM> g_aEnumElemToString_E_MODO_SAM[] =
    {
        { MED_AUDIO, "MED_AUDIO" },
        { MED_RED, "MED_RED" },
        { REPOSO, "REPOSO" },
        { (E_MODO_SAM)  ~0, NULL }
    };

    inline bool getEnumElemId( E_MODO_SAM & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_MODO_SAM> * it =  g_aEnumElemToString_E_MODO_SAM; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_MODO_SAM & elem )
    {
        for(const EnumElemToString<E_MODO_SAM> * it =  g_aEnumElemToString_E_MODO_SAM; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_MODO_SAM > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_MODO_SAM) / sizeof(* g_aEnumElemToString_E_MODO_SAM);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_MODO_SAM[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_TIPO_AUDIO_RED> g_aEnumElemToString_E_TIPO_AUDIO_RED[] =
    {
        { TIPO_AUDIO, "TIPO_AUDIO" },
        { TIPO_RED, "TIPO_RED" },
        { (E_TIPO_AUDIO_RED)  ~0, NULL }
    };

    inline bool getEnumElemId( E_TIPO_AUDIO_RED & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_TIPO_AUDIO_RED> * it =  g_aEnumElemToString_E_TIPO_AUDIO_RED; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_TIPO_AUDIO_RED & elem )
    {
        for(const EnumElemToString<E_TIPO_AUDIO_RED> * it =  g_aEnumElemToString_E_TIPO_AUDIO_RED; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_TIPO_AUDIO_RED > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_TIPO_AUDIO_RED) / sizeof(* g_aEnumElemToString_E_TIPO_AUDIO_RED);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_TIPO_AUDIO_RED[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_VALOR_OPTIMO> g_aEnumElemToString_E_VALOR_OPTIMO[] =
    {
        { MINIMO, "MINIMO" },
        { MAXIMO, "MAXIMO" },
        { (E_VALOR_OPTIMO)  ~0, NULL }
    };

    inline bool getEnumElemId( E_VALOR_OPTIMO & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_VALOR_OPTIMO> * it =  g_aEnumElemToString_E_VALOR_OPTIMO; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_VALOR_OPTIMO & elem )
    {
        for(const EnumElemToString<E_VALOR_OPTIMO> * it =  g_aEnumElemToString_E_VALOR_OPTIMO; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_VALOR_OPTIMO > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_VALOR_OPTIMO) / sizeof(* g_aEnumElemToString_E_VALOR_OPTIMO);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_VALOR_OPTIMO[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_TIPO_CR> g_aEnumElemToString_E_TIPO_CR[] =
    {
        { MSK_RX, "MSK_RX" },
        { MSK_SQ, "MSK_SQ" },
        { MSK_TX, "MSK_TX" },
        { MSK_MTX, "MSK_MTX" },
        { MSK_PTT, "MSK_PTT" },
        { MSK_RTX, "MSK_RTX" },
        { MSK_PTT_FICT, "MSK_PTT_FICT" },
        { (E_TIPO_CR)  ~0, NULL }
    };

    inline bool getEnumElemId( E_TIPO_CR & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_TIPO_CR> * it =  g_aEnumElemToString_E_TIPO_CR; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_TIPO_CR & elem )
    {
        for(const EnumElemToString<E_TIPO_CR> * it =  g_aEnumElemToString_E_TIPO_CR; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_TIPO_CR > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_TIPO_CR) / sizeof(* g_aEnumElemToString_E_TIPO_CR);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_TIPO_CR[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_RESULTADO> g_aEnumElemToString_E_RESULTADO[] =
    {
        { E_RESULTADO_OPTIMO, "E_RESULTADO_OPTIMO" },
        { E_RESULTADO_BUENO, "E_RESULTADO_BUENO" },
        { E_RESULTADO_MALO, "E_RESULTADO_MALO" },
        { E_RESULTADO_NO_RES, "E_RESULTADO_NO_RES" },
        { (E_RESULTADO)  ~0, NULL }
    };

    inline bool getEnumElemId( E_RESULTADO & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_RESULTADO> * it =  g_aEnumElemToString_E_RESULTADO; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_RESULTADO & elem )
    {
        for(const EnumElemToString<E_RESULTADO> * it =  g_aEnumElemToString_E_RESULTADO; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_RESULTADO > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_RESULTADO) / sizeof(* g_aEnumElemToString_E_RESULTADO);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_RESULTADO[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_RES_MEDIDA> g_aEnumElemToString_E_RES_MEDIDA[] =
    {
        { E_RES_MEDIDA_OPTIMO, "E_RES_MEDIDA_OPTIMO" },
        { E_RES_MEDIDA_BUENO, "E_RES_MEDIDA_BUENO" },
        { E_RES_MEDIDA_MALO, "E_RES_MEDIDA_MALO" },
        { E_RES_MEDIDA_NO_RES, "E_RES_MEDIDA_NO_RES" },
        { (E_RES_MEDIDA)  ~0, NULL }
    };

    inline bool getEnumElemId( E_RES_MEDIDA & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_RES_MEDIDA> * it =  g_aEnumElemToString_E_RES_MEDIDA; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_RES_MEDIDA & elem )
    {
        for(const EnumElemToString<E_RES_MEDIDA> * it =  g_aEnumElemToString_E_RES_MEDIDA; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_RES_MEDIDA > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_RES_MEDIDA) / sizeof(* g_aEnumElemToString_E_RES_MEDIDA);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_RES_MEDIDA[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_ID_CANAL_BUCLE> g_aEnumElemToString_E_ID_CANAL_BUCLE[] =
    {
        { CANAL_1, "CANAL_1" },
        { CANAL_2, "CANAL_2" },
        { CANAL_3, "CANAL_3" },
        { CASCOS_1, "CASCOS_1" },
        { CASCOS_2, "CASCOS_2" },
        { ALTAVOZ_RAD, "ALTAVOZ_RAD" },
        { ALTAVOZ_AI, "ALTAVOZ_AI" },
        { MICRO_1, "MICRO_1" },
        { MICRO_2, "MICRO_2" },
        { (E_ID_CANAL_BUCLE)  ~0, NULL }
    };

    inline bool getEnumElemId( E_ID_CANAL_BUCLE & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_ID_CANAL_BUCLE> * it =  g_aEnumElemToString_E_ID_CANAL_BUCLE; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_ID_CANAL_BUCLE & elem )
    {
        for(const EnumElemToString<E_ID_CANAL_BUCLE> * it =  g_aEnumElemToString_E_ID_CANAL_BUCLE; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_ID_CANAL_BUCLE > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_ID_CANAL_BUCLE) / sizeof(* g_aEnumElemToString_E_ID_CANAL_BUCLE);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_ID_CANAL_BUCLE[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_TIPO_MEDIDA> g_aEnumElemToString_E_TIPO_MEDIDA[] =
    {
        { ATENUACION, "ATENUACION" },
        { RUIDO, "RUIDO" },
        { DISTORSION, "DISTORSION" },
        { RETARDO_INTERNO, "RETARDO_INTERNO" },
        { RETARDO_EXTERNO, "RETARDO_EXTERNO" },
        { PESQ, "PESQ" },
        { TAM_RECORTES, "TAM_RECORTES" },
        { NUM_RECORTES, "NUM_RECORTES" },
        { ECO, "ECO" },
        { DIAFONIA, "DIAFONIA" },
        { IPDV_ANALOGICO, "IPDV_ANALOGICO" },
        { IPDV_DIGITAL, "IPDV_DIGITAL" },
        { PAQUETES_PERDIDOS, "PAQUETES_PERDIDOS" },
        { (E_TIPO_MEDIDA)  ~0, NULL }
    };

    inline bool getEnumElemId( E_TIPO_MEDIDA & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_TIPO_MEDIDA> * it =  g_aEnumElemToString_E_TIPO_MEDIDA; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_TIPO_MEDIDA & elem )
    {
        for(const EnumElemToString<E_TIPO_MEDIDA> * it =  g_aEnumElemToString_E_TIPO_MEDIDA; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_TIPO_MEDIDA > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_TIPO_MEDIDA) / sizeof(* g_aEnumElemToString_E_TIPO_MEDIDA);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_TIPO_MEDIDA[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_ESTADO_FTP> g_aEnumElemToString_E_ESTADO_FTP[] =
    {
        { E_ESTADO_FTP_ACTIVO, "E_ESTADO_FTP_ACTIVO" },
        { E_ESTADO_FTP_NO_ACTIVO, "E_ESTADO_FTP_NO_ACTIVO" },
        { (E_ESTADO_FTP)  ~0, NULL }
    };

    inline bool getEnumElemId( E_ESTADO_FTP & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_ESTADO_FTP> * it =  g_aEnumElemToString_E_ESTADO_FTP; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_ESTADO_FTP & elem )
    {
        for(const EnumElemToString<E_ESTADO_FTP> * it =  g_aEnumElemToString_E_ESTADO_FTP; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_ESTADO_FTP > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_ESTADO_FTP) / sizeof(* g_aEnumElemToString_E_ESTADO_FTP);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_ESTADO_FTP[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_ESTADO_GENERAL> g_aEnumElemToString_E_ESTADO_GENERAL[] =
    {
        { E_ESTADO_GENERAL_ACTIVO, "E_ESTADO_GENERAL_ACTIVO" },
        { E_ESTADO_GENERAL_NO_ACTIVO, "E_ESTADO_GENERAL_NO_ACTIVO" },
        { (E_ESTADO_GENERAL)  ~0, NULL }
    };

    inline bool getEnumElemId( E_ESTADO_GENERAL & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_ESTADO_GENERAL> * it =  g_aEnumElemToString_E_ESTADO_GENERAL; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_ESTADO_GENERAL & elem )
    {
        for(const EnumElemToString<E_ESTADO_GENERAL> * it =  g_aEnumElemToString_E_ESTADO_GENERAL; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_ESTADO_GENERAL > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_ESTADO_GENERAL) / sizeof(* g_aEnumElemToString_E_ESTADO_GENERAL);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_ESTADO_GENERAL[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_ESTADO_CLUSTER> g_aEnumElemToString_E_ESTADO_CLUSTER[] =
    {
        { E_ESTADO_CLUSTER_ACTIVO, "E_ESTADO_CLUSTER_ACTIVO" },
        { E_ESTADO_CLUSTER_NO_ACTIVO, "E_ESTADO_CLUSTER_NO_ACTIVO" },
        { E_ESTADO_CLUSTER_DEGRADADO, "E_ESTADO_CLUSTER_DEGRADADO" },
        { E_ESTADO_CLUSTER_FALLO, "E_ESTADO_CLUSTER_FALLO" },
        { (E_ESTADO_CLUSTER)  ~0, NULL }
    };

    inline bool getEnumElemId( E_ESTADO_CLUSTER & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_ESTADO_CLUSTER> * it =  g_aEnumElemToString_E_ESTADO_CLUSTER; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_ESTADO_CLUSTER & elem )
    {
        for(const EnumElemToString<E_ESTADO_CLUSTER> * it =  g_aEnumElemToString_E_ESTADO_CLUSTER; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_ESTADO_CLUSTER > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_ESTADO_CLUSTER) / sizeof(* g_aEnumElemToString_E_ESTADO_CLUSTER);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_ESTADO_CLUSTER[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_ESTADO_BACKUP> g_aEnumElemToString_E_ESTADO_BACKUP[] =
    {
        { E_ESTADO_BACKUP_OK, "E_ESTADO_BACKUP_OK" },
        { E_ESTADO_BACKUP_NO_ACTUALIZADO, "E_ESTADO_BACKUP_NO_ACTUALIZADO" },
        { E_ESTADO_BACKUP_FALLO, "E_ESTADO_BACKUP_FALLO" },
        { (E_ESTADO_BACKUP)  ~0, NULL }
    };

    inline bool getEnumElemId( E_ESTADO_BACKUP & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_ESTADO_BACKUP> * it =  g_aEnumElemToString_E_ESTADO_BACKUP; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_ESTADO_BACKUP & elem )
    {
        for(const EnumElemToString<E_ESTADO_BACKUP> * it =  g_aEnumElemToString_E_ESTADO_BACKUP; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_ESTADO_BACKUP > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_ESTADO_BACKUP) / sizeof(* g_aEnumElemToString_E_ESTADO_BACKUP);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_ESTADO_BACKUP[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_MEDIDAS_RED> g_aEnumElemToString_E_MEDIDAS_RED[] =
    {
        { IPDV, "IPDV" },
        { PAQPERDIDOS, "PAQPERDIDOS" },
        { (E_MEDIDAS_RED)  ~0, NULL }
    };

    inline bool getEnumElemId( E_MEDIDAS_RED & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_MEDIDAS_RED> * it =  g_aEnumElemToString_E_MEDIDAS_RED; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_MEDIDAS_RED & elem )
    {
        for(const EnumElemToString<E_MEDIDAS_RED> * it =  g_aEnumElemToString_E_MEDIDAS_RED; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_MEDIDAS_RED > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_MEDIDAS_RED) / sizeof(* g_aEnumElemToString_E_MEDIDAS_RED);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_MEDIDAS_RED[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_ESTADO_POSICION> g_aEnumElemToString_E_ESTADO_POSICION[] =
    {
        { NO_OK, "NO_OK" },
        { OK, "OK" },
        { (E_ESTADO_POSICION)  ~0, NULL }
    };

    inline bool getEnumElemId( E_ESTADO_POSICION & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_ESTADO_POSICION> * it =  g_aEnumElemToString_E_ESTADO_POSICION; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_ESTADO_POSICION & elem )
    {
        for(const EnumElemToString<E_ESTADO_POSICION> * it =  g_aEnumElemToString_E_ESTADO_POSICION; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_ESTADO_POSICION > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_ESTADO_POSICION) / sizeof(* g_aEnumElemToString_E_ESTADO_POSICION);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_ESTADO_POSICION[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_ON_OFF> g_aEnumElemToString_E_ON_OFF[] =
    {
        { OFF, "OFF" },
        { ON, "ON" },
        { (E_ON_OFF)  ~0, NULL }
    };

    inline bool getEnumElemId( E_ON_OFF & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_ON_OFF> * it =  g_aEnumElemToString_E_ON_OFF; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_ON_OFF & elem )
    {
        for(const EnumElemToString<E_ON_OFF> * it =  g_aEnumElemToString_E_ON_OFF; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_ON_OFF > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_ON_OFF) / sizeof(* g_aEnumElemToString_E_ON_OFF);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_ON_OFF[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_ABONADO_SAM> g_aEnumElemToString_E_ABONADO_SAM[] =
    {
        { ABONADO_NULO, "ABONADO_NULO" },
        { PATRON, "PATRON" },
        { MEDIDAS, "MEDIDAS" },
        { (E_ABONADO_SAM)  ~0, NULL }
    };

    inline bool getEnumElemId( E_ABONADO_SAM & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_ABONADO_SAM> * it =  g_aEnumElemToString_E_ABONADO_SAM; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_ABONADO_SAM & elem )
    {
        for(const EnumElemToString<E_ABONADO_SAM> * it =  g_aEnumElemToString_E_ABONADO_SAM; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_ABONADO_SAM > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_ABONADO_SAM) / sizeof(* g_aEnumElemToString_E_ABONADO_SAM);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_ABONADO_SAM[idx];
        }

        return nElems;
    }

    static EnumElemToString<E_TIPO_ERROR_SAM> g_aEnumElemToString_E_TIPO_ERROR_SAM[] =
    {
        { TIMEOUT_POSICION, "TIMEOUT_POSICION" },
        { SECTORIZADA, "SECTORIZADA" },
        { FALLO_SECTORIZACION, "FALLO_SECTORIZACION" },
        { FALLO_REGISTRO, "FALLO_REGISTRO" },
        { TIMEOUT_PATRON, "TIMEOUT_PATRON" },
        { ERROR_PRESENCIAS, "ERROR_PRESENCIAS" },
        { ERROR_ESTADO_TARJETA, "ERROR_ESTADO_TARJETA" },
        { ERROR_TIPO, "ERROR_TIPO" },
        { TIMEOUT_INTERFAZ, "TIMEOUT_INTERFAZ" },
        { NO_ACEPTADO, "NO_ACEPTADO" },
        { FALLO_LLAMADA_SAM, "FALLO_LLAMADA_SAM" },
        { FALLO_DES_LLAMADA_SAM, "FALLO_DES_LLAMADA_SAM" },
        { FALLO_ACTIVACION_BUCLE, "FALLO_ACTIVACION_BUCLE" },
        { FALLO_DES_ACTIVACION_BUCLE, "FALLO_DES_ACTIVACION_BUCLE" },
        { TIMEOUT_MEDIDAS, "TIMEOUT_MEDIDAS" },
        { ERROR_ESTADO, "ERROR_ESTADO" },
        { SIN_RECHAZO, "SIN_RECHAZO" },
        { OTROS, "OTROS" },
        { (E_TIPO_ERROR_SAM)  ~0, NULL }
    };

    inline bool getEnumElemId( E_TIPO_ERROR_SAM & elem, const char * szElemName)
    {
        for(const EnumElemToString<E_TIPO_ERROR_SAM> * it =  g_aEnumElemToString_E_TIPO_ERROR_SAM; it && it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
    }

    inline bool getEnumElemName( const char ** pszElemName, const E_TIPO_ERROR_SAM & elem )
    {
        for(const EnumElemToString<E_TIPO_ERROR_SAM> * it =  g_aEnumElemToString_E_TIPO_ERROR_SAM; it && it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
    }

    inline size_t getEnumElem( EnumElemToString<  E_TIPO_ERROR_SAM > * pElem, size_t idx )
    {
        const size_t nElems = sizeof(g_aEnumElemToString_E_TIPO_ERROR_SAM) / sizeof(* g_aEnumElemToString_E_TIPO_ERROR_SAM);

        if(idx < nElems && pElem)
        {
            *pElem = g_aEnumElemToString_E_TIPO_ERROR_SAM[idx];
        }

        return nElems;
    }


    /// Nombres de tipos de datos

    /// Obtiene el nombre del tipo de datos para E_TIP_MSG
    template<>
    inline const char * getTypeName<E_TIP_MSG>()
    {
        return "E_TIP_MSG";
    }

    /// Obtiene el nombre del tipo de datos para E_ID_MSG
    template<>
    inline const char * getTypeName<E_ID_MSG>()
    {
        return "E_ID_MSG";
    }

    /// Obtiene el nombre del tipo de datos para E_SUBID_MSG
    template<>
    inline const char * getTypeName<E_SUBID_MSG>()
    {
        return "E_SUBID_MSG";
    }

    /// Obtiene el nombre del tipo de datos para E_TIPO_EQUIPO
    template<>
    inline const char * getTypeName<E_TIPO_EQUIPO>()
    {
        return "E_TIPO_EQUIPO";
    }

    /// Obtiene el nombre del tipo de datos para E_EST_OPERATIVO
    template<>
    inline const char * getTypeName<E_EST_OPERATIVO>()
    {
        return "E_EST_OPERATIVO";
    }

    /// Obtiene el nombre del tipo de datos para E_SINO
    template<>
    inline const char * getTypeName<E_SINO>()
    {
        return "E_SINO";
    }

    /// Obtiene el nombre del tipo de datos para E_TIPO_PROC
    template<>
    inline const char * getTypeName<E_TIPO_PROC>()
    {
        return "E_TIPO_PROC";
    }

    /// Obtiene el nombre del tipo de datos para E_MOTIVO_SECTORIZ
    template<>
    inline const char * getTypeName<E_MOTIVO_SECTORIZ>()
    {
        return "E_MOTIVO_SECTORIZ";
    }

    /// Obtiene el nombre del tipo de datos para E_SUB_ID_MSJ_TLF
    template<>
    inline const char * getTypeName<E_SUB_ID_MSJ_TLF>()
    {
        return "E_SUB_ID_MSJ_TLF";
    }

    /// Obtiene el nombre del tipo de datos para E_SUB_ID_MSJ_RAD
    template<>
    inline const char * getTypeName<E_SUB_ID_MSJ_RAD>()
    {
        return "E_SUB_ID_MSJ_RAD";
    }

    /// Obtiene el nombre del tipo de datos para E_SISTEMA_ABC
    template<>
    inline const char * getTypeName<E_SISTEMA_ABC>()
    {
        return "E_SISTEMA_ABC";
    }

    /// Obtiene el nombre del tipo de datos para E_TIPO_EQUIPO_EST
    template<>
    inline const char * getTypeName<E_TIPO_EQUIPO_EST>()
    {
        return "E_TIPO_EQUIPO_EST";
    }

    /// Obtiene el nombre del tipo de datos para E_TIPO_PUESTO
    template<>
    inline const char * getTypeName<E_TIPO_PUESTO>()
    {
        return "E_TIPO_PUESTO";
    }

    /// Obtiene el nombre del tipo de datos para E_TIPO_GATEWAY
    template<>
    inline const char * getTypeName<E_TIPO_GATEWAY>()
    {
        return "E_TIPO_GATEWAY";
    }

    /// Obtiene el nombre del tipo de datos para E_TIPO_ABONADO_INTERNO
    template<>
    inline const char * getTypeName<E_TIPO_ABONADO_INTERNO>()
    {
        return "E_TIPO_ABONADO_INTERNO";
    }

    /// Obtiene el nombre del tipo de datos para E_ESTADO_ENLACE
    template<>
    inline const char * getTypeName<E_ESTADO_ENLACE>()
    {
        return "E_ESTADO_ENLACE";
    }

    /// Obtiene el nombre del tipo de datos para E_TIPO_ENLACE
    template<>
    inline const char * getTypeName<E_TIPO_ENLACE>()
    {
        return "E_TIPO_ENLACE";
    }

    /// Obtiene el nombre del tipo de datos para E_EST_TF
    template<>
    inline const char * getTypeName<E_EST_TF>()
    {
        return "E_EST_TF";
    }

    /// Obtiene el nombre del tipo de datos para E_SUBESTADO
    template<>
    inline const char * getTypeName<E_SUBESTADO>()
    {
        return "E_SUBESTADO";
    }

    /// Obtiene el nombre del tipo de datos para E_SERVICIO
    template<>
    inline const char * getTypeName<E_SERVICIO>()
    {
        return "E_SERVICIO";
    }

    /// Obtiene el nombre del tipo de datos para E_TIPO_PRIORIDAD_R2
    template<>
    inline const char * getTypeName<E_TIPO_PRIORIDAD_R2>()
    {
        return "E_TIPO_PRIORIDAD_R2";
    }

    /// Obtiene el nombre del tipo de datos para E_TIPO_RUTA_EST
    template<>
    inline const char * getTypeName<E_TIPO_RUTA_EST>()
    {
        return "E_TIPO_RUTA_EST";
    }

    /// Obtiene el nombre del tipo de datos para E_ESTADO_CONFIG
    template<>
    inline const char * getTypeName<E_ESTADO_CONFIG>()
    {
        return "E_ESTADO_CONFIG";
    }

    /// Obtiene el nombre del tipo de datos para E_PROCEDENCIA_SECTORIZ
    template<>
    inline const char * getTypeName<E_PROCEDENCIA_SECTORIZ>()
    {
        return "E_PROCEDENCIA_SECTORIZ";
    }

    /// Obtiene el nombre del tipo de datos para TIPO_TMCS
    template<>
    inline const char * getTypeName<TIPO_TMCS>()
    {
        return "TIPO_TMCS";
    }

    /// Obtiene el nombre del tipo de datos para E_ESTADO_ENLACE_SACTA
    template<>
    inline const char * getTypeName<E_ESTADO_ENLACE_SACTA>()
    {
        return "E_ESTADO_ENLACE_SACTA";
    }

    /// Obtiene el nombre del tipo de datos para E_ESTADO_LAN
    template<>
    inline const char * getTypeName<E_ESTADO_LAN>()
    {
        return "E_ESTADO_LAN";
    }

    /// Obtiene el nombre del tipo de datos para E_MODO_SAM
    template<>
    inline const char * getTypeName<E_MODO_SAM>()
    {
        return "E_MODO_SAM";
    }

    /// Obtiene el nombre del tipo de datos para E_TIPO_AUDIO_RED
    template<>
    inline const char * getTypeName<E_TIPO_AUDIO_RED>()
    {
        return "E_TIPO_AUDIO_RED";
    }

    /// Obtiene el nombre del tipo de datos para E_VALOR_OPTIMO
    template<>
    inline const char * getTypeName<E_VALOR_OPTIMO>()
    {
        return "E_VALOR_OPTIMO";
    }

    /// Obtiene el nombre del tipo de datos para E_TIPO_CR
    template<>
    inline const char * getTypeName<E_TIPO_CR>()
    {
        return "E_TIPO_CR";
    }

    /// Obtiene el nombre del tipo de datos para E_RESULTADO
    template<>
    inline const char * getTypeName<E_RESULTADO>()
    {
        return "E_RESULTADO";
    }

    /// Obtiene el nombre del tipo de datos para E_RES_MEDIDA
    template<>
    inline const char * getTypeName<E_RES_MEDIDA>()
    {
        return "E_RES_MEDIDA";
    }

    /// Obtiene el nombre del tipo de datos para E_ID_CANAL_BUCLE
    template<>
    inline const char * getTypeName<E_ID_CANAL_BUCLE>()
    {
        return "E_ID_CANAL_BUCLE";
    }

    /// Obtiene el nombre del tipo de datos para E_TIPO_MEDIDA
    template<>
    inline const char * getTypeName<E_TIPO_MEDIDA>()
    {
        return "E_TIPO_MEDIDA";
    }

    /// Obtiene el nombre del tipo de datos para E_ESTADO_FTP
    template<>
    inline const char * getTypeName<E_ESTADO_FTP>()
    {
        return "E_ESTADO_FTP";
    }

    /// Obtiene el nombre del tipo de datos para E_ESTADO_GENERAL
    template<>
    inline const char * getTypeName<E_ESTADO_GENERAL>()
    {
        return "E_ESTADO_GENERAL";
    }

    /// Obtiene el nombre del tipo de datos para E_ESTADO_CLUSTER
    template<>
    inline const char * getTypeName<E_ESTADO_CLUSTER>()
    {
        return "E_ESTADO_CLUSTER";
    }

    /// Obtiene el nombre del tipo de datos para E_ESTADO_BACKUP
    template<>
    inline const char * getTypeName<E_ESTADO_BACKUP>()
    {
        return "E_ESTADO_BACKUP";
    }

    /// Obtiene el nombre del tipo de datos para E_MEDIDAS_RED
    template<>
    inline const char * getTypeName<E_MEDIDAS_RED>()
    {
        return "E_MEDIDAS_RED";
    }

    /// Obtiene el nombre del tipo de datos para E_ESTADO_POSICION
    template<>
    inline const char * getTypeName<E_ESTADO_POSICION>()
    {
        return "E_ESTADO_POSICION";
    }

    /// Obtiene el nombre del tipo de datos para E_ON_OFF
    template<>
    inline const char * getTypeName<E_ON_OFF>()
    {
        return "E_ON_OFF";
    }

    /// Obtiene el nombre del tipo de datos para E_ABONADO_SAM
    template<>
    inline const char * getTypeName<E_ABONADO_SAM>()
    {
        return "E_ABONADO_SAM";
    }

    /// Obtiene el nombre del tipo de datos para E_TIPO_ERROR_SAM
    template<>
    inline const char * getTypeName<E_TIPO_ERROR_SAM>()
    {
        return "E_TIPO_ERROR_SAM";
    }

    /// Obtiene el nombre del tipo de datos para MSG_EST_CONFIG
    template<>
    inline const char * getTypeName<MSG_EST_CONFIG>()
    {
        return "MSG_EST_CONFIG";
    }

    /// Obtiene el nombre del tipo de datos para IDENT_EQUIPO
    template<>
    inline const char * getTypeName<IDENT_EQUIPO>()
    {
        return "IDENT_EQUIPO";
    }

    /// Obtiene el nombre del tipo de datos para CAB_MSG_TH
    template<>
    inline const char * getTypeName<CAB_MSG_TH>()
    {
        return "CAB_MSG_TH";
    }

    /// Obtiene el nombre del tipo de datos para SUP_PUES
    template<>
    inline const char * getTypeName<SUP_PUES>()
    {
        return "SUP_PUES";
    }

    /// Obtiene el nombre del tipo de datos para ESTADO_HW_GENERAL
    template<>
    inline const char * getTypeName<ESTADO_HW_GENERAL>()
    {
        return "ESTADO_HW_GENERAL";
    }

    /// Obtiene el nombre del tipo de datos para ESTADO_HW_ENLACE
    template<>
    inline const char * getTypeName<ESTADO_HW_ENLACE>()
    {
        return "ESTADO_HW_ENLACE";
    }

    /// Obtiene el nombre del tipo de datos para ESTADO_SUPERVISION_ENLACE
    template<>
    inline const char * getTypeName<ESTADO_SUPERVISION_ENLACE>()
    {
        return "ESTADO_SUPERVISION_ENLACE";
    }

    /// Obtiene el nombre del tipo de datos para SUP_GW
    template<>
    inline const char * getTypeName<SUP_GW>()
    {
        return "SUP_GW";
    }

    /// Obtiene el nombre del tipo de datos para SUP_TMCS
    template<>
    inline const char * getTypeName<SUP_TMCS>()
    {
        return "SUP_TMCS";
    }

    /// Obtiene el nombre del tipo de datos para MSG_CONFIG_IMPLANTADA
    template<>
    inline const char * getTypeName<MSG_CONFIG_IMPLANTADA>()
    {
        return "MSG_CONFIG_IMPLANTADA";
    }

    /// Obtiene el nombre del tipo de datos para SUP_FICH_CONF
    template<>
    inline const char * getTypeName<SUP_FICH_CONF>()
    {
        return "SUP_FICH_CONF";
    }

    /// Obtiene el nombre del tipo de datos para INF_CRC_FICHEROS
    template<>
    inline const char * getTypeName<INF_CRC_FICHEROS>()
    {
        return "INF_CRC_FICHEROS";
    }

    /// Obtiene el nombre del tipo de datos para MSG_SUPER
    template<>
    inline const char * getTypeName<MSG_SUPER>()
    {
        return "MSG_SUPER";
    }

    /// Obtiene el nombre del tipo de datos para PUESTOS_INTEGRANTES
    template<>
    inline const char * getTypeName<PUESTOS_INTEGRANTES>()
    {
        return "PUESTOS_INTEGRANTES";
    }

    /// Obtiene el nombre del tipo de datos para GATEWAYS_INTEGRANTES
    template<>
    inline const char * getTypeName<GATEWAYS_INTEGRANTES>()
    {
        return "GATEWAYS_INTEGRANTES";
    }

    /// Obtiene el nombre del tipo de datos para DAT_FICH_CONFIG
    template<>
    inline const char * getTypeName<DAT_FICH_CONFIG>()
    {
        return "DAT_FICH_CONFIG";
    }

    /// Obtiene el nombre del tipo de datos para MSG_CONFIG_INF_FICH_CONFIG
    template<>
    inline const char * getTypeName<MSG_CONFIG_INF_FICH_CONFIG>()
    {
        return "MSG_CONFIG_INF_FICH_CONFIG";
    }

    /// Obtiene el nombre del tipo de datos para MSG_CONFIG_FICH_CONFIG
    template<>
    inline const char * getTypeName<MSG_CONFIG_FICH_CONFIG>()
    {
        return "MSG_CONFIG_FICH_CONFIG";
    }

    /// Obtiene el nombre del tipo de datos para MSG_ENVIA_TABLA
    template<>
    inline const char * getTypeName<MSG_ENVIA_TABLA>()
    {
        return "MSG_ENVIA_TABLA";
    }

    /// Obtiene el nombre del tipo de datos para MSG_ARRANQUE_CONFIG
    template<>
    inline const char * getTypeName<MSG_ARRANQUE_CONFIG>()
    {
        return "MSG_ARRANQUE_CONFIG";
    }

    /// Obtiene el nombre del tipo de datos para MSG_SECTORIZA
    template<>
    inline const char * getTypeName<MSG_SECTORIZA>()
    {
        return "MSG_SECTORIZA";
    }

    /// Obtiene el nombre del tipo de datos para SAM_timeb
    template<>
    inline const char * getTypeName<SAM_timeb>()
    {
        return "SAM_timeb";
    }

    /// Obtiene el nombre del tipo de datos para MSG_DATETIME_SG
    template<>
    inline const char * getTypeName<MSG_DATETIME_SG>()
    {
        return "MSG_DATETIME_SG";
    }

    /// Obtiene el nombre del tipo de datos para ABO_PUE_TEL_EST
    template<>
    inline const char * getTypeName<ABO_PUE_TEL_EST>()
    {
        return "ABO_PUE_TEL_EST";
    }

    /// Obtiene el nombre del tipo de datos para MSG_EST_COMU_TLF
    template<>
    inline const char * getTypeName<MSG_EST_COMU_TLF>()
    {
        return "MSG_EST_COMU_TLF";
    }

    /// Obtiene el nombre del tipo de datos para ABO_PUE_RD_EST
    template<>
    inline const char * getTypeName<ABO_PUE_RD_EST>()
    {
        return "ABO_PUE_RD_EST";
    }

    /// Obtiene el nombre del tipo de datos para CANAL_EST
    template<>
    inline const char * getTypeName<CANAL_EST>()
    {
        return "CANAL_EST";
    }

    /// Obtiene el nombre del tipo de datos para CANAL_BSS_EST
    template<>
    inline const char * getTypeName<CANAL_BSS_EST>()
    {
        return "CANAL_BSS_EST";
    }

    /// Obtiene el nombre del tipo de datos para MSG_EST_COMU_RAD
    template<>
    inline const char * getTypeName<MSG_EST_COMU_RAD>()
    {
        return "MSG_EST_COMU_RAD";
    }

    /// Obtiene el nombre del tipo de datos para MSG_CONFIG_SESION
    template<>
    inline const char * getTypeName<MSG_CONFIG_SESION>()
    {
        return "MSG_CONFIG_SESION";
    }

    /// Obtiene el nombre del tipo de datos para MSG_ENLACE_SACTA
    template<>
    inline const char * getTypeName<MSG_ENLACE_SACTA>()
    {
        return "MSG_ENLACE_SACTA";
    }

    /// Obtiene el nombre del tipo de datos para MSG_MEDIDAS_RED
    template<>
    inline const char * getTypeName<MSG_MEDIDAS_RED>()
    {
        return "MSG_MEDIDAS_RED";
    }

    /// Obtiene el nombre del tipo de datos para MSG_ESTADO_RED
    template<>
    inline const char * getTypeName<MSG_ESTADO_RED>()
    {
        return "MSG_ESTADO_RED";
    }

    /// Obtiene el nombre del tipo de datos para RES_MEDIDA
    template<>
    inline const char * getTypeName<RES_MEDIDA>()
    {
        return "RES_MEDIDA";
    }

    /// Obtiene el nombre del tipo de datos para MEDIDAS_CANAL_BUCLE
    template<>
    inline const char * getTypeName<MEDIDAS_CANAL_BUCLE>()
    {
        return "MEDIDAS_CANAL_BUCLE";
    }

    /// Obtiene el nombre del tipo de datos para MSG_INTERFAZ_BATERIA
    template<>
    inline const char * getTypeName<MSG_INTERFAZ_BATERIA>()
    {
        return "MSG_INTERFAZ_BATERIA";
    }

    /// Obtiene el nombre del tipo de datos para MSG_POSICION_BATERIA
    template<>
    inline const char * getTypeName<MSG_POSICION_BATERIA>()
    {
        return "MSG_POSICION_BATERIA";
    }

    /// Obtiene el nombre del tipo de datos para MSG_RED_SAM
    template<>
    inline const char * getTypeName<MSG_RED_SAM>()
    {
        return "MSG_RED_SAM";
    }

    /// Obtiene el nombre del tipo de datos para MED_CANAL_BUCLE
    template<>
    inline const char * getTypeName<MED_CANAL_BUCLE>()
    {
        return "MED_CANAL_BUCLE";
    }

    /// Obtiene el nombre del tipo de datos para MED_POSICION
    template<>
    inline const char * getTypeName<MED_POSICION>()
    {
        return "MED_POSICION";
    }

    /// Obtiene el nombre del tipo de datos para MED_INTERFAZ
    template<>
    inline const char * getTypeName<MED_INTERFAZ>()
    {
        return "MED_INTERFAZ";
    }

    /// Obtiene el nombre del tipo de datos para MSG_AUDIO_SAM_POSICION
    template<>
    inline const char * getTypeName<MSG_AUDIO_SAM_POSICION>()
    {
        return "MSG_AUDIO_SAM_POSICION";
    }

    /// Obtiene el nombre del tipo de datos para MSG_AUDIO_SAM_INTERFAZ
    template<>
    inline const char * getTypeName<MSG_AUDIO_SAM_INTERFAZ>()
    {
        return "MSG_AUDIO_SAM_INTERFAZ";
    }

    /// Obtiene el nombre del tipo de datos para MSG_PRES_VRP
    template<>
    inline const char * getTypeName<MSG_PRES_VRP>()
    {
        return "MSG_PRES_VRP";
    }

    /// Obtiene el nombre del tipo de datos para MSG_PRES_ESTAD
    template<>
    inline const char * getTypeName<MSG_PRES_ESTAD>()
    {
        return "MSG_PRES_ESTAD";
    }

    /// Obtiene el nombre del tipo de datos para DAT_INICIO_SESION_SAM
    template<>
    inline const char * getTypeName<DAT_INICIO_SESION_SAM>()
    {
        return "DAT_INICIO_SESION_SAM";
    }

    /// Obtiene el nombre del tipo de datos para MSG_BATERIA_POR_ID
    template<>
    inline const char * getTypeName<MSG_BATERIA_POR_ID>()
    {
        return "MSG_BATERIA_POR_ID";
    }

    /// Obtiene el nombre del tipo de datos para MSG_ENVIA_BATERIA_VACIA
    template<>
    inline const char * getTypeName<MSG_ENVIA_BATERIA_VACIA>()
    {
        return "MSG_ENVIA_BATERIA_VACIA";
    }

    /// Obtiene el nombre del tipo de datos para MSG_ID_SESION
    template<>
    inline const char * getTypeName<MSG_ID_SESION>()
    {
        return "MSG_ID_SESION";
    }

    /// Obtiene el nombre del tipo de datos para M_UMBRAL_INTERVALO
    template<>
    inline const char * getTypeName<M_UMBRAL_INTERVALO>()
    {
        return "M_UMBRAL_INTERVALO";
    }

    /// Obtiene el nombre del tipo de datos para M_UMBRALES_MEDIDAS
    template<>
    inline const char * getTypeName<M_UMBRALES_MEDIDAS>()
    {
        return "M_UMBRALES_MEDIDAS";
    }

    /// Obtiene el nombre del tipo de datos para MSG_UMBRALES
    template<>
    inline const char * getTypeName<MSG_UMBRALES>()
    {
        return "MSG_UMBRALES";
    }

    /// Obtiene el nombre del tipo de datos para MSG_EJECUTA_LISTA_MEDIDAS_RED
    template<>
    inline const char * getTypeName<MSG_EJECUTA_LISTA_MEDIDAS_RED>()
    {
        return "MSG_EJECUTA_LISTA_MEDIDAS_RED";
    }

    /// Obtiene el nombre del tipo de datos para MSG_MODIFICA_PARAMETROS
    template<>
    inline const char * getTypeName<MSG_MODIFICA_PARAMETROS>()
    {
        return "MSG_MODIFICA_PARAMETROS";
    }

    /// Obtiene el nombre del tipo de datos para MSG_POSICION_SAM
    template<>
    inline const char * getTypeName<MSG_POSICION_SAM>()
    {
        return "MSG_POSICION_SAM";
    }

    /// Obtiene el nombre del tipo de datos para MSG_RESP_POSICION
    template<>
    inline const char * getTypeName<MSG_RESP_POSICION>()
    {
        return "MSG_RESP_POSICION";
    }

    /// Obtiene el nombre del tipo de datos para MSG_INTERFAZ_MEDICION_SAM
    template<>
    inline const char * getTypeName<MSG_INTERFAZ_MEDICION_SAM>()
    {
        return "MSG_INTERFAZ_MEDICION_SAM";
    }

    /// Obtiene el nombre del tipo de datos para MSG_RESP_MEDICION_SAM
    template<>
    inline const char * getTypeName<MSG_RESP_MEDICION_SAM>()
    {
        return "MSG_RESP_MEDICION_SAM";
    }

    /// Obtiene el nombre del tipo de datos para MSG_SET_LLAMADA_SAM
    template<>
    inline const char * getTypeName<MSG_SET_LLAMADA_SAM>()
    {
        return "MSG_SET_LLAMADA_SAM";
    }

    /// Obtiene el nombre del tipo de datos para MSG_RESP_LLAMADA_SAM
    template<>
    inline const char * getTypeName<MSG_RESP_LLAMADA_SAM>()
    {
        return "MSG_RESP_LLAMADA_SAM";
    }

    /// Obtiene el nombre del tipo de datos para MSG_SET_BUCLE_SAM
    template<>
    inline const char * getTypeName<MSG_SET_BUCLE_SAM>()
    {
        return "MSG_SET_BUCLE_SAM";
    }

    /// Obtiene el nombre del tipo de datos para MSG_PRES_SAM
    template<>
    inline const char * getTypeName<MSG_PRES_SAM>()
    {
        return "MSG_PRES_SAM";
    }

    /// Obtiene el nombre del tipo de datos para MSG_PRES_WATCHDOG_SAM
    template<>
    inline const char * getTypeName<MSG_PRES_WATCHDOG_SAM>()
    {
        return "MSG_PRES_WATCHDOG_SAM";
    }

    /// Obtiene el nombre del tipo de datos para MSG_TH
    template<>
    inline const char * getTypeName<MSG_TH>()
    {
        return "MSG_TH";
    }

} ///< namespaceSAM_Declaration


#endif //SAM_DeclarationParser_h__

