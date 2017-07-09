/// @file SAM_Declaration.h
/// @note Vendor: Microsoft

//

#ifndef SAM_Declaration_h__
#define SAM_Declaration_h__

#include "SAM_DeclarationCommon.h"


/// Encode as big endian. That is a network independent code.
#define CHARTOLONG_BE(x4, x3, x2, x1) ((uint32)(uint8)(x1) | ((uint32)(uint8)(x2) << 8) | ((uint32)(uint8)(x3) << 16) | ((uint32)(uint8)(x4) << 24))
#define CHARTOLONG_LE(x4, x3, x2, x1) ((uint32)(uint8)(x4) | ((uint32)(uint8)(x3) << 8) | ((uint32)(uint8)(x2) << 16) | ((uint32)(uint8)(x1) << 24))

#ifndef CHARTOLONG
#if defined(BIG_ENDIAN_OR_PPC)
#define CHARTOLONG CHARTOLONG_BE
#else
#define CHARTOLONG CHARTOLONG_LE
#endif
#endif
/// @note Usando la tabla T_PUESTOS: config_sdcxxi_hw.txt
/// @note Usando la tabla T_GATEWAYS: config_sdcxxi_hw.txt
/// @note Usando la tabla T_ENL: config_sdcxxi_hw.txt
/// @note Usando la tabla T_CENT: config_sdcxxi_log.txt
/// @note Usando la tabla T_ABON: config_sdcxxi_log_sect_a.txt config_sdcxxi_log_sect_b.txt
/// @note Usando la tabla T_EMPL: config_sdcxxi_log.txt
/// @note Usando la tabla T_FREC: config_sdcxxi_log.txt

#define MAX_NOMBRE_PUESTO 12
#define MAX_NOMBRE_ABONADO 12
#define MAX_NOMBRE_BATERIA 30
#define MAX_NOMBRE_DESCRIPTIVO 40
#define NMAX_PUESTOS 256
#define NMAX_GATEWAYS 50
#define NMAX_NOMBRE_FICHERO 48
#define NMAX_NOMBRE_CONFIG 21
#define NMAX_VERSION_CONFIG 6
#define NMAX_BYTES_BLOQUE 1024
#define NMAX_TAM_TABLA 1000
#define NMAX_NOMBRE_TABLA 25
#define NMAX_NOMBRE_PROCESO 64
#define LNG_PREF_EST 5
#define LNG_NOM_ABON_EST 20
#define N_MAX_DIG_COD 20
#define NTITFZMU 32
#define NUM_ENLACES_GW 32
#define LMAXMENSSIP 2000 /// Longitud máxima de mensajes SIP
#define NMAX_CARAC_CAUSA 100 /// mensaje de sectorización
#define NMAX_PARAMETRO 20 /// Longitud máxima de un parámetro
#define MAX_NOMBRE_INFORME 12
#define MAX_NOMBRE_CENTRO 20
#define MAX_NOMBRE_BP 12
#define MAX_NOMBRE_MEDIDA 20
#define MAX_BUCLES_POSICION 6
#define MAX_CANALES_INTERFAZ 3
#define MAX_MEDIDAS_CANAL_BUCLE 9
#define MAX_MEDIDAS_RED 2
#define NMAX_ESTADO 3

namespace SAM {} // Reserva el namespace SAM por si no lo estuviera.


/**
      * @brief Declaraciones de estructuras y tipos enumerados usados en la comunicación UDP a bajo
      *     nivel con otros módulos del SCV.
      */
namespace SAM_Declaration
{
#ifdef _WIN32
#pragma pack(push, 1)
#endif

    using namespace SAM;
    using namespace SAM_DeclarationParser;

    enum E_TIP_MSG
    {
        PETI = CHARTOLONG('P','E','T','I'),
        RESP = CHARTOLONG('R','E','S','P'),
        NOTIF = CHARTOLONG('N','O','T','I')
    };

    enum E_ID_MSG /// Mensajes de estadísticas
    {
        M_PRESENCIA = CHARTOLONG('P','R','E','S'),
        M_FICH_CONFIG = CHARTOLONG('F','I','C','O'),
        M_INF_FICH_CONFIG = CHARTOLONG('I','N','C','O'),
        M_ARRANQUE = CHARTOLONG('A','R','R','A'),
        M_CONFIG_VALIDADA = CHARTOLONG('C','N','V','L'),
        M_ENVIA_TABLA = CHARTOLONG('E','N','T','B'),
        M_SECTORIZA = CHARTOLONG('S','E','C','T'),
        M_DATETIME_SG = CHARTOLONG('D','T','T','M'),
        M_ESTADISTICAS_TF = CHARTOLONG('E','S','T','F'), ///< RRG 31/03/2008
        M_ESTADISTICAS_RD = CHARTOLONG('E','S','R','D'), ///< RRG 31/03/2008 // Información de sectorizacion para estadisticas
        M_CONFIG_IMPLANTADA = CHARTOLONG('C','O','I','M'), ///< RRG 09/04/2008 ; Información de fichero de config para estadísticas
        M_SALIR_APLICACION = CHARTOLONG('S','A','A','P'), ///< RRG 16/04/2009
        M_COMENZAR_APLICACION = CHARTOLONG('C','O','A','P'), ///< RRG 16/04/2009
        M_ESTADO_CONFIG = CHARTOLONG('E','S','C','N'), ///< Mensaje de estado de configuraciones desde TMCS a Estadísticas
        M_ENLACE_SACTA = CHARTOLONG('E','N','S','A'), ///< Subestructura de mensaje de información de estado del enlace con SACTA
        M_INICIO_SESION = CHARTOLONG('I','N','S','E'), ///< Para solicitar ficheros al TMCS
        M_PRES_SAM = CHARTOLONG('P','S','A','M'), ///< Presencias del SAM
        M_ACK_FICHERO = CHARTOLONG('S','A','C','K'), ///< ACK del fichero de configuracióon
        M_BATERIA_CANCELADA = CHARTOLONG('S','B','P','C'), ///< La ejecucion de la bateria de pruebas se ha cancelado
        M_AUDIO_SAM_POSICION = CHARTOLONG('M','A','U','P'), ///< SAM: Medidas de audio en posiciones
        M_AUDIO_SAM_INTERFAZ = CHARTOLONG('M','A','U','I'), ///< SAM: Medidas de audio en interfaces
        M_RED_SAM = CHARTOLONG('M','R','E','D'), ///< SAM: Medidas de red
        M_PRES_VRP = CHARTOLONG('P','V','R','P'), ///< Presencias del VR&P
        M_PRES_ESTAD = CHARTOLONG('P','E','S','T'), ///< Presencias del Servidor de Estadísticas para que lo supervise el TMCS
        M_ESTADO_RED = CHARTOLONG('E','R','E','D'), ///< TMCS: Estado de la red
        M_MAX_SESIONES_SAM = CHARTOLONG('N','S','E','S'), ///< SAM: Numero maximo de sesiones alcanzado
        M_PORCENTAJE_AVANCE_SAM = CHARTOLONG('S','A','V','A'), ///< SAM: Porcentaje de avance
        M_INICIO_SESION_SAM = CHARTOLONG('S','I','N','S'), ///< SAM: Inicio de sesion
        M_FIN_SESION_SAM = CHARTOLONG('S','F','N','S'), ///< SAM: Fin de sesion
        M_LISTA_BATERIAS_SAM = CHARTOLONG('S','Q','L','P'), ///< SAM: Consulta la lista de baterias de pruebas
        M_CONSULTA_BATERIA_SAM = CHARTOLONG('S','Q','B','P'), ///< SAM: Consulta de bateria de pruebas
        M_CREACION_BATERIA_SAM = CHARTOLONG('S','C','B','P'), ///< SAM: Creacion de bateria de pruebas
        M_POSICION_BATERIA_SAM = CHARTOLONG('S','M','B','P'), ///< SAM: Alta, modificacion o borrado de las pruebas de una posición para una bateria de pruebas
        M_INTERFAZ_BATERIA_SAM = CHARTOLONG('S','M','B','I'), ///< SAM: Alta, modificacion o borrado de las pruebas de un interfaz para una bateria de pruebas
        M_BORRADO_BATERIA_SAM = CHARTOLONG('S','D','B','P'), ///< SAM: Borrado de bateria de pruebas
        M_RESULTADO_BATERIA_SAM = CHARTOLONG('S','R','B','P'), ///< SAM: Consulta del resultado de un bateria de pruebas
        M_EJECUTA_BATERIA_SAM = CHARTOLONG('S','X','B','P'), ///< SAM: Ejecuta una bateria de pruebas
        M_CANCELA_BATERIA_SAM = CHARTOLONG('S','N','B','P'), ///< SAM: Cancela la ejecucion una bateria de pruebas
        M_LISTA_MEDIDAS_RED = CHARTOLONG('S','L','M','R'), ///< SAM: Lista de medidas de red disponibles
        M_EJECUTA_MEDIDAS_RED = CHARTOLONG('S','X','M','R'), ///< SAM: Ejecuta un listado de medidas de red
        M_CANCELA_MEDIDAS_RED = CHARTOLONG('S','C','M','R'), ///< SAM: Cancela la ejecucion de medidas de red
        M_CONSULTA_UMBRALES = CHARTOLONG('S','C','U','M'), ///< SAM: Consulta los umbrales de las medidas del SAM
        M_MODIFICA_UMBRALES = CHARTOLONG('S','M','U','M'), ///< SAM: Modifica los umbrales de las medidas del SAM
        M_CONSULTA_PARAMETROS = CHARTOLONG('S','C','P','M'), ///< SAM: Consulta los parámetros de configuración del SAM
        M_MODIFICA_PARAMETROS = CHARTOLONG('S','M','P','M'), ///< SAM: Consulta los parámetros de configuración del SAM
        M_RESP_ACTIVACION_PATRON = CHARTOLONG('S','R','A','P'), ///< SAM: Respuesta a la peticion de la activacion de una posición como patrón
        M_ACTIVACION_PATRON = CHARTOLONG('S','A','C','P'), ///< SAM: Respuesta a la peticion de la activacion o desactivacion de una posición como patrón
        M_POSICION_MEDICION_SAM = CHARTOLONG('S','A','P','M'), ///< SAM: Peticion de la activacion o desactivacion de una posición en modo medición
        M_RESP_POSICION_MEDICION_SAM = CHARTOLONG('S','R','P','M'), ///< SAM: Respuesta a la peticion de la activacion o desactivacion de una posición en modo medición
        M_INTERFAZ_MEDICION_SAM = CHARTOLONG('S','A','I','M'), ///< SAM: Peticion de la activacion o desactivacion de un interfaz en modo medición
        M_RESP_MEDICION_SAM = CHARTOLONG('S','R','I','M'), ///< SAM: Respuesta a la peticion de la activacion o desactivacion de un interfaz en modo medición
        M_SET_LLAMADA_SAM = CHARTOLONG('S','A','L','L'), ///< SAM: Para establecer la llamada SAM
        M_RESP_LLAMADA_SAM = CHARTOLONG('S','R','L','L'), ///< SAM: Respuesta a la petición de establecer la llamada SAM
        M_SET_BUCLE_SAM = CHARTOLONG('S','A','B','U'), ///< SAM: Activacion de los bucles de la UGA
        M_PRES_WATCHDOG_SAM = CHARTOLONG('W','T','D','G') ///< SAM: Mensajes de presencia del Servidor SAM al Watchdog
    };

    enum E_SUBID_MSG
    {
        M_PING = CHARTOLONG('P','I','N','G'),
        M_STOP = CHARTOLONG('S','T','O','P')
    };

    enum E_TIPO_EQUIPO
    {
        EQ_NULO = 0,
        EQ_PUESTO = CHARTOLONG('P','U','E','S'),
        EQ_GATEWAY = CHARTOLONG('G','A','T','E'),
        EQ_GESTOR_MC = CHARTOLONG('G','_','M','C'),
        EQ_SUPERV = CHARTOLONG('S','U','P','E'),
        EQ_SUPERR = CHARTOLONG('S','U','P','R'),
        EQ_EQUI_GEN = CHARTOLONG('E','Q','G','E'),
        EQ_SAM = CHARTOLONG('S','S','A','M'),
        EQ_VRP = CHARTOLONG('S','V','R','P'),
        EQ_VRP_SACTA = CHARTOLONG('S','V','S','C'),
        EQ_PROXY = CHARTOLONG('P','R','X','Y'),
        EQ_ESTAD = CHARTOLONG('E','S','T','S'),
        EQ_WATCHDOG = CHARTOLONG('W','T','C','H')
    };

    enum E_EST_OPERATIVO /// Estado del procesador
    {
        E_NULO = 0,
        E_PRINCIPAL = 1,
        E_RESERVA = 2,
        E_FALLO = 3
    };

    enum E_SINO
    {
        NO = 0,
        SI = 1
    };

    enum E_TIPO_PROC
    {
        E_PROC_A = 0,
        E_PROC_B = 1
    };

    enum E_MOTIVO_SECTORIZ /// Procedencia sectorizacion
    {
        E_SECT_POR_ENVIO_CONFIG_NUEVA_TODOS = 1,
        E_SECT_POR_ENVIO_CONFIG_ARRANQUE_PUES_GW = 2,
        E_SECT_POR_SECT_LOCAL_TMCS = 3,
        E_SECT_POR_RECONF_UCS = 4,
        E_SECT_POR_SECT_SACTA = 5,
        E_NOSECT_POR_SECT_ERRONEA = 6,
        E_NOSECT_POR_SECT_EN_PROCESO = 7,
        E_NOSECT_POR_CONFIG_EN_PROCESO = 8,
        E_NOSECT_POR_SAM_SECTORIZADA = 9,
        E_SECT_SAM = 10,
        E_SECT_POR_RECONF_UCS_JACKS = 11
    };

    enum E_SUB_ID_MSJ_TLF /// SUBIDENTIFICADOR MENSAJE(1 Byte)
            /// Identificador del mensaje dentro de estadísticas telefonía
    {
        COMUNICACION = 1, ///< Comunicación
        INTERVENCION = 3, ///< Facilidad intervención
        REDIAL = 4, ///< Facilidad redial
        PRIO = 5, ///< Facilidad prioridad
        RELL_AUTO = 6, ///< Facilidad rellamada automática
        CAPT_LLAM = 7, ///< Captura de llamada
        REDIR = 8, ///< Redireccion llamadas
        TRANSFER_LLAM = 9 ///< Transferencia de llamada
    };

    enum E_SUB_ID_MSJ_RAD /// SUBIDENTIFICADOR MENSAJE RADIO(1 Byte)
            /// Identificador del mensaje dentro de estadísticas radio
    {
        PULSACION_PTT = 1, ///< Pulsación PTT
        SQUELCH = 2, ///< Squelch (Enviado por los gateways)
        SELECCION_TX = 3, ///< Selección TX
        SELECCION_RX = 4, ///< Selección RX
        FACILIDAD_GRUPO_RTX = 5, ///< Facilidad grupo RTX
        FACILIDAD_CASCOS_ALTAVOZ = 6, ///< Facilidad cascos/altavoz
        FACILIDAD_CAMBIO_EMPLAZAMIENTO = 7, ///< Facilidad Cambio emplazamiento
        FACILIDAD_INB_CASCOS_ALTAVOZ = 8, ///< Facilidad INB. Cascos/Altavoz
        FACILIDAD_ULTIMA_LLAMADA_RADIO = 9, ///< Facilidad Última llamada radio
        FACILIDAD_BSS_OVER = 10, ///< Facilidad BSS Over
        FACILIDAD_CLIMAX = 11, ///< Facilidad CLIMAX
        RECEPECION_AUDIO_EN_POSICION = 12, ///< Recepción audio en posición
        CALIDAD_BSS = 13, ///< Calidad BSS (Enviado por los gateways)
        FACILIDAD_ANNADIR_FREC = 14, ///< Facilidad Añadir FREC
        FACILIDAD_BORRAR_FREC = 15, ///< Facilidad Borrar FREC
        FACILIDAD_CAMBIO_MAIN_STANDBY = 16, ///< Facilidad Cambio Main/Standby
        FACILIDAD_CAMBIO_PAGINA_RADIO = 17, ///< Facilidad Cambio página radio
        FACILIDAD_SIDETONE = 18, ///< Facilidad Side Tine
        FACILIDAD_JACKS_CONT = 19, ///< Facilidad Jacks Controlador. Esta facilidad es virtual, ya que se recibe realmente como parte del mensaje de presencia de los puestos.
        FACILIDAD_JACKS_COORD = 20, ///< Facilidad Jacks Coordinador. Esta facilidad es virtual, ya que se recibe realmente como parte del mensaje de presencia de los puestos.
        CAPT_COORD_RADIO = 21 ///< Captura de servicios radio por el Coordinador
    };

    enum E_SISTEMA_ABC /// SISTEMA (A/B/C) (Sistema activo donde se realiza la llamada) (1 Byte)
    {
        SISTEMA_A = 'A',
        SISTEMA_B = 'B',
        SISTEMA_AYB = 'C' ///< Sistema A + Sistema B
    };

    enum E_TIPO_EQUIPO_EST /// Tipo equipo(Puesto/Gateway) origen/destino llamada
    {
        EQ_EST_NULO = 0,
        EQ_EST_PUESTO = 1, ///< PUESTO
        EQ_EST_GATEWAY = 2, ///< GATEWAY
        EQ_EST_GESTOR_MC = 3,
        EQ_EST_SUPERV = 4, ///< TMCS
        EQ_EST_EQUI_GEN = 5 ///< EQUIPO EN GENERAL, PUEDE SER UN GW,PUES, ...
    };

    enum E_TIPO_PUESTO
    {
        PUESTO_APOYO, ///< PUESTO APOYO
        TELEFONO_IP,
        NO_PUESTO,
        PUESTO_EJ, ///< PUESTO EJECUTIVO
        PUESTO_PL ///< PUESTO PLANIFICADOR
    };

    enum E_TIPO_GATEWAY
    {
        GW_NOIDEN = 0,
        GW_INDRA,
        GW_OTRO, ///< Gateway comercial CISCO
        GW_POSICION
    };

    enum E_TIPO_ABONADO_INTERNO
    {
        ABO_APOYO, ///< Abonado que puede asociarse a puestos
        ABO_PILOTO, ///< Piloto para simuladores
        ABO_TELEFONO, ///< Telefonos
        ABO_EJECUTIVO, ///< Abonado que se asocia a un puesto que pertenece a una UCS
        ABO_PLANIFICADOR, ///< Abonado que se asocia a un puesto que pertenece a una UCS
        NO_ABONADO ///< No definido el abonado
    };

    enum E_ESTADO_ENLACE
    {
        M_C = 0x80, ///< Máscara de configurado -> Solo para enlaces telefonía
        M_U = 0x40, ///< Máscara de util -> Solo para enlaces telefonía
        PROTOCOLO_OK = (M_C | M_U), ///< Protocolo correcto, en tarjetas R2, LCN
        PROTOCOLO_MAL = 0, ///< Protocolo mal, en tarjetas R2, LCN
        IFGW_TINEX = 0x0F ///< Tarjeta inexistente
    };

    enum E_TIPO_ENLACE /// Con el tipo de tarjeta se determinará el tipo de red a través del que se hace una llamada:
            /// - Redes PSTN (públicas) -> EYMDTMF, PABX, E1, BRI, FXO, ISDN_ST
            /// - Redes AGVN (red entre centros de control) - R2, QSIG, N5
            /// - Redes AGVN punto a punto (TFU/FXS, BL, BC, EYM, LCE)
    {
        EYMDTMF = 0x01,
        LCN = 0x02,
        CR = 0x03,
        N5 = 0x05, ///< CCITT 5
        EYM = 0x06,
        CRI = 0x07,
        LCE = 0x09,
        BL = 0x0a,
        PABX = 0x0b,
        R2 = 0x0c,
        QSIG = 0x0d,
        TFU = 0x0e,
        VACIO = 0x0f,
        ISDN_ST = 0x1d,
        E1 = 0x2d,
        BRI = 0x2e,
        FXO = 0x2f,
        FXS = 0x30,
        LC_AUX1 = 0x31, ///< Tarjeta IKUSI
        CR_AUX1 = 0x32, ///< Tarjeta de trunking (TELTRONIC)
        CR_BSS = 0x33, ///< Tarjeta Canal Radio con BSS
        NO_ENL
    };

    enum E_EST_TF /// ESTADO DE LA COMUNICACION (1 Byte)
            /// Dependen del valor del campo SUBIDENTIFICADOR MENSAJE.
            /// Fase de la comunicación en la que se encuentra la línea frente a la terminación relacionada
    {
        E_TF_SINESTADO = 1,
        E_TF_NULL, ///< JALR
        E_TF_INICIAL, ///< Libre
        E_TF_LLAMADO, ///< Llamada pasiva (Recibiendo llamada)
        E_TF_CONECTANDO, ///< NO USADO EN ESTADISTICAS
        E_TF_LLAMANDO, ///< Llamada activa (Llamando)
        E_TF_CONVERSACION, ///< Conversacion
        E_TF_RETENIDA, ///< Consulta pasiva (Lo han dejado retenido)- Llamadas retenidas por otro puesto
        E_TF_HOLD, ///< Consulta activa (Deja algún retenido) - Llamadas retenidas por el puesto local
        E_TF_TRANSITO, ///< NO USADO EN ESTADISTICAS
        E_TF_ESCUCHANDO, ///< Escucha activa (Escuchando)
        E_TF_ESCUCHADO, ///< Escucha pasiva (Escuchado)
        E_TF_PARK_ACT, ///< Park activo (Deja a alguno aparcado)
        E_TF_PARK_PAS, ///< Park pasivo (Lo han dejado aparcado)
        E_TF_INTERVENIDO,
        E_TF_ANULANDO, ///< Anulación activa (El usuario ha pulsado tecla ANUL)
        E_TF_ANULADO, ///< Anulación activa (El usuario ha pulsado tecla ANUL)
        E_TF_INTRUA = 30, ///< Intrusión activa (Intruyendo) (0x05)
        E_TF_INTRUP, ///< Intrusión pasiva (Intruido) (0x06)
        E_TF_LLACON, ///< Llamada en consulta (0x0A)
        E_TF_CONCON, ///< Conversación en consulta (0x0B)
        E_TF_OCUPADO, ///< Ocupado (Descolgado) (0x0C)
        E_TF_INTERVENCION, ///< El origen interviene a destino (0x0D)
        E_TF_INTVDO, ///< El destino es intervenido (0x0E)
        E_TF_FACIL_ACTIVA, ///< Facilidad activa
        E_TF_REDIR_ACTIVA, ///< Quien pulsa la tecla redir se pone en este subestado
        E_TF_REDIR_PASIVA, ///< Quien recibe la redireccion se pone en este estado
        E_TF_CI_CIPL_A, ///< Intrusion GET_CIPL en canal instructor (0x10)
        E_TF_CI_CIPL_P, ///< Intrusion GET_CIPL en canal intruido (0x11)
        E_TF_CI_NOTI_A, ///< Intrusion CI_NOTI en canal intructor (0x12)
        E_TF_CI_NOTI_P, ///< Intrusion CI_NOTI en canal intruido (0x13)
        E_TF_CI_CONV_A, ///< Intrusion CI_CONV en canal intructor (0x14)
        E_TF_CI_CONV_B, ///< Intrusion CI_CONV en canal intruido (0x15)
        E_TF_PET_CIPL, ///< Marca de peticion CIPL en canal instructor (0x15)
        E_TF_IA_TRANSTXLC, ///< (0x1A)
        E_TF_IA_TXLC, ///< (0x1B)
        E_TF_IA_RXLC ///< (0x1C)
    };

    enum E_SUBESTADO /// Da indicación adicional, sobre el estado de la comunicación.
            /// - Los tres primeros subestados, acompañan al estado de LIBRE, para indicar que una llamada (LLAMAA) se anuló, por cualquiera de ellos.
            /// - El estado INTV acompaña al estado de CONVER para indicar que tal línea, esta en estado de posible intervención (requisada).
            /// Subestados especiales de QSIG:
            /// Subestados para intrusión QSIG:
            /// Subestados aplicables a LC QSIG (SS-IA):
    {
        NINGUNO = 0, ///< No hay subestado
        OCUP = 0x04, ///< terminal remoto ocupado
        CONG = 0x05, ///< teminal de salida congestionado
        FS = 0x06, ///< terminal remoto fuera de servicio
        RECH = 0x07, ///< rechazo
        INTV = 0x09, ///< intervencion
        RELLAMADA = 0x0A, ///< rellamada
        OCU_EXT = 0x0B, ///< ocupado externo
        CCAN_ATS = 0x0F, ///< Cambio de canal QSIG en saliente
        REEN_ATS = 0x13, ///< Reecaminamiento de entrante
        PROG_ATS = 0x11, ///< progress
        ALERT_ATS = 0x12, ///< Alerting
        INTRUS_ATS = 0x15, ///< Intrusion QSIG
        EXPCIT5_ATS = 0x16, ///< Indicación de expiración del timer T5
        INTRUS_FAIL = 0x17, ///< Indicación de intrusión fallida
        INTRUS_COMP = 0x18,
        TXLC = 0x20,
        RXLC = 0x21,
        OCUTXLC = 0x22,
        ANUTXLC = 0x23,
        ANURXLC = 0x24,
        OCURXLC = 0x25,
        NOTECLC = 0x26
    };

    enum E_SERVICIO /// SERVICIO (RD/AD/LC) (1 Byte)
    {
        S_TLF = 2, ///< Llamada TLF tanto AD o AI
        S_LC = 3,
        S_AD = 4, ///< Llamada TLF por AD
        S_AI = 5 ///< Llamada TLF por AI
    };

    enum E_TIPO_PRIORIDAD_R2 /// Prioridad de la llamada. Numérico.
    {
        PRIORIDAD_PRIO = 1,
        PRIORIDAD_ALTA = 2,
        PRIORIDAD_MEDIA = 3,
        PRIORIDAD_BAJA = 4,
        PRIORIDAD_PRIO_TRANS = 6,
        PRIORIDAD_ALTA_TRANS = 7,
        PRIORIDAD_MEDIA_TRANS = 8,
        PRIORIDAD_BAJA_TRANS = 9
    };

    enum E_TIPO_RUTA_EST /// Nos indica el tipo de ruta de la llamada, si es interna, directa, alternativa o es tránsito.
    {
        SIN_RUTA_EST = 0, ///< Sin Ruta definida
        RUTA_LOCAL_EST = 1, ///< Ruta Local
        RUTA_ATSDIRECTA_EST = 3, ///< Ruta ATS Directa
        RUTA_ATSALTERNATIVA_EST = 4, ///< Ruta ATS Alternativa
        RUTA_TRANSITO_EST = 7 ///< Ruta transito
    };

    enum E_ESTADO_CONFIG /// Estado de la configuración
    {
        E_ESTADO_NO_VALIDADA = 1, ///< Configuración no validada
        E_ESTADO_VALIDADA = 2, ///< Configuración validada
        E_ESTADO_VERSIONADA = 3, ///< Configuración versionada (bloqueada)
        E_ESTADO_BORRADA = 4 ///< Configuración eliminada
    };

    enum E_PROCEDENCIA_SECTORIZ /// Procedencia sectorizacion (local, del propio TMCS) o remota por envío de un agente externo
    {
        E_SECT_LOCAL = 1,
        E_SECT_REMOTA = 2
    };

    enum TIPO_TMCS
    {
        TIPO_CLIENTE = 'C',
        TIPO_SERVIDOR = 'S'
    };

    enum E_ESTADO_ENLACE_SACTA
    {
        E_ESTADO_ENLACE_SACTA_CONECTADO = 1, ///< Enlace conectado
        E_ESTADO_ENLACE_SACTA_DESCONECTADO = 2 ///< Enlace desconectado
    };

    enum E_ESTADO_LAN
    {
        E_ESTADO_LAN_CONECTADO = 1, ///< LAN conectado
        E_ESTADO_LAN_DESCONECTADO = 2 ///< LAN desconectado
    };

    enum E_MODO_SAM
    {
        MED_AUDIO = 1, ///< Realizando medidas de audio analógico
        MED_RED = 2, ///< Realizando medidas de red
        REPOSO = 3 ///< No está realizando ninguna medida
    };

    enum E_TIPO_AUDIO_RED
    {
        TIPO_AUDIO = 1, ///< Audio analógico
        TIPO_RED = 2 ///< Red
    };

    enum E_VALOR_OPTIMO /// Valor deseado para la medida
    {
        MINIMO = 1, ///< La medida es mejor cuanto menor sea
        MAXIMO = 2 ///< La medida es mejor cuanto mayor sea
    };

    enum E_TIPO_CR
    {
        MSK_RX = 0x40, ///< Máscara de selección de recepción
        MSK_SQ = 0x20, ///< Máscara de squelch
        MSK_TX = 0x10, ///< Máscara de transmisión
        MSK_MTX = 0x08, ///< Máscara de múltiple TX
        MSK_PTT = 0x04, ///< Máscara de PTT
        MSK_RTX = 0x02, ///< Máscara de retransmisión
        MSK_PTT_FICT = 0x01 ///< Máscara de PTT ficticio
    };

    enum E_RESULTADO /// Clasificación de resultados obtenidos (posición/interfaz)
    {
        E_RESULTADO_OPTIMO = 1, ///< Todas las medidas realizadas han tenido resultado OPTIMO
        E_RESULTADO_BUENO = 2, ///< Al menos un resultado es BUENO y ninguno MALO
        E_RESULTADO_MALO = 3, ///< Al menos un resultado es MALO
        E_RESULTADO_NO_RES = 4 ///< Ninguna de las medidas tiene umbral definido
    };

    enum E_RES_MEDIDA /// Igual que E_RESULTADO pero se define de nuevo porque tiene distinto significado
    {
        E_RES_MEDIDA_OPTIMO = 1, ///< Resultado mejor que el umbral (si hay 1 umbral) y mejor que el umbral óptimo (si hay 2 umbrales)
        E_RES_MEDIDA_BUENO = 2, ///< Resultado entre ambos umbrales (si hay 2 umbrales)
        E_RES_MEDIDA_MALO = 3, ///< Resultado peor que el umbral (si hay 1 umbral) y peor que ambos umbrales (si hay 2 umbrales)
        E_RES_MEDIDA_NO_RES = 4 ///< Medida sin umbrales
    };

    enum E_ID_CANAL_BUCLE
    {
        CANAL_1 = 1, ///< Bucle del canal 1 de una tarjeta ISDN o QSIG
        CANAL_2 = 2, ///< Bucle del canal 2 de una tarjeta ISDN o QSIG
        CANAL_3 = 3, ///< Bucle del canal 3 de una tarjeta QSIG
        CASCOS_1 = 4, ///< Bucle de la salida de audio de los Jacks 1, del controlador
        CASCOS_2 = 5, ///< Bucle de la salida de audio de los Jacks 2, del coordinador
        ALTAVOZ_RAD = 6, ///< Bucle del altavoz de radio
        ALTAVOZ_AI = 7, ///< Bucle del altavoz de telefonía
        MICRO_1 = 8, ///< Bucle de la entrada de audio de los Jacks 1, del controlador
        MICRO_2 = 9 ///< Bucle de la entrada de audio de los Jacks 2, del coordinador
    };

    enum E_TIPO_MEDIDA
    {
        ATENUACION = 1, ///< Atenuación en función de la frecuencia
        RUIDO = 2, ///< Ruido en canal en reposo
        DISTORSION = 3, ///< Distorsión
        RETARDO_INTERNO = 4, ///< Retardo Interno (entre posiciones)
        RETARDO_EXTERNO = 5, ///< Retardo Externo (posición-interfaz)
        PESQ = 6, ///< Índice eléctrico de calidad de voz (PESQ LQO)
        TAM_RECORTES = 7, ///< Tamaño Recortes temporales de segmentos de voz
        NUM_RECORTES = 8, ///< Cantidad de Recortes temporales de segmentos de voz
        ECO = 9, ///< Eco
        DIAFONIA = 10, ///< Diafonía
        IPDV_ANALOGICO = 11, ///< IPDV (Inter Packet Delay Variation) analógico
        IPDV_DIGITAL = 12, ///< IPDV (Inter Packet Delay Variation) digital (de tramas RTP)
        PAQUETES_PERDIDOS = 13 ///< Paquetes perdidos
    };

    enum E_ESTADO_FTP
    {
        E_ESTADO_FTP_ACTIVO = 1, ///< Estado ACTIVO
        E_ESTADO_FTP_NO_ACTIVO = 2 ///< Estado NO ACTIVO
    };

    enum E_ESTADO_GENERAL /// Estado general del componente (web, BD, server)
    {
        E_ESTADO_GENERAL_ACTIVO = 1, ///< Estado ACTIVO
        E_ESTADO_GENERAL_NO_ACTIVO = 2 ///< Estado NO ACTIVO
    };

    enum E_ESTADO_CLUSTER /// Estado del cluster
    {
        E_ESTADO_CLUSTER_ACTIVO = 1, ///< Estado ACTIVO
        E_ESTADO_CLUSTER_NO_ACTIVO = 2, ///< Estado NO ACTIVO
        E_ESTADO_CLUSTER_DEGRADADO = 3, ///< Estado DEGRADADO
        E_ESTADO_CLUSTER_FALLO = 4 ///< Estado FALLO
    };

    enum E_ESTADO_BACKUP /// Estado del backup de la BD
    {
        E_ESTADO_BACKUP_OK = 1, ///< Estado ACTIVO
        E_ESTADO_BACKUP_NO_ACTUALIZADO = 2, ///< Estado NO ACTUALIZADO
        E_ESTADO_BACKUP_FALLO = 3 ///< Estado FALLO
    };

    enum E_MEDIDAS_RED /// Medidas de red que puede realizar el SAM
    {
        IPDV = 0, ///< IPDV Digital
        PAQPERDIDOS = 1 ///< Paquetes perdidos
    };

    enum E_ESTADO_POSICION /// Estado de la posición en el SAM
    {
        NO_OK = 0, ///< Posición en modo DESACTIVADA
        OK = 1 ///< Posición en modo ACTIVADA
    };

    enum E_ON_OFF /// Para activar o desactivar los modos patrón en posiciones y medición en posiciones e interfaces
    {
        OFF = 0, ///< Desactivacion
        ON = 1 ///< Activacion
    };

    enum E_ABONADO_SAM /// Tipo de abonado SAM: MEDIDAS o PATRÓN
    {
        ABONADO_NULO = 0,
        PATRON = 1, ///< PATRÓN
        MEDIDAS = 2 ///< MEDIDAS
    };

    enum E_TIPO_ERROR_SAM /// Tipos de error y motivos de rechazo
    {
        TIMEOUT_POSICION = 1, ///< Error timeout TIMOUT_SET_POS_MEDICION superado. Activacion modo medición
        SECTORIZADA = 2, ///< Error la posición ya estaba sectorizada. Activacion modo medición / patrón
        FALLO_SECTORIZACION = 3, ///< Error la posición no se pudo sectorizar o des-sectorizar con el usuario ID_USR_MEDIDAS o ID_USR_PATRON
        FALLO_REGISTRO = 4, ///< Error la posición no se pudo registrar o des-registrar en el proxy. Activacion modo medición / patrón
        TIMEOUT_PATRON = 5, ///< Error timeout TIMOUT_SET_POS_PATRON superado. Activacion modo patrón
        ERROR_PRESENCIAS = 6, ///< Error no se reciben presencias del gateway que contiene la interfaz. Activacion / desactivacion modo medición o analisis de audio
        ERROR_ESTADO_TARJETA = 7, ///< Error las presencias del GW indican que la tarjeta esta fuera de servicio. Análisis audio interfaz
        ERROR_TIPO = 8, ///< Error las presencias del GW indican que el tipo de la tarjeta que está en el GW no coincide con el tipo de la tarjeta configurada. Análisis audio interfaz
        TIMEOUT_INTERFAZ = 9, ///< Error timeout TIMOUT_SET_INT_MEDIDAS superado. Análisis audio interfaz
        NO_ACEPTADO = 10, ///< Error la petición de cambio no se ha aceptado. Análisis audio interfaz
        FALLO_LLAMADA_SAM = 11, ///< Error en el establecimiento de la llamada SAM. Análisis audio posición / interfaz
        FALLO_DES_LLAMADA_SAM = 12, ///< Error en la finalizacion de la llamada SAM. Análisis audio posición / interfaz
        FALLO_ACTIVACION_BUCLE = 13, ///< Error en la activacion de un bucle en la posición. Análisis audio posición
        FALLO_DES_ACTIVACION_BUCLE = 14, ///< Error en la desactivacion de un bucle en la posición. Análisis audio posición
        TIMEOUT_MEDIDAS = 15, ///< Error timeout TIMOUT_SET_POS_MEDIDAS superado. Análisis audio posición
        ERROR_ESTADO = 16, ///< Error no se reciben presencias de la posición. Activacion / desactivacion modo medición / patrón o análisis de audio
        SIN_RECHAZO = 19, ///< Respuesta cuando la posición se ha activado en modo Patron correctamente. Activacion posición patrón
        OTROS = 20 ///< Error otros motivos de rechazo
    };



    struct MSG_EST_CONFIG /// Mensajes de estado de las configuraciones - MSG_EST_CONFIG
    {
        uint32 idConfig;
        uint32 subIdConfig;
        int8 nombreConfig[NMAX_NOMBRE_CONFIG]; ///< Nombre de la configuración
        int8 version[NMAX_VERSION_CONFIG]; ///< Versión de la configuración
        E_ESTADO_CONFIG estado;
    } TIGHLY_PACKED;

    struct IDENT_EQUIPO
    {
        E_TIPO_EQUIPO tipoEq;
        uint32 id;
    } TIGHLY_PACKED;

    struct CAB_MSG_TH /// Cabecera de todos los mensajes
    {
        uint16 lng; ///< Longitud mensaje incluyendo este campo. (2 bytes)
        E_TIP_MSG tipoMsg; ///< Peticion, respuesta, notificacion (4 bytes)
        E_ID_MSG idMsg; ///< Identificacion de mensaje (4 bytes)
        IPv4 dirOrigen; ///< Direccion IP del servidor SAM (4 bytes)
        IPv4 dirDestino; ///< Direccion de Destino (4 bytes)
        IDENT_EQUIPO idOrigen; ///< Identificador equipo origen
        IDENT_EQUIPO idDestino; ///< Identificador equipo destino
        uint16 portDestino; ///< Puerto destino (2 bytes)
        uint32 numMensaje; ///< Identifica a cada mensaje(este numero es distinto para cada mensaje) (4 bytes)
    } TIGHLY_PACKED;

    struct SUP_PUES /// Puesto
    {
        uint32 idAbonado; ///< Id del abonado
        int8 nomAbonado[MAX_NOMBRE_ABONADO]; ///< Nombre del abonado
        Enum<E_SINO, uint8> estJacksCont; ///< Estado jacks controlador (SI/NO)
        Enum<E_SINO, uint8> estJacksCoord; ///< Estado jacks coordinador (SI/NO)
        Enum<E_SINO, uint8> estPttCont; ///< Estado PTT controlador (SI/NO)
        Enum<E_SINO, uint8> estPttCoord; ///< Estado PTT coordinador (SI/NO)
        Enum<E_SINO, uint8> servRad; ///< Servicio radio (SI/NO)
        Enum<E_SINO, uint8> servTlf; ///< Servicio telefonía (SI/NO)
        Enum<E_SINO, uint8> servLC; ///< Servicio linea caliente (SI/NO)
        Enum<E_SINO, uint8> sectorizado; ///< Indica si el puesto esta sectorizado (SI/NO)
    } TIGHLY_PACKED;

    struct ESTADO_HW_GENERAL
    {
        E_TIPO_PROC tipoGW_68; ///< Tipo procesador 68000 (A = 0 o B = 1)
        uint8 numProc; ///< Numero de procesador del 68000
        E_EST_OPERATIVO estadoGW_68; ///< Estado del procesador 68000
        E_EST_OPERATIVO estadoGW_68Dual; ///< Estado del procesador 68000 dual
    } TIGHLY_PACKED;

    struct ESTADO_HW_ENLACE
    {
        Enum<E_TIPO_ENLACE, uint8> tipo; ///< Tipo de enlace
        Enum<E_ESTADO_ENLACE, uint8> estado; ///< Estado del enlace
    } TIGHLY_PACKED;

    struct ESTADO_SUPERVISION_ENLACE
    {
        uint16 id; ///< Id de enlace
        uint8 estado; ///< Estado del enlace
    } TIGHLY_PACKED;

    struct SUP_GW /// Gateway
    {
        E_EST_OPERATIVO ppal_rsva; ///< Estado principal / reserva
        uint8 proc; ///< Estado procesador (Ver + abajo su descripción)
        uint8 alarm; ///< Estado alarmas procesador (Ver + abajo su descripción)
        ESTADO_HW_GENERAL estHw;
        ESTADO_HW_ENLACE enlace[NUM_ENLACES_GW]; ///< Estado de enlaces del GW (Solo para presencias GW)
    } TIGHLY_PACKED;

    struct SUP_TMCS /// TMCS only used in Linux OS
    {
        uint16 idConfig; ///< ID_CONFIG (2 Bytes)
        uint16 subIdConfig; ///< SUB_ID_CONFIG (2 Bytes)
        uint16 idEmerge; ///< ID_EMERGENCY (2 Bytes)
        uint16 idSectorisation; ///< ID_SECTORISATION (2 Bytes)
    } TIGHLY_PACKED;

    struct MSG_CONFIG_IMPLANTADA
    {
        uint32 numSesion; ///< Numero de sesion
        int8 nomConfig[NMAX_NOMBRE_CONFIG]; ///< Nombre de la configuración
        uint32 id; ///< id de la configuración
        uint32 subId; ///< subid de la configuración
        int8 actual; ///< (1) indica configuración actual, (0) indica configuración de emergencia
    } TIGHLY_PACKED;

    struct SUP_FICH_CONF
    {
        int32 id; ///< identificador de la configuración
        int32 subId; ///< identificador auxiliar de la configuración
        uint32 CRC; ///< CRC del fichero de configuración descomprimido
    } TIGHLY_PACKED;

    struct INF_CRC_FICHEROS
    {
        SUP_FICH_CONF supFichConfHw; ///< Información de ID, subID y CRC del fichero de config
        SUP_FICH_CONF supFichConfLog; ///< Información de ID, subID y CRC del fichero de config
        SUP_FICH_CONF supFichConfLogSectA; ///< Información de ID, subID y CRC del fichero de config. LogSectA
        SUP_FICH_CONF supFichConfLogSectB; ///< Información de ID, subID y CRC del fichero de config. LogSectB
    } TIGHLY_PACKED;

    struct MSG_SUPER
    {
        uint32 id; /// Id de puesto, gateway o enlace
        int8 nomPuesto[MAX_NOMBRE_PUESTO]; /// Nombre del puesto
        uint8 estado; /// Estado TMCS A(Activo) R(Reserva) C(Cliente) 00h(Activo)
        union
        {
            SUP_GW gw; ///< Parte de supervision especifica gateway
            /// CAB_MSG_TH::idOrigen.tipoEq ///< [Usado cuando el miembro pMSG_TH->cab.idOrigen.tipoEq es EQ_GATEWAY]
            ESTADO_SUPERVISION_ENLACE enlace[NUM_ENLACES_GW]; ///< Parte de supervision especifica puestos ///< [Usado cuando el miembro pMSG_TH->cab.idOrigen.tipoEq es EQ_NULO]
            SUP_TMCS tmcs; ///< Parte de supervision especifica del TMCS !isWindows() ///< [Usado cuando el miembro pMSG_TH->cab.idOrigen.tipoEq es EQ_SUPERV /*&& !isWindows()*/]
            SUP_TMCS tmcsr; ///< Parte de supervision especifica del TMCS -- !isWindows() ///< [Usado cuando el miembro pMSG_TH->cab.idOrigen.tipoEq es EQ_SUPERR /*&& !isWindows()*/]
            SUP_PUES pues; ///< Parte de supervision especifica puestos
        } TIGHLY_PACKED;

        int8 sistRd; /// Valores: SISTEMA_A = 'A', SISTEMA_B = 'B', SISTEMA_AYB = 'C'
        int8 sistTf; /// Valores: SISTEMA_A = 'A', SISTEMA_B = 'B', SISTEMA_AYB = 'C'
        Enum<E_SINO, uint8> mantenimiento; /// Estado puesto en mantenimiento - 1 (Si), 0 (No)
        int8 estadoConfigurado; /// Indica que tipo de TMCS es. Puede ser ‘S’ (para servidor, sea principal o reserva) o ‘C’ (para cliente).
        Enum<E_SINO, uint8> estadoSectorizado; /// Indica si el gw ha recibido la sectoriz o no - 1(si) 0(no)
        Enum<E_SINO, uint8> estadoSelFreqEmer; /// Est de seleccion de frecuencias de emergencia - 1(si) 0(no)
        uint8 idGwSimulado; /// 0 (No gwsimu) Los puestos y los gateways dicen si hace de gwSimulados y el id del gwSimulado
        INF_CRC_FICHEROS infCrcEmer;
        INF_CRC_FICHEROS infCrcAct;
        uint8 estEthernet0; /// 0 Estado de la boca de LAN 0(2 -> conectado, !=2 -> desconectado)
        uint8 estEthernet1; /// 0 Estado de la boca de LAN 1(2 -> conectado, !=2 -> desconectado)
    } TIGHLY_PACKED;

    struct PUESTOS_INTEGRANTES
    {
        uint32 numero; ///< Numero puestos que integran el ejercicio
        uint8 idPuesto[NMAX_PUESTOS]; ///< [Limitado por el miembro ´numero´] ///< Puestos
    } TIGHLY_PACKED;

    struct GATEWAYS_INTEGRANTES
    {
        uint32 numero; ///< Numero gateways que integran el ejercicio
        uint8 idGateway[NMAX_GATEWAYS]; ///< [Limitado por el miembro ´numero´] ///< gateways
    } TIGHLY_PACKED;

    struct DAT_FICH_CONFIG
    {
        uint32 numBloques; ///< Numero total de bloques (n)
        uint32 numBytes; ///< Numero de bytes del fichero de configuración
        uint32 bloque; ///< Numero de bloque tx/rx (1 .... n)
        uint32 numBytesBloque; ///< Numero de bytes del bloque tx/rx (por defecto 256 bytes, excepto el ultimo)
        int8 nombreFichero[NMAX_NOMBRE_FICHERO];
        uint32 crcFichero; ///< CRC del fichero que se va a enviar
        int32 idFichero;
        int32 subidFichero;
    } TIGHLY_PACKED;

    struct MSG_CONFIG_INF_FICH_CONFIG
    {
        uint32 numSesion; ///< Número de sesion
        DAT_FICH_CONFIG fich;
        PUESTOS_INTEGRANTES puestosIntegrantes;
        GATEWAYS_INTEGRANTES gatewaysIntegrantes;
    } TIGHLY_PACKED;

    struct MSG_CONFIG_FICH_CONFIG
    {
        uint32 numSesion; ///< Numero de sesion
        uint32 fichCorrecto; ///< En la respuesta del fichero de configuración: * True (1): Bien, se ha recibido bien el fichero * False (0): Mal, se ha recibido mal el fichero
        DAT_FICH_CONFIG fich;
        uint8 byte[NMAX_BYTES_BLOQUE]; ///< Bytes del fichero
    } TIGHLY_PACKED;

    struct MSG_ENVIA_TABLA /// Subestructura de mensaje de envío de tabla de sectorización
            /// Para mensajes M_ENVIA_TABLA
    {
        int8 nombreTabla[NMAX_NOMBRE_TABLA];
        int8 sistema;
        uint8 numBloques; ///< Numero de bloques total del mensaje
        uint8 bloque; ///< Numero que identifica a este mensaje
        uint8 bufferTabla[NMAX_TAM_TABLA];
    } TIGHLY_PACKED;

    struct MSG_ARRANQUE_CONFIG /// Subestructura de mensaje de arranque
    {
        uint32 numSesion; ///< Numero de sesion
        INF_CRC_FICHEROS infCrc; ///< Información de Id y CRC de los ficheros
        PUESTOS_INTEGRANTES puestosIntegrantes;
        GATEWAYS_INTEGRANTES gatewaysIntegrantes;
        int8 sistRd;
        int8 sistTf;
    } TIGHLY_PACKED;

    struct MSG_SECTORIZA /// Subestructura del mensaje de sectorización
            /// Para mensaje M_SECTORIZA
    {
        uint32 idSectorizacion; ///< Numero que identifica la sectorizacion implantada
        E_PROCEDENCIA_SECTORIZ procedencia; ///< Procedencia sectorizacion
        E_MOTIVO_SECTORIZ motivo; ///< Motivo por el que se envía la sectorización
        uint8 Estado; ///< Sectorizacion Aceptada(1) o Rechazada (0)
        int8 causa[NMAX_CARAC_CAUSA]; ///< Causa de rechazo de la sectorización
    } TIGHLY_PACKED;

    struct SAM_timeb
    {
        time_t time; ///< In Windows, define _USE_32BIT_TIME_T for the type to be a 32 bits integer
        uint16 millitm;
        int16 timezone;
        int16 dstflag;
    } TIGHLY_PACKED;

    struct MSG_DATETIME_SG /// Subestructura mensaje de fecha y hora
    {
        SAM_timeb datetime;
    } TIGHLY_PACKED;

    struct ABO_PUE_TEL_EST /// Subestructura de los mensajes de comunicaciones de telefonía y radio
    {
        Enum<E_TIPO_EQUIPO_EST, uint8> tipoEq; /// TIPO EQUIPO (PUES o GW) (1 Byte)
        union
        {
            Enum<E_TIPO_GATEWAY, uint8> tipoGw; ///< TIPO de GATEWAY (1 Byte) ///< [Usado cuando el miembro tipoEq es EQ_EST_GATEWAY]
            Enum<E_TIPO_PUESTO, uint8> tipoPues; ///< TIPO de PUESTO (1 Byte)
        } TIGHLY_PACKED;

        uint16 idEq; /// ID EQUIPO(T_PUESTOS/T_GATEWAYS) (2 Bytes) ///< [Toma los valores de la tabla ]
        union
        {
            uint8 NumMicroSwitchGw; ///< NUMERO MICROSWITH para GW (1 Byte) ///< [Usado cuando el miembro tipoEq es EQ_EST_GATEWAY]
            Enum<E_TIPO_ABONADO_INTERNO, uint8> tipoAbon; ///< TIPO ABONADO para puestos (1 Byte)
        } TIGHLY_PACKED;

        union
        {
            uint16 idEnl; ///< TARJETA GATEWAY (2 Bytes) ///< [Toma los valores de la tabla ]  ///< [Usado cuando el miembro tipoEq es EQ_EST_GATEWAY]
            uint16 idAbon; ///< ID ABONADO (2 Bytes) ///< [Toma los valores de la tabla ]
        } TIGHLY_PACKED;

        int8 strAbon[LNG_NOM_ABON_EST]; /// NOMBRE ABONADO (20 Bytes)
        uint8 slotGw; /// SLOT TARJETA GATEWAY (1 Byte)
        union
        {
            Enum<E_TIPO_ENLACE, uint8> tipoTarjetaGw; ///< TIPO DE TARJETA GW (1 Byte)
            /// Con el tipo de tarjeta se determinará el tipo de red a través del que se hace una llamada:
            /// - Redes PSTN (públicas) -> EYMDTMF, PABX, E1, BRI, FXO, ISDN_ST
            /// - Redes AGVN (red entre centros de control) -> R2, QSIG, N5
            /// - Redes AGVN punto a punto (TFU/FXS, BL, BC, EYM, LCE) ///< [Usado cuando el miembro tipoEq es EQ_EST_GATEWAY]
            uint8 tipoAbonSimple_Integrado; ///< ABONADO SIMPLE/INTEGRADO (1 Byte)
        } TIGHLY_PACKED;

        uint16 idCentro; /// ID CENTRO (2 Bytes). Identificador del centro(o red) origen/destino de la llamada ///< [Toma los valores de la tabla ]
        uint8 pref[LNG_PREF_EST]; /// PREFIJO (5 Bytes). Prefijo abonado origen/destino de la llamada (Puede ser un campo vacio)
        int8 codigo[N_MAX_DIG_COD]; /// CODIGO (20 Bytes). Código abonado origen/destino de la llamada (puede ser el ATS o el público)
        Enum<E_EST_TF, uint8> estComu; /// ESTADO DE LA COMUNICACION (1 Byte)
        Enum<E_EST_TF, uint8> estComuAnt; /// ESTADO DE LA COMUNICACION ANTERIOR (1 Byte)
        Enum<E_SUBESTADO, uint8> subEst; /// SUBESTADO ORIGEN (1 Byte)
    } TIGHLY_PACKED;

    struct MSG_EST_COMU_TLF
    {
        Enum<E_SUB_ID_MSJ_TLF, uint8> subIdMens; ///< SUBIDENTIFICADOR MENSAJE(1 Byte)
        Enum<E_SISTEMA_ABC, int8> sistema; ///< SISTEMA (A/B/C) (1 Byte)
        uint16 idConfig; ///< ID_CONFIG (2 Bytes)
        uint16 subIdConfig; ///< SUB_ID_CONFIG (2 Bytes)
        uint8 sesion; ///< SESIÓN (1 Byte)
        ABO_PUE_TEL_EST org; ///< Origen llamada
        ABO_PUE_TEL_EST des; ///< Destino llamada
        Enum<E_SERVICIO, uint8> servicio; ///< (AD/AI/LC) (1 Byte)
        Enum<E_TIPO_PRIORIDAD_R2, uint8> prioridad;
        Enum<E_TIPO_RUTA_EST, uint8> tipoRuta;
    } TIGHLY_PACKED;

    struct ABO_PUE_RD_EST /// Mensajes estadisticas de comunicaciones radio
    {
        Enum<E_TIPO_EQUIPO_EST, uint8> tipoEq; /// TIPO EQUIPO (PUES o GW) (1 Byte)
        union
        {
            Enum<E_TIPO_GATEWAY, uint8> tipoGw; ///< TIPO de GATEWAY (1 Byte) ///< [Usado cuando el miembro tipoEq es EQ_EST_GATEWAY]
            Enum<E_TIPO_PUESTO, uint8> tipoPues; ///< TIPO de PUESTO (1 Byte)
        } TIGHLY_PACKED;

        uint16 idEq; /// ID EQUIPO(PUESTO/GATEWAY) (2 Bytes) ///< [Toma los valores de la tabla ]
        union
        {
            uint8 NumMicroSwitchGw; ///< NUMERO MICROSWITH para GW (1 Byte) ///< [Usado cuando el miembro tipoEq es EQ_EST_GATEWAY]
            Enum<E_TIPO_ABONADO_INTERNO, uint8> tipoAbon; ///< TIPO ABONADO para puestos (1 Byte)
        } TIGHLY_PACKED;

        uint16 idAbon; /// ID ABONADO (2 Bytes) ///< [Toma los valores de la tabla ]
        uint8 tipoAbonSimple_Integrado; /// TIPO DE TARJETA GW/ABONADO SIMPLE/INTEGRADO (1 Byte)
        int8 strAbon[LNG_NOM_ABON_EST]; /// NOMBRE ABONADO (20 Bytes)
    } TIGHLY_PACKED;

    struct CANAL_EST
    {
        uint16 idFrec; ///< ID FRECUENCIA n (2 Bytes) ///< [Toma los valores de la tabla ]
        uint16 idEmpl; ///< ID EMPLAZAMIENTO n (2 Bytes) ///< [Toma los valores de la tabla ]
        uint16 idGw; ///< ID GATEWAY n (2 Bytes) ///< [Toma los valores de la tabla ]
        uint16 idTarj; ///< ID TARJETA n (2 Bytes) ///< [Toma los valores de la tabla ]
        uint8 slot; ///< SLOT TARJETA n (1 Byte)
        Enum<E_TIPO_ENLACE, uint8> tipoTarj; ///< TIPO TARJETA n (CR, CR_BSS, CR_BSS2) (1 Byte)
        uint8 estado1; ///< ESTADO1 FRECUENCIA n (1 Byte)
        uint8 estado2; ///< ESTADO2 FRECUENCIA n (1 Byte)
    } TIGHLY_PACKED;

    struct CANAL_BSS_EST
    {
        uint16 idFrec; ///< ID FRECUENCIA n (2 Bytes) ///< [Toma los valores de la tabla ]
        uint16 idEmpl; ///< ID EMPLAZAMIENTO n (2 Bytes) ///< [Toma los valores de la tabla ]
        uint16 idTarj; ///< ID TARJETA n (2 Bytes) ///< [Toma los valores de la tabla ]
        uint8 slot; ///< SLOT TARJETA n (1 Byte)
        Enum<E_TIPO_ENLACE, uint8> tipoTarj; ///< TIPO TARJETA n (CR, CR_BSS, CR_BSS2)
        uint16 calFrec; ///< CALIDAD FRECUENCIA n (2 Bytes)
        uint16 RelSenalRuido; ///< RELACION SNR FRECUENCIA n (2 Bytes)
    } TIGHLY_PACKED;

    struct MSG_EST_COMU_RAD
    {
        Enum<E_SUB_ID_MSJ_RAD, uint8> subIdMens; /// SUBIDENTIFICADOR MENSAJE RADIO(1 Byte)
        Enum<E_SISTEMA_ABC, int8> sistema; /// SISTEMA (A/B/C) (1 Byte)
        uint16 idConfig; /// ID_CONFIG (2 Bytes)
        uint16 subIdConfig; /// SUB_ID_CONFIG (2 Bytes)
        uint8 sesion; /// SESIÓN (1 Byte)
        ABO_PUE_RD_EST org; /// Origen llamada
        uint8 estFuncion; /// ESTADO FUNCIÓN (1 Byte)
        uint8 numCanales; /// NÚMERO DE CANALES rellenados de la estructura siguiente(1 Byte)
        union
        {
            CANAL_BSS_EST canalBSS[NTITFZMU]; ///< [Limitado por el miembro ´numCanales´] ///< [Usado cuando el miembro subIdMens es CALIDAD_BSS]
            CANAL_EST canal[NTITFZMU]; ///< [Limitado por el miembro ´numCanales´]
        } TIGHLY_PACKED;

    } TIGHLY_PACKED;

    struct MSG_CONFIG_SESION
    {
        uint32 numSesion;
        INF_CRC_FICHEROS infCrc;
    } TIGHLY_PACKED;

    struct MSG_ENLACE_SACTA
    {
        E_ESTADO_ENLACE_SACTA estado;
    } TIGHLY_PACKED;

    struct MSG_MEDIDAS_RED /// Respuesta a la petición del listado de medidas de red
    {
        Enum<E_MEDIDAS_RED, uint8> ipdvDigital; ///< Medida IPDV Digital
        Enum<E_MEDIDAS_RED, uint8> paquetesPerdidos; ///< Medida Perdida de paquetes
    } TIGHLY_PACKED;

    struct MSG_ESTADO_RED
    {
        Enum<E_ESTADO_LAN, uint8> estadoLanGestion; ///< Estado de la LAN de Gestión (1 byte)
        Enum<E_ESTADO_LAN, uint8> estadoLanVoz; ///< Estado de la LAN de Voz (1 byte)
    } TIGHLY_PACKED;

    struct RES_MEDIDA /// La estructura RES_MEDIDA del resultado de pruebas de audio, con el resultado de una medida
    {
        Enum<E_TIPO_MEDIDA, uint8> id; ///< Identificador de la medida
        float valor; ///< Valor de la medida para el canal/bucle
        Enum<E_RES_MEDIDA, uint8> medida; ///< Clasificación del resultado
    } TIGHLY_PACKED;

    struct MEDIDAS_CANAL_BUCLE
    {
        Enum<E_ID_CANAL_BUCLE, uint8> tipoBucle; ///< Tipo de bucle o canal
        uint8 numMedidas; ///< Numero de medidas que se van a hacer en el bucle o canal
        Enum<E_TIPO_MEDIDA, uint8> medidas[MAX_MEDIDAS_CANAL_BUCLE]; ///< [Limitado por el miembro ´numMedidas´] ///< Numero de medidas que se van a hacer en el bucle o canal
    } TIGHLY_PACKED;

    struct MSG_INTERFAZ_BATERIA
    {
        uint32 idSesion; ///< Identificador de la sesión del usuario
        uint16 idBateria; ///< Identificador de la batería de pruebas a la que estan asociadas las pruebas de la posición
        uint16 idGW; ///< Identificador del GW al que pertenece el interfaz (2 bytes)
        uint16 idInterfaz; ///< Identificador del interfaz (2 bytes)
        uint8 slot; ///< Slot que ocupa el interfaz en el GW (1 byte)
        Enum<E_TIPO_ENLACE, uint8> tipo; ///< Tipo de interfaz sobre el que se realizó la medida (1 byte)
        uint8 numCanales; ///< Número de canales a medir en el interfaz
        MEDIDAS_CANAL_BUCLE canales[MAX_CANALES_INTERFAZ]; ///< [Limitado por el miembro ´numCanales´] ///< Estructura MED_CANAL repetida numCanales veces
    } TIGHLY_PACKED;

    struct MSG_POSICION_BATERIA
    {
        uint32 idSesion; ///< Identificador de la sesión del usuario
        uint16 idBateria; ///< Identificador de la batería de pruebas a la que estan asociadas las pruebas de la posición
        uint16 idPosicion; ///< Identificador de la posición
        uint8 numBucles; ///< Numero de bucles para los que se definen pruebas
        MEDIDAS_CANAL_BUCLE bucles[MAX_BUCLES_POSICION]; ///< [Limitado por el miembro ´numBucles´] ///< Estructura que contiene las medidas que se haran sobre los bucles
    } TIGHLY_PACKED;

    struct MSG_RED_SAM /// EvtSAM_EstadisticasDeMedidasRed.- El S-SAM envía las medidas realizadas de red al S-SeeSCV y al S-TMCs.
    {
        SAM_timeb fecha; ///< Fecha de inicio de la ejecución de las pruebas
        uint16 idCentro; ///< Identificador del centro en el que se ejecutó la BdP
        uint16 idConfig; ///< ID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        uint16 subIdConfig; ///< SubID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        uint8 numMedidas; ///< Número de medidas de red realizadas
        RES_MEDIDA medida[MAX_MEDIDAS_RED]; ///< [Limitado por el miembro ´numMedidas´] ///< Estructura RES_MEDIDA repetida numMedidas veces
    } TIGHLY_PACKED;

    struct MED_CANAL_BUCLE
    {
        Enum<E_ID_CANAL_BUCLE, uint8> id; ///< Id. del bucle
        uint8 numMedidas; ///< Número de medidas realizadas sobre el bucle
        RES_MEDIDA medida[MAX_MEDIDAS_CANAL_BUCLE]; ///< [Limitado por el miembro ´numMedidas´] ///< Estructura RES_MEDIDA repetida numMedidas veces
    } TIGHLY_PACKED;

    struct MED_POSICION
    {
        uint16 id; ///< Identificador de la posición (2 bytes)
        uint32 ip; ///< Dirección IP de la posición (4 bytes)
        Enum<E_RESULTADO, uint8> resultadoPos; ///< Clasificación del resultado general de la posición
        uint8 numBucles; ///< Número de bucles medidos en la posición
        MED_CANAL_BUCLE bucle[MAX_BUCLES_POSICION]; ///< [Limitado por el miembro ´numBucles´] ///< Estructura MED_CANAL_BUCLE repetida numBucles veces
    } TIGHLY_PACKED;

    struct MED_INTERFAZ
    {
        uint16 idInterfaz; ///< Identificador del interfaz (2 bytes)
        uint16 idGW; ///< Identificador de usuario del GW al que pertenece el interfaz (2 bytes)
        uint8 slot; ///< Slot que ocupa el interfaz en el GW (1 byte)
        uint8 tipo; ///< Tipo de interfaz sobre el que se realizó la medida (1 byte)
        Enum<E_RESULTADO, uint8> resultadoInt; ///< Clasificación del resultado general de la interfaz
        uint8 numCanales; ///< Número de canales medidos en el interfaz
        MED_CANAL_BUCLE canal[MAX_CANALES_INTERFAZ]; ///< [Limitado por el miembro ´numCanales´] ///< Estructura MED_CANAL_BUCLE repetida numCanales veces
    } TIGHLY_PACKED;

    struct MSG_AUDIO_SAM_POSICION
    {
        uint16 id; ///< Identificador de la Batería de Pruebas
        SAM_timeb fecha; ///< Fecha de inicio de la ejecución de las pruebas
        uint16 idCentro; ///< Identificador del centro en el que se ejecutó la BdP
        uint16 idConfig; ///< ID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        uint16 subIdConfig; ///< SubID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        MED_POSICION posicion; ///< Estructura con el resultado de las medidas realizadas para una posición
    } TIGHLY_PACKED;

    struct MSG_AUDIO_SAM_INTERFAZ
    {
        uint16 id; ///< Identificador de la Batería de Pruebas
        SAM_timeb fecha; ///< Fecha de inicio de la ejecución de las pruebas
        uint16 idCentro; ///< Identificador del centro en el que se ejecutó la BdP
        uint16 idConfig; ///< ID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        uint16 subIdConfig; ///< SubID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        MED_INTERFAZ interfaz; ///< Estructura con el resultado de las medidas sobre los interfaces
    } TIGHLY_PACKED;

    struct MSG_PRES_VRP
    {
        Enum<E_ESTADO_FTP, uint8> estadoFTP; ///< Estado del servidor FTP (1 byte)
        uint32 espacioTotal; ///< Espacio total en en MB (4 bytes)
        uint32 espacioOcupado; ///< Espacio ocupado en disco en MB (4 bytes)
    } TIGHLY_PACKED;

    struct MSG_PRES_ESTAD
    {
        Enum<E_ESTADO_GENERAL, uint8> estadoWeb; ///< Estado del servidor Web (1 byte)
        Enum<E_ESTADO_GENERAL, uint8> estadoBd; ///< Estado del servidor BD (1 byte)
        Enum<E_ESTADO_GENERAL, uint8> estadoServer; ///< Estado del servidor PHP (1 byte)
        Enum<E_ESTADO_CLUSTER, uint8> estadoCluster; ///< Estado del cluster de servidores (1 byte)
        Enum<E_ESTADO_BACKUP, uint8> estadoBackup; ///< Estado de los backups (1 byte)
        uint32 espacioTotal; ///< Espacio total en disco local o cabina de discos en MB (4 bytes)
        uint32 espacioOcupado; ///< Espacio ocupado en disco local o cabina de discos en MB (4 bytes)
    } TIGHLY_PACKED;

    struct DAT_INICIO_SESION_SAM
    {
        int8 rolUsuario[MAX_NOMBRE_ABONADO]; ///< Rol del ususario que inicia la sesion
    } TIGHLY_PACKED;

    struct MSG_BATERIA_POR_ID
    {
        uint32 idSesion; ///< Identificador de la sesion del usuario que hace la consulta de la bateria de pruebas
        uint16 idBateria; ///< Identificador de la bateria de pruebas que se quiere consultar
    } TIGHLY_PACKED;

    struct MSG_ENVIA_BATERIA_VACIA
    {
        uint32 idSesion; ///< Identificador de la sesion del usuario que hace la consulta de la bateria de pruebas
        int8 nombre[MAX_NOMBRE_BATERIA]; ///< Nombre de la bateria de pruebas a crear
        SAM_timeb fechaCreacion; ///< Fecha de creacion de la bateria de pruebas
        uint16 idCentro; ///< Identificador del centro en el que se ejecutó la BdP
        uint16 idConfig; ///< ID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        uint16 subIdConfig; ///< SubID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
    } TIGHLY_PACKED;

    struct MSG_ID_SESION
    {
        uint32 idSesion; ///< Identificador de la sesion del usuario para envio de peticiones al servidor SAM
    } TIGHLY_PACKED;

    struct M_UMBRAL_INTERVALO
    {
        float rMin; ///< Umbral inferior. Usar qNan para un valor nulo.
        float rMax; ///< Umbral superior. Usar qNan para un valor nulo.
    } TIGHLY_PACKED;

    struct M_UMBRALES_MEDIDAS /// 5.2.2.10.2 Medidas de Audio
    {
        M_UMBRAL_INTERVALO atenuacion; ///< Atenuacion en funcion de la frecuencia
        M_UMBRAL_INTERVALO ruido; ///< Ruido del canal en reposo
        M_UMBRAL_INTERVALO distorsion; ///< Distorsion
        M_UMBRAL_INTERVALO retardo; ///< Retardo
        M_UMBRAL_INTERVALO pesq; ///< Indice electrico de calidad de voz
        M_UMBRAL_INTERVALO rtemporal; ///< Recorte temporal
        M_UMBRAL_INTERVALO eco; ///< Eco
        M_UMBRAL_INTERVALO diafonia; ///< Diafonia
        M_UMBRAL_INTERVALO ipdvAnalogico; ///< IPDV (Inter Packet Delay Variation) analogico
        M_UMBRAL_INTERVALO ipdvDigital; ///< IPDV (Inter Packet Delay Variation) digital
        M_UMBRAL_INTERVALO paquetes; ///< Paquetes perdidos
    } TIGHLY_PACKED;

    struct MSG_UMBRALES
    {
        uint32 idSesion; ///< Identificador de la sesion del usuario
        M_UMBRALES_MEDIDAS umbrales; ///< Numero de medidas de red a realizar
    } TIGHLY_PACKED;

    struct MSG_EJECUTA_LISTA_MEDIDAS_RED
    {
        uint32 idSesion; ///< Identificador de la sesion del usuario
        uint8 numMedidasRed; ///< Numero de medidas de red a realizar
        Enum<E_TIPO_MEDIDA, uint8> medidas[MAX_MEDIDAS_RED]; ///< [Limitado por el miembro ´numMedidasRed´] ///< Lista de medidas de red a realizar
    } TIGHLY_PACKED;

    struct MSG_MODIFICA_PARAMETROS
    {
        uint32 idSesion; ///< Identificador de la sesion del usuario
        uint8 numParametros; ///< Numero de parámetros a modificar
        uint8 parametros[NMAX_PARAMETRO]; ///< [Limitado por el miembro ´numParametros´] ///< Parámetros de configuración con sus valores para su modificacion
    } TIGHLY_PACKED;

    struct MSG_POSICION_SAM /// Peticion para solicitar al TMCS la activación o desactivacion de un equipo MEDIDAS o PATRÓN
    {
        Enum<E_ON_OFF, uint8> activarSam; ///< Peticion de activacion o desactivacion de la posición o el patrón en modo medición
        IPv4 puesto; ///< Direccion IP del puesto o patrón (4 bytes)
        Enum<E_ABONADO_SAM, uint8> abonado; ///< Tipo de abonado SAM: MEDIDAS o PATRÓN
    } TIGHLY_PACKED;

    struct MSG_RESP_POSICION /// Mensaje del TMCS indicando si el modo de "abonado" se ha activado correctamente o no.
    {
        Enum<E_ESTADO_POSICION, uint8> resultado; ///< Estado de activacion de la posición
        Enum<E_ABONADO_SAM, uint8> abonado; ///< Tipo de abonado SAM: MEDIDAS o PATRÓN
        Enum<E_TIPO_ERROR_SAM, uint8> motivoRechazo; ///< Error por el que la posición no se ha podido activar
    } TIGHLY_PACKED;

    struct MSG_INTERFAZ_MEDICION_SAM /// M_INTERFAZ_MEDICION_SAM.- Peticion para solicitar la activación o desactivacion de un interfaz en modo medición
    {
        Enum<E_ON_OFF, uint8> activarMedicionSam; ///< ACTIVAR_MEDICION_SAM.- Peticion de activacion o desactivacion del interfaz en modo medición
        uint8 slot; ///< INTERFAZ_DESTINO.- Numero de slot del gw donde se encuentra la interfaz a medir (en caso de ISDN o QSIG se suma el número de canal 0, 1 ó 2)
    } TIGHLY_PACKED;

    struct MSG_RESP_MEDICION_SAM /// Respuesta para la activación o desactivacion de un interfaz en modo medición
    {
        Enum<E_ON_OFF, uint8> estado; ///< Estado en que ha quedado el interfaz tras la peticion de activacion o desactivacion del modo medición
        uint8 slot; ///< Numero de slot del gw donde se encuentra la interfaz a medir (en caso de ISDN o QSIG se suma el número de canal 0, 1 ó 2)
    } TIGHLY_PACKED;

    struct MSG_SET_LLAMADA_SAM /// Peticion de establecimiento de la llamada SAM
    {
        Enum<E_ON_OFF, uint8> estado; ///< ON: establecer la llamada, OFF: finalizar la llamada
        IPv4 destino; ///< Posición o interfaz con el que la posición patrón debe establecer la llamada
    } TIGHLY_PACKED;

    struct MSG_RESP_LLAMADA_SAM /// Respuesta para la peticion de establecimiento o finalizacion de la llamada SAM
    {
        Enum<E_ON_OFF, uint8> estado; ///< ON: llamada establecida, OFF: llamada no establecida
        IPv4 destino; ///< Posición o interfaz con el que la posición patrón ha establecido la llamada
    } TIGHLY_PACKED;

    struct MSG_SET_BUCLE_SAM /// Peticion o respuesta de activacion / desactivacion de los bucles de la UGA
    {
        Enum<E_ID_CANAL_BUCLE, uint8> nbucle; ///< Dispositvo de la UGA cuyo bucle se quiere activar
        Enum<E_ON_OFF, uint8> estado; ///< ON: activar el bucle (o bucle activado), OFF: desactivar el bucle (o bucle desactivado)
    } TIGHLY_PACKED;

    struct MSG_PRES_SAM /// Tipo de mensaje de información de estado del servidor SAM (Presencias)
    {
        Enum<E_MODO_SAM, uint8> modoSAM; ///< Estado de las mediciones
    } TIGHLY_PACKED;

    struct MSG_PRES_WATCHDOG_SAM /// Tipo de presencia enviados por el servidor SAM al watchdog
    {
        E_SUBID_MSG subtipo;
        int32 longId;
        int8 proceso[NMAX_NOMBRE_PROCESO];
    } TIGHLY_PACKED;

    struct MSG_TH /// Estructura mensaje general
            /// Cálculo de la longitud del mensaje:
            /// lngDatos = sizeof(CAB_MSG_TH) + ( sizeof(MSG_SUPER) ó sizeof(MSG_CTRL_CONFIG) ó sizeof(ESTRUCTURA_MENSAJE))
    {
        CAB_MSG_TH cab; /// VER PUNTO CABECERA DE TODOS LOS MENSAJES
        union  /// DATOS_MSG_TH --> dat // En esta union está contenido el cuerpo de todos los mensajes
                /// MSG_CONFIG_GLOBAL
                /// Subestructura mensaje de configuración
        {
            MSG_SUPER super; ///< Mensajes presencia ///< [Usado cuando el miembro cab.idMsg es M_PRESENCIA]
            MSG_CONFIG_SESION sesion; ///< Para solicitar ficheros al TMCS ///< [Usado cuando el miembro cab.idMsg es M_INICIO_SESION]
            MSG_CONFIG_INF_FICH_CONFIG infFichConfig; ///< Para mensajes M_INF_FICH_CONFIG ///< [Usado cuando el miembro cab.idMsg es M_INF_FICH_CONFIG]
            MSG_CONFIG_FICH_CONFIG fichConf; ///< Para mensajes M_FICH_CONFIG ///< [Usado cuando el miembro cab.idMsg es M_FICH_CONFIG]
            MSG_ARRANQUE_CONFIG arranque; ///< Para mensajes M_ARRANQUE ///< [Usado cuando el miembro cab.idMsg es M_ARRANQUE]
            MSG_CONFIG_IMPLANTADA configImplantada; ///< Para mensajes M_CONFIG_IMPLANTADA ///< [Usado cuando el miembro cab.idMsg es M_CONFIG_IMPLANTADA]
            MSG_ENVIA_TABLA enviatabla; ///< Mensajes tablas sectorizacion ///< [Usado cuando el miembro cab.idMsg es M_ENVIA_TABLA]
            MSG_DATETIME_SG m_datetime_sg; ///< Mensajes fecha y hora ///< [Usado cuando el miembro cab.idMsg es M_DATETIME_SG]
            MSG_EST_COMU_TLF mEstadisComuTlf; ///< Mensajes estadisticas de comunicaciones telefonía ///< [Usado cuando el miembro cab.idMsg es M_ESTADISTICAS_TF]
            MSG_EST_COMU_RAD mEstadisComuRad; ///< Mensajes estadisticas de comunicaciones radio ///< [Usado cuando el miembro cab.idMsg es M_ESTADISTICAS_RD]
            MSG_SECTORIZA sectoriza; ///< Mensajes sectorización (no en especificaciones 1.9-) ///< [Usado cuando el miembro cab.idMsg es M_SECTORIZA]
            MSG_EST_CONFIG mEstadoConfig; ///< Mensajes de estado de las configuraciones ///< [Usado cuando el miembro cab.idMsg es M_ESTADO_CONFIG]
            MSG_ENLACE_SACTA mEnlaceSacta; ///< Subestructura de mensaje de información de estado del enlace con SACTA ///< [Usado cuando el miembro cab.idMsg es M_ENLACE_SACTA]
            MSG_PRES_SAM mPresenciaSAM; ///< EvtSAM_Presencia Tipo de mensaje de información de estado del servidor SAM a estadísticas (Presencias) ///< [Usado cuando el miembro cab.idMsg es M_PRES_SAM]
            MSG_PRES_WATCHDOG_SAM mPresenciaWatchdogSAM; ///< Presencia del servidor SAM al Watchdog ///< [Usado cuando el miembro cab.idMsg es M_PRES_WATCHDOG_SAM]
            MSG_CONFIG_FICH_CONFIG mAckFichero; ///< Tipo de mensaje de maximo numero de sesiones alcanzado en el servidor SAM ///< [Usado cuando el miembro cab.idMsg es M_ACK_FICHERO]
            IPv4 mDirIP; ///< Tipo de mensaje de maximo numero de sesiones alcanzado en el servidor SAM ///< [Usado cuando el miembro cab.idMsg es M_MAX_SESIONES_SAM]
            uint8 mAvance; ///< Tipo de mensaje de porcentaje de avance en medidas SAM ///< [Usado cuando el miembro cab.idMsg es M_PORCENTAJE_AVANCE_SAM]
            DAT_INICIO_SESION_SAM mInicioSesionSAM; ///< Tipo de mensaje de inicio de sesion en el servidor SAM ///< [Usado cuando el miembro cab.idMsg es M_INICIO_SESION_SAM]
            uint32 mIdSesion; ///< Identificador de sesion ///< [Usado cuando el miembro cab.idMsg es M_FIN_SESION_SAM]
            MSG_BATERIA_POR_ID mListaBaterias; ///< Tipo de mensaje para la consulta de la lista de baterias de pruebas ///< [Usado cuando el miembro cab.idMsg es M_LISTA_BATERIAS_SAM]
            MSG_BATERIA_POR_ID mConsultaBateria; ///< Tipo de mensaje para la consulta de una bateria de pruebas ///< [Usado cuando el miembro cab.idMsg es M_CONSULTA_BATERIA_SAM]
            MSG_ENVIA_BATERIA_VACIA mCreaBateria; ///< Tipo de mensaje para la creacion de una bateria de pruebas ///< [Usado cuando el miembro cab.idMsg es M_CREACION_BATERIA_SAM]
            MSG_POSICION_BATERIA mPosicionBateria; ///< Tipo de mensaje para el alta, la modificacion o el borrado de las pruebas de una posición en una bateria de pruebas ///< [Usado cuando el miembro cab.idMsg es M_POSICION_BATERIA_SAM]
            MSG_INTERFAZ_BATERIA mInterfazBateria; ///< Tipo de mensaje para el alta, la modificacion o el borrado de las pruebas de un interfaz en una bateria de pruebas ///< [Usado cuando el miembro cab.idMsg es M_INTERFAZ_BATERIA_SAM]
            MSG_BATERIA_POR_ID mBorraBateria; ///< Tipo de mensaje para el borrado de una bateria de pruebas ///< [Usado cuando el miembro cab.idMsg es M_BORRADO_BATERIA_SAM]
            MSG_BATERIA_POR_ID mResultadoBateria; ///< Tipo de mensaje para la consulta del resultado de una bateria de pruebas ///< [Usado cuando el miembro cab.idMsg es M_RESULTADO_BATERIA_SAM]
            MSG_BATERIA_POR_ID mEjecutaBateria; ///< Tipo de mensaje para la ejecucion de una bateria de pruebas ///< [Usado cuando el miembro cab.idMsg es M_EJECUTA_BATERIA_SAM]
            MSG_BATERIA_POR_ID mCancelaBateria; ///< Tipo de mensaje para la cancelacion de la ejecucion de una bateria de pruebas ///< [Usado cuando el miembro cab.idMsg es M_CANCELA_BATERIA_SAM]
            MSG_ID_SESION mMedidasRed; ///< Tipo de mensaje para la consulta de las medidas RED ///< [Usado cuando el miembro cab.idMsg es M_LISTA_MEDIDAS_RED]
            MSG_EJECUTA_LISTA_MEDIDAS_RED mEjecutaMedidasRed; ///< Tipo de mensaje para la ejecución de las medidas RED ///< [Usado cuando el miembro cab.idMsg es M_EJECUTA_MEDIDAS_RED]
            MSG_ID_SESION mCancelaMedidasRed; ///< Tipo de mensaje para la ejecución de las medidas RED ///< [Usado cuando el miembro cab.idMsg es M_CANCELA_MEDIDAS_RED]
            MSG_UMBRALES mConsultaUmbrales; ///< Tipo de mensaje para la ejecución de las medidas RED ///< [Usado cuando el miembro cab.idMsg es M_CONSULTA_UMBRALES]
            MSG_UMBRALES mModificaUmbrales; ///< Tipo de mensaje para la ejecución de las medidas RED ///< [Usado cuando el miembro cab.idMsg es M_MODIFICA_UMBRALES]
            MSG_ID_SESION mConsultaParametros; ///< Tipo de mensaje para la consulta de los paramertos de configuración del SAM ///< [Usado cuando el miembro cab.idMsg es M_CONSULTA_PARAMETROS]
            MSG_MODIFICA_PARAMETROS mModificaParametros; ///< Tipo de mensaje para la modificacion de los paramertos de configuración del SAM ///< [Usado cuando el miembro cab.idMsg es M_MODIFICA_PARAMETROS]
            MSG_AUDIO_SAM_POSICION mAudioSAMPosicion; ///< Tipo de mensaje para enviar el resultado de la medición en una posición ///< [Usado cuando el miembro cab.idMsg es M_AUDIO_SAM_POSICION]
            MSG_AUDIO_SAM_INTERFAZ mAudioSAMInterfaz; ///< Tipo de mensaje para enviar el resultado de la medición en un interfaz ///< [Usado cuando el miembro cab.idMsg es M_AUDIO_SAM_INTERFAZ]
            MSG_RED_SAM mRedSAM; ///< Tipo de mensaje para enviar el resultado de las pruebas de red ///< [Usado cuando el miembro cab.idMsg es M_RED_SAM]
            MSG_POSICION_SAM mActivacionPatron; ///< Tipo de mensaje para solicitar que una posición se active o desactive en modo patrón ///< [Usado cuando el miembro cab.idMsg es M_ACTIVACION_PATRON]
            MSG_POSICION_SAM mPosicionMedicion; ///< Tipo de mensaje para que la posición se active o desactive en modo medición ///< [Usado cuando el miembro cab.idMsg es M_POSICION_MEDICION_SAM]
            MSG_RESP_POSICION mRespActivacionPatron; ///< Tipo de mensaje para la respuesta de la peticion de activar o desactivar la posición patrón ///< [Usado cuando el miembro cab.idMsg es M_RESP_ACTIVACION_PATRON]
            MSG_RESP_POSICION mRespPosicionMedicion; ///< Tipo de mensaje para la respuesta a la petición para que la posición se active o desactive en modo medición ///< [Usado cuando el miembro cab.idMsg es M_RESP_POSICION_MEDICION_SAM]
            MSG_INTERFAZ_MEDICION_SAM mInterfazMedicion; ///< Tipo de mensaje para la respuesta a la petición para que la posición se active o desactive en modo medición ///< [Usado cuando el miembro cab.idMsg es M_INTERFAZ_MEDICION_SAM]
            MSG_RESP_MEDICION_SAM mRespInterfazMedicion; ///< Tipo de mensaje para la respuesta a la petición para que un interfaz se active o desactive en modo medición ///< [Usado cuando el miembro cab.idMsg es M_RESP_MEDICION_SAM]
            MSG_SET_LLAMADA_SAM mSetLlamadaSAM; ///< Tipo de mensaje para la activacion de la llamada SAM para poder realizar la medición ///< [Usado cuando el miembro cab.idMsg es M_SET_LLAMADA_SAM]
            MSG_RESP_LLAMADA_SAM mRespSetLlamadaSAM; ///< Tipo de mensaje para la respuesta de la activacion / desactivacion de la llamada SAM ///< [Usado cuando el miembro cab.idMsg es M_RESP_LLAMADA_SAM]
            MSG_SET_BUCLE_SAM mSetBucle; ///< Tipo de mensaje para la peticion de la activacion / desactivacion del bucle de la UGA. ///< [Usado cuando el miembro cab.idMsg es M_SET_BUCLE_SAM]
        } TIGHLY_PACKED;

    } TIGHLY_PACKED;


#ifdef _WIN32
#pragma pack(pop)
#endif

} // namespace SAM_Declaration


#endif //SAM_Declaration_h__

