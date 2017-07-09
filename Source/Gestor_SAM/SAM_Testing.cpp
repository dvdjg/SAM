#include <QFile>
#include <SAM_Log.h>
#include <QTextStream>

#include "SAM_DataBase.h"
#include "SAM_Testing.h"
#include "SAM_MeasuresCount.h"

using namespace SAM;

Testing::Testing()
{
}

bool Testing::test()
{
    bool ret = true;

    //ret = testStateSerializationWrite() && ret;
    //ret = testStateSerializationRead() && ret;

    return ret;
}

bool Testing::testStateSerializationWrite()
{
    ChannelLoopMeasure posChannelLoopMeasure;
    posChannelLoopMeasure.tipoCanalBucle = CANAL_2;
    posChannelLoopMeasure.medidas[ATENUACION].tipoMedida = ATENUACION;
    posChannelLoopMeasure.medidas[ATENUACION].valor = 123;
    posChannelLoopMeasure.medidas[ATENUACION].clasificacion = E_RES_MEDIDA_OPTIMO;
    posChannelLoopMeasure.medidas[PESQ].tipoMedida = PESQ;
    posChannelLoopMeasure.medidas[PESQ].valor = 43;
    posChannelLoopMeasure.medidas[PESQ].clasificacion = E_RES_MEDIDA_MALO;

    DeviceResults positionResults;
    positionResults.fecha = QDateTime::currentDateTime();
    positionResults.clasificacion = E_RESULTADO_BUENO;
    positionResults.medidaBucles[posChannelLoopMeasure.tipoCanalBucle] = posChannelLoopMeasure;

    Position position;
    position.id = 210;
    position.ip = "10.40.64.210";
    position.nombre = "Puesto#210";
    position.bucles[ALTAVOZ_AI].tipoCanalBucle = ALTAVOZ_AI;
    position.bucles[ALTAVOZ_AI].aMedir << DISTORSION << RETARDO_INTERNO << ECO;
    position.medidas << positionResults;

    Link link;
    link.tipoInterfaz = CR_BSS;
    link.canales[CASCOS_1].tipoCanalBucle = CASCOS_1;
    link.canales[CASCOS_1].aMedir << PESQ << RUIDO << ECO;
    link.medidas << positionResults;

    Gateway gateway;
    gateway.id = 250;
    gateway.ip = "10.40.64.250";
    gateway.nombre = "Gateway#250";
    link.id = 1;
    link.slot = 7;
    gateway.interfaces[link.slot] = link;
    link.slot = 13;
    gateway.interfaces[link.slot] = link;

    Gateway gateway2;
    gateway2.id = 260;
    gateway2.ip = "10.40.64.260";
    gateway2.nombre = "Gateway#260";
    link.id = 2;
    link.slot = 4;
    gateway2.interfaces[link.slot] = link;
    link.id = 4;
    link.slot = 6;
    gateway2.interfaces[link.slot] = link;

    Battery battery;
    battery.idBateria = 201;
    battery.nombre = "Batería nº1";
    battery.creacion = QDateTime::currentDateTime();
    battery.modificacion = battery.creacion;
    battery.modificacion = QDateTime(); // Un valor nulo
    battery.idConfig = 105;
    battery.subIdConfig = 106;
    battery.idCentro = 107;
    //battery.config["HW"] = "Archivo de configuración Hardware";
    battery.posiciones[position.ip] = position;
    battery.gateways[gateway.ip] = gateway;
    battery.gateways[gateway2.ip] = gateway2;

    /*
     *  5.2.2.10.2.1 Atenuación en función de la Frecuencia
     *
     *  El S-SAM, bajo petición, debe medir la atenuación en función de
     *  la frecuencia para comunicaciones telefónicas.
     *  El S-SAM, bajo petición, debe medir la atenuación en función de
     *  la frecuencia para comunicaciones radio.
     */
    MeasureDefinition defATENUACION;
    defATENUACION.tipoMedida = ATENUACION;
    defATENUACION.tipo = TIPO_AUDIO;
    defATENUACION.nombre = "Atenuación en función de la frecuencia";

    /*
     *  5.2.2.10.2.2 Ruido
     *  El S-SAM, bajo petición, debe medir el ruido del canal en reposo.
     *  El servidor SAM debe definir la medida de ruido con los siguientes campos:
     *    - IDmedida: automático
     *    - Tipo=AUDIO.
     *    - NombreMedida: "Ruido"
     *    - ValorOptimo=MINIMO.
     *    - rMin= sin definir.
     *    - rMax= -60
     *    - unidad="dBm"
     */
    MeasureDefinition defRUIDO;
    defRUIDO.tipoMedida = RUIDO;
    defRUIDO.tipo = TIPO_AUDIO;
    defRUIDO.nombre = "Ruido";
    defRUIDO.valorOptimo = MINIMO;
    defRUIDO.rMax = -60;
    defRUIDO.unidad = "dBm";

    /*
     *  5.2.2.10.2.3 Distorsión
     *  El S-SAM, bajo petición, debe medir la distorsión total.
     *  El servidor SAM debe definir la medida de distorsión con los siguientes campos:
     *    - IDmedida: automático
     *    - Tipo=AUDIO.
     *    - NombreMedida: "Distorsión"
     *    - ValorOptimo=MINIMO.
     *    - rMin= sin definir.
     *    - rMax= sin definir
     *    - unidad="%"
     */
    MeasureDefinition defDISTORSION;
    defDISTORSION.tipoMedida = DISTORSION;
    defDISTORSION.tipo = TIPO_AUDIO;
    defDISTORSION.nombre = "Distorsión";
    defDISTORSION.valorOptimo = MINIMO;
    defDISTORSION.unidad = "%";

    /*
     *  5.2.2.10.2.4 Retardo
     *  El S-SAM, bajo petición, debe medir el retardo para comunicaciones internas
     * (entre posiciones).
     *  El servidor SAM debe definir la medida del retardo para
     *  comunicaciones internas con los siguientes campos:
     *    - IDmedida: automático
     *    - Tipo=AUDIO.
     *    - NombreMedida: "Retardo Interno (entre posiciones)"
     *    - ValorOptimo=MINIMO.
     *    - rMin: sin definir.
     *    - rMax= 145
     *    - unidad="ms"
     */
    MeasureDefinition defRETARDO_INTERNO;
    defRETARDO_INTERNO.tipoMedida = RETARDO_INTERNO;
    defRETARDO_INTERNO.tipo = TIPO_AUDIO;
    defRETARDO_INTERNO.nombre = "Retardo Interno (entre posiciones)";
    defRETARDO_INTERNO.valorOptimo = MINIMO;
    defRETARDO_INTERNO.rMax = 145;
    defRETARDO_INTERNO.unidad = "ms";

    /*
     *  5.2.2.10.2.4 Retardo
     *  El S-SAM, bajo petición, debe medir el retardo para comunicaciones externas
     *  (entre una posición y una interfaz del gateway).
     *  El servidor SAM debe definir la medida del retardo para comunicaciones
     *  externas (entre una posición y un interfaz de un gateway) con los siguientes campos:
     *    - IDmedida: automático
     *    - Tipo=AUDIO.
     *    - NombreMedida: "Retardo Externo (posición-interfaz)"
     *    - ValorOptimo=MINIMO.
     *    - rMin: sin definir.
     *    - rMax= 45
     *    - unidad="ms"
     */
    MeasureDefinition defRETARDO_EXTERNO;
    defRETARDO_EXTERNO.tipoMedida = RETARDO_EXTERNO;
    defRETARDO_EXTERNO.tipo = TIPO_AUDIO;
    defRETARDO_EXTERNO.nombre = "Retardo Externo (posición-interfaz)";
    defRETARDO_EXTERNO.valorOptimo = MINIMO;
    defRETARDO_EXTERNO.rMax = 45;
    defRETARDO_EXTERNO.unidad = "ms";

    /*
     *  5.2.2.10.2.5 Índice eléctrico de calidad de voz (PESQ LQO)
     *  El S-SAM, bajo petición, debe medir el índice acústico de claridad de voz,
     *  medido conforme a TOSQA2001 (Telecommunication Objective Speech Quality Assesment)
     *  sobre una comunicación G.711 extremo a extremo.
     *  El servidor SAM debe definir la medida del PESQ LQO con los siguientes campos:
     *       - IDmedida: automático
     *       - Tipo=AUDIO.
     *       - NombreMedida: "PESQ LQO"
     *       - ValorOptimo=    MAXIMO.
     *       - rMin= 4
     *       - rMax: sin definir
     *       - unidad="N/A"
     */
    MeasureDefinition defPESQ;
    defPESQ.tipoMedida = DISTORSION;
    defPESQ.tipo = TIPO_AUDIO;
    defPESQ.nombre = "PESQ LQO";
    defPESQ.valorOptimo = MAXIMO;
    defPESQ.rMin = 4;
    defPESQ.unidad = "N/A";

    /*
     *  5.2.2.10.2.6 Recorte temporal
     *  El S-SAM, bajo petición, debe medir el tamaño de los recortes de segmentos de voz.
     *  El servidor SAM debe definir la medida del tamaño de los recortes con los siguientes campos:
     *      - IDmedida: automático
     *      - Tipo=AUDIO.
     *      - NombreMedida: "Tamaño Recortes de segmentos de voz"
     *      - ValorOptimo=    MINIMO.
     *      - rMin= sin definir
     *      - rMax= 64
     *      - unidad="ms"
     */
    MeasureDefinition defTAM_RECORTES;
    defTAM_RECORTES.tipoMedida = TAM_RECORTES;
    defTAM_RECORTES.tipo = TIPO_AUDIO;
    defTAM_RECORTES.nombre = "Tamaño Recortes de segmentos de voz";
    defTAM_RECORTES.valorOptimo = MINIMO;
    defTAM_RECORTES.rMax = 64;
    defTAM_RECORTES.unidad = "ms";

    /*
     *  5.2.2.10.2.6 Recorte temporal
     *  El S-SAM, bajo petición, debe medir la cantidad (en %) de recortes de segmentos de voz.
     *  El servidor SAM debe definir la medida de la cantidad de recortes de segmentos de voz con los siguientes campos:
     *      - IDmedida: automático
     *      - Tipo=AUDIO.
     *      - NombreMedida: "Cantidad de Recortes"
     *      - ValorOptimo=    MINIMO.
     *      - rMin= sin definir
     *      - rMax= 0,2
     *      - unidad="%"
     */
    MeasureDefinition defNUM_RECORTES;
    defNUM_RECORTES.tipoMedida = NUM_RECORTES;
    defNUM_RECORTES.tipo = TIPO_AUDIO;
    defNUM_RECORTES.nombre = "Cantidad de Recortes";
    defNUM_RECORTES.valorOptimo = MINIMO;
    defNUM_RECORTES.rMax = 0.2f;
    defNUM_RECORTES.unidad = "ms";

    /*
     *  5.2.2.10.2.7 Eco
     *  El S-SAM, bajo petición, debe medir el eco de la señal de audio conforme establece ED-136, Sección 6.3.2.
     */
    MeasureDefinition defECO;
    defECO.tipoMedida = ECO;
    defECO.tipo = TIPO_AUDIO;
    defECO.nombre = "Eco";

    /*
     *  5.2.2.10.2.8 Diafonía
     *  El S-SAM, bajo petición, debe medir la diafonía cuando se inyecta un tono
     *  de 1020 Hz a 0 dBm en cualquier otro circuito de voz.
     *  El servidor SAM debe definir la medida de la diafonía con los siguientes campos:
     *      - IDmedida: automático
     *      - Tipo=AUDIO.
     *      - NombreMedida: "Diafonía"
     *      - ValorOptimo=    MINIMO.
     *      - rMin= sin definir
     *      - rMax= -60
     *      - unidad="dBm"
     */
    MeasureDefinition defDIAFONIA;
    defDIAFONIA.tipoMedida = DIAFONIA;
    defDIAFONIA.tipo = TIPO_AUDIO;
    defDIAFONIA.nombre = "Diafonía";
    defDIAFONIA.valorOptimo = MINIMO;
    defDIAFONIA.rMax = -60;
    defDIAFONIA.unidad = "dBm";

    /*
     *  5.2.3.2.2 Medidas de Red
     *  5.2.3.2.2.1 IPDV Digital de tramas RTP
     *  El S-SAM, bajo petición, debe medir la IPDV (IP Packet Delay Variation) en la transmisión de audio.
     *  El servidor SAM debe definir la medida de IPDV digital con los siguientes campos:
     *      - IDmedida: automático
     *      - Tipo=AUDIO.
     *      - NombreMedida: "IPDV digital"
     *      - ValorOptimo=MINIMO.
     *      - rMin= sin definir.
     *      - rMax= 5
     *      - unidad="ms"
     */
    MeasureDefinition defIPDV_DIGITAL;
    defIPDV_DIGITAL.tipoMedida = IPDV_DIGITAL;
    defIPDV_DIGITAL.tipo = TIPO_AUDIO;
    defIPDV_DIGITAL.nombre = "IPDV Digital";
    defIPDV_DIGITAL.valorOptimo = MINIMO;
    defIPDV_DIGITAL.rMax = 5;
    defIPDV_DIGITAL.unidad = "ms";

    MeasureDefinition defIPDV_ANALOGICO;
    defIPDV_ANALOGICO.tipoMedida = IPDV_ANALOGICO;
    defIPDV_ANALOGICO.tipo = TIPO_AUDIO;
    defIPDV_ANALOGICO.nombre = "IPDV Analógico";
    defIPDV_ANALOGICO.valorOptimo = MINIMO;
    defIPDV_ANALOGICO.rMax = 5;
    defIPDV_ANALOGICO.unidad = "ms";

    /*
     *  5.2.3.2.2.2 Pérdida de Paquetes
     *  El S-SAM, bajo petición, debe medir la pérdida de paquetes en la LAN.
     *  El servidor SAM debe definir la medida de paquetes perdidos con los siguientes campos:
     *      - IDmedida: automático
     *      - Tipo=AUDIO.
     *      - NombreMedida: "Paquetes perdidos"
     *      - ValorOptimo=MINIMO.
     *      - rMin= sin definir.
     *      - rMax= 0,05
     *      - unidad="%"
     */
    MeasureDefinition defPAQUETES_PERDIDOS;
    defPAQUETES_PERDIDOS.tipoMedida = PAQUETES_PERDIDOS;
    defPAQUETES_PERDIDOS.tipo = TIPO_AUDIO;
    defPAQUETES_PERDIDOS.nombre = "Paquetes perdidos";
    defPAQUETES_PERDIDOS.valorOptimo = MINIMO;
    defPAQUETES_PERDIDOS.rMax = 0.05f;
    defPAQUETES_PERDIDOS.unidad = "%";

    UserSession session;
    session.idSesion = 202;
    session.rol = "Pruebas";
    session.creacion = QDateTime::currentDateTime();
    session.modificacion = QDateTime::currentDateTime();
    session.parametros << 110 << 111 << 112 << 113;
    session.definicionMedidas[ATENUACION]       = defATENUACION;
    session.definicionMedidas[RUIDO]            = defRUIDO;
    session.definicionMedidas[DISTORSION]       = defDISTORSION;
    session.definicionMedidas[RETARDO_INTERNO]  = defRETARDO_INTERNO;
    session.definicionMedidas[RETARDO_EXTERNO]  = defRETARDO_EXTERNO;
    session.definicionMedidas[PESQ]             = defPESQ;
    session.definicionMedidas[TAM_RECORTES]     = defTAM_RECORTES;
    session.definicionMedidas[NUM_RECORTES]     = defNUM_RECORTES;
    session.definicionMedidas[ECO]              = defECO;
    session.definicionMedidas[DIAFONIA]         = defDIAFONIA;
    session.definicionMedidas[IPDV_ANALOGICO]   = defIPDV_ANALOGICO;
    session.definicionMedidas[IPDV_DIGITAL]     = defIPDV_DIGITAL;
    session.definicionMedidas[PAQUETES_PERDIDOS]= defPAQUETES_PERDIDOS;
    session.baterias[battery.idBateria] = battery;

    SAM_DataBase samDB;
    samDB.estado.idSesion = session.idSesion;
    samDB.estado.idBateria = battery.idBateria;
    samDB.estado.ipPatron = "10.40.64.103";
    samDB.estado.ipEquipo = "10.40.64.104";
    samDB.estado.tipoCanalBucle = CASCOS_1;
    samDB.estado.tipoMedida = RUIDO;
    samDB.sesionesUsuario[session.idSesion] = session;

    int nStored = samDB.storeSessions("storage");
    LOG_INFO(STATE) << "Almacenado estado" << nStored;

    MeasuresCount measuresCount;
    samDB.countBatteryMeasureDevices(&measuresCount);

    LOG_INFO(STATE) << "Cuenta de dispositivos: "
                       "Posiciones =" << measuresCount.nPositions
                    << ", Bucles =" << measuresCount.nLoops
                    << ", Gateways =" << measuresCount.nGateways
                    << ", Interfaces =" << measuresCount.nLinks
                    << ", Canales =" << measuresCount.nChannels
                    << ", Total Medidas =" << measuresCount.nMeasures;

    return true;
}

bool Testing::testStateSerializationRead()
{
    SAM_DataBase samDB;

    samDB.loadSessions("storage");
    samDB.storeSessions("storageTemp");

    /*
    QString jsonString;
    JVariant jsonVariant;

    QFile file("Estado.json");
    if (file.open(QFile::ReadOnly))
    {
        QTextStream stream(&file);

        jsonString = stream.readAll();

        jsonVariant.parse(jsonString);

        state.fromVariantMap(jsonVariant);

        //jsonVariant = state.toVariant();
        jsonString = state.toJson();

        QFile file("PruebasEstadoOut.json");
        if (file.open(QFile::WriteOnly | QFile::Truncate))
        {
            QTextStream stream(&file);

            stream << jsonString;
        }

    }
    */

    return false;
}

