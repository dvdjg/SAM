/** @file Funciones de conversión de datos entre objetos C++ y objetos JSON.
 *
 */
#include <QFile>
#include <QDir>
#include <QRegExp>
#include <limits>

#include "SAM_Log.h"
#include "SAM_Helpers.h"
#include "SAM_JVariant.h"
#include "SAM_MeasuresCount.h"
#include "SAM_DataBase.h"

namespace SAM
{
    bool isNumber(float number)
    {
        return number >= -std::numeric_limits<float>::max()
            && number <=  std::numeric_limits<float>::max();
    }

    template<typename T>
    QString toString(const T & t)
    {
        return QLocale::system().toString(t);
    }

    template<typename T>
    QString toString(const EnumHolder<T> & t)
    {
        return t.toString();
    }

    template<>
    QString toString(const QString & t)
    {
        return t;
    }

    template<typename T>
    QVariant toVariant(const T & t)
    {
        QVariant ret = t;
        return ret;
    }

    template<typename Key, typename T>
    QVariant toVariant(const QMap<Key, T> & map)
    {
        TRACE(STATE);

        QVariantMap ret;
        QMapIterator<Key, T> it(map);

        while(it.hasNext())
        {
             it.next();
             QString key(toString(it.key()));
             ret[key] = toVariant(it.value());
        }

        return ret;
    }

    template<typename T>
    QVariant toVariant(const QList<T> & lst)
    {
        TRACE(STATE);

        QVariantList ret;
        QListIterator<T> it(lst);

        while(it.hasNext()) {
             ret.append(toVariant(it.next()));
        }

        return ret;
    }

    template<>
    QVariant toVariant(const QVariantMap & map)
    {
        return map;
    }

    template<>
    QVariant toVariant(const QVariantList & lst)
    {
        return lst;
    }

    template<typename T>
    void fromVariant(QList<T> & dst, const QVariant & var)
    {
        TRACE(STATE);

        QVariantList lstVar = var.toList();
        dst.clear();

        QListIterator<QVariant> it(lstVar);
        while(it.hasNext()) {
            T val(JVariant(it.next()));
            dst.append(val);
        }
    }

    template<typename Key, typename T>
    void fromVariant(QMap<Key, T> & dst, const QVariant & var)
    {
        TRACE(STATE);

        QVariantMap mapVar = var.toMap();

        QMapIterator<QString, QVariant> it(mapVar);
        dst.clear();
        while(it.hasNext())
        {
             it.next();
             Key key(JVariant(it.key()));
             T val(JVariant(it.value()));

             dst[key] = val;
        }
    }

    uint SAM_DataBase::s_idSesion = 1;

    E_RES_MEDIDA classifyMeasure(float valor,
                                 E_VALOR_OPTIMO valorOptimo,
                                 float rMin,
                                 float rMax)
    {
        TRACE(STATE);

        E_RES_MEDIDA resMedida = E_RES_MEDIDA_NO_RES;

        /**
         *  5.2.4.3.1.1 Evaluación del resultado de la medida
         *
         *  Para una medida con un umbral definido, el servidor SAM debe evaluar
         *  si el resultado obtenido es malo, bueno u óptimo.
         *  Si para una medida, numUmbrales=1 y ValorOptimo=MINIMO, el servidor SAM
         *  debe evaluar el resultado obtenido como:
         *        - OPTIMO: si valor < rMax
         *        - MALO: si valor >= rMax
         *
         *  Si para una medida, numUmbrales=1 y ValorOptimo=MAXIMO,
         *  el servidor SAM debe evaluar el resultado obtenido como:
         *        - OPTIMO: si valor > rMin
         *        - MALO: si valor <= rMin
         *
         *  Para una medida con dos umbrales definidos, el servidor SAM debe evaluar
         *  si el resultado obtenido es malo, bueno u óptimo.
         *  Si para una medida, numUmbrales=2 y ValorOptimo=MINIMO, el servidor SAM
         *  debe evaluar el resultado obtenido como:
         *        - OPTIMO: si valor < rMin
         *        - BUENO: si rMin <= valor < rMax
         *        - MALO: si valor >= rMax
         *
         *  Si para una medida, numUmbrales=2 y ValorOptimo=MAXIMO, el servidor SAM
         *  debe evaluar el resultado obtenido como:
         *        - OPTIMO: si valor > rMax
         *        - BUENO: si rMin < valor <= rMax
         *        - MALO: si valor <= rMin
         */

        /**
         *  5.2.2.2 Informe de Pruebas de Audio
         *
         *  - OPTIMO: resultado mejor que el umbral (si hay 1 umbral) y mejor que el umbral óptimo (si hay 2 umbrales).
         *  - BUENO: resultado entre ambos umbrales (si hay 2 umbrales).
         *  - MALO: resultado peor que el umbral (si hay 1 umbral) y peor que ambos umbrales (si hay 2 umbrales).
         *  - NO_RES: medida sin umbrales.
         */
        if(isNumber(rMin) && isNumber(rMax)) // numUmbrales=2
        {
            if(valorOptimo == MINIMO) {
                resMedida = (valor < rMin) ? E_RES_MEDIDA_OPTIMO :
                           (valor >= rMax) ? E_RES_MEDIDA_MALO :
                                             E_RES_MEDIDA_BUENO;
            } else if(valorOptimo == MAXIMO) {
                resMedida = (valor > rMax) ? E_RES_MEDIDA_OPTIMO :
                           (valor <= rMin) ? E_RES_MEDIDA_MALO :
                                             E_RES_MEDIDA_BUENO;
            } else {
                LOG_WARN_LN(STATE) << "Valor óptimo no permitido:" << valorOptimo;
            }
        }
        else
        {
            if(isNumber(rMax))
            {
                if(valorOptimo == MINIMO) { // numUmbrales=1
                    resMedida = (valor < rMax) ? E_RES_MEDIDA_OPTIMO : E_RES_MEDIDA_MALO;
                } else {
                    LOG_WARN_LN(STATE) << "Combinación diferente a rMax/MINIMO no permitida.";
                }
            }
            else if(isNumber(rMin))
            {
                if(valorOptimo == MAXIMO) { // numUmbrales=1
                    resMedida = (valor > rMin) ? E_RES_MEDIDA_OPTIMO : E_RES_MEDIDA_MALO;
                } else {
                    LOG_WARN_LN(STATE) << "Combinación diferente a rMin/MAXIMO no permitida.";
                }
            }
        }

        return resMedida;
    }

    // Los valores nulos de las cifras se van a codificar como Quiet NaN.

    MeasureDefinition::MeasureDefinition() :
        rMin(std::numeric_limits<float>::quiet_NaN()),
        rMax(std::numeric_limits<float>::quiet_NaN())
    {
    }

    MeasureDefinition::MeasureDefinition(const QVariantMap & in) :
        rMin(std::numeric_limits<float>::quiet_NaN()),
        rMax(std::numeric_limits<float>::quiet_NaN())
    { fromVariantMap(in); }

    MeasureDefinition::MeasureDefinition(const QVariant & in) :
        rMin(std::numeric_limits<float>::quiet_NaN()),
        rMax(std::numeric_limits<float>::quiet_NaN())
    { fromVariantMap(in.toMap()); }

    QVariantMap MeasureDefinition::toVariant() const
    {
        TRACE(STATE);

        QVariantMap map;

        map["tipoMedida"] = tipoMedida;
        map["tipo"] = tipo;
        map["nombre"] = nombre;
        map["rMin"] = (double) rMin;
        map["rMax"] = (double) rMax;
        map["unidad"] = unidad;

        return map;
    }

    void MeasureDefinition::fromVariantMap(const QVariantMap & in)
    {
        TRACE(STATE);

        JVariant map(in);

        tipoMedida = map["tipoMedida"];
        tipo = map["tipo"];
        nombre = map["nombre"].toString();
        rMin = map["rMin"];
        rMax = map["rMax"];
        unidad = map["unidad"].toString();
    }

    QVariantMap MeasuresState::toVariant() const
    {
        TRACE(STATE);

        QVariantMap map;

        map["idSesion"] = idSesion;
        map["idBateria"] = idBateria;
        map["ipPatron"] = ipPatron;
        map["ipEquipo"] = ipEquipo;
        map["tipoCanalBucle"] = tipoCanalBucle;
        map["tipoMedida"] = tipoMedida;

        return map;
    }

    void MeasuresState::fromVariantMap(const QVariantMap & in)
    {
        TRACE(STATE);

        JVariant map(in);

        idSesion = map["idSesion"];
        idBateria = map["idBateria"];
        ipPatron = map["ipPatron"].toString();
        ipEquipo = map["ipEquipo"].toString();
        tipoCanalBucle = map["tipoCanalBucle"];
        tipoMedida = map["tipoMedida"];
    }

    void MeasuresState::clear()
    {
        idSesion = 0;
        idBateria = 0;
        ipPatron.clear();
        ipEquipo.clear();
        slot = 0;
        tipoInterfaz.clear();
        tipoCanalBucle.clear();
        tipoMedida.clear();
    }

    QVariantMap Equipment::toVariant() const
    {
        TRACE(STATE);

        QVariantMap map;

        map["id"] = id;
        map["ip"] = ip;
        map["nombre"] = nombre;

        return map;
    }

    void Equipment::fromVariantMap(const QVariantMap & in)
    {
        TRACE(STATE);

        JVariant map(in);

        id = map["id"];
        ip = map["ip"].toString();
        nombre = map["nombre"].toString();
    }

    QVariantMap ChannelLoop::toVariant() const
    {
        TRACE(STATE);

        QVariantMap map;

        map["tipoCanalBucle"] = tipoCanalBucle;
        map["aMedir"] = SAM::toVariant(aMedir);

        return map;
    }

    void ChannelLoop::fromVariantMap(const QVariantMap & in)
    {
        TRACE(STATE);

        JVariant map(in);

        tipoCanalBucle = map["tipoCanalBucle"];
        fromVariant(aMedir, map["aMedir"]);
    }

    QVariantMap MeasureResult::toVariant() const
    {
        TRACE(STATE);

        QVariantMap map;

        map["tipoMedida"] = tipoMedida;
        map["valor"] = (double) valor;
        map["clasificacion"] = clasificacion;

        return map;
    }

    void MeasureResult::fromVariantMap(const QVariantMap & in)
    {
        TRACE(STATE);

        JVariant map(in);

        tipoMedida = map["tipoMedida"];
        valor = map["valor"];
        clasificacion = map["clasificacion"];
    }

    QVariantMap ChannelLoopMeasure::toVariant() const
    {
        TRACE(STATE);

        QVariantMap map;

        map["tipoCanalBucle"] = tipoCanalBucle;
        map["medidas"] = SAM::toVariant(medidas);

        return map;
    }

    void ChannelLoopMeasure::fromVariantMap(const QVariantMap & in)
    {
        TRACE(STATE);

        JVariant map(in);

        tipoCanalBucle = map["tipoCanalBucle"];
        fromVariant(medidas, map["medidas"]);
    }

    QVariantMap DeviceResults::toVariant() const
    {
        TRACE(STATE);

        QVariantMap map;

        map["fecha"] = fecha;
        map["clasificacion"] = clasificacion;
        map["medidaBucles"] = SAM::toVariant(medidaBucles);

        return map;
    }

    void DeviceResults::fromVariantMap(const QVariantMap & in)
    {
        TRACE(STATE);

        JVariant map(in);

        fecha = map["fecha"];
        clasificacion = map["clasificacion"];
        fromVariant(medidaBucles, map["medidaBucles"]);
    }

    void DeviceResults::classify()
    {
        /**
         *  5.2.2.2 Informe de Pruebas de Audio
         *
         *  Clasificación del resultado general:
         *   - OPTIMO: si todas las medidas realizadas sobre la posición han tenido
         *      un resultado OPTIMO. (NOTA: O al menos un OPTIMO y el resto NO_RES)
         *   - BUENO: si al menos un resultado de las medidas es BUENO, y ninguno MALO.
         *   - MALO: si hay al menos un resultado de las medidas es MALO.
         *   - NO_RES: ninguna de las medidas tiene umbral definido.
         */

        // Recorre todas las medidas de todos los bucles y los clasifica.
        clasificacion = E_RESULTADO_NO_RES;
        foreach(const ChannelLoopMeasure & canalBucle, medidaBucles)
        {
            foreach(const MeasureResult & medida, canalBucle.medidas)
            {
                if(medida.clasificacion == E_RES_MEDIDA_MALO) {
                    clasificacion = E_RESULTADO_MALO;
                    return;
                } else if(medida.clasificacion == E_RES_MEDIDA_BUENO) {
                    clasificacion = E_RESULTADO_BUENO;
                } else if(medida.clasificacion == E_RES_MEDIDA_OPTIMO
                              && clasificacion == E_RESULTADO_NO_RES) {
                    clasificacion = E_RESULTADO_OPTIMO;
                }
            }
        }
    }

    QVariantMap Position::toVariant() const
    {
        TRACE(STATE);

        QVariantMap map = Equipment::toVariant();

        map["bucles"] = SAM::toVariant(bucles);
        map["medidas"] = SAM::toVariant(medidas);

        return map;
    }

    void Position::fromVariantMap(const QVariantMap & in)
    {
        TRACE(STATE);

        Equipment::fromVariantMap(in);

        JVariant map(in);

        fromVariant(bucles, map["bucles"]);
        fromVariant(medidas, map["medidas"]);
    }

    QVariantMap Link::toVariant() const
    {
        TRACE(STATE);

        QVariantMap map;

        map["id"] = id;
        map["slot"] = slot;
        map["tipoInterfaz"] = tipoInterfaz;
        map["canales"] = SAM::toVariant(canales);
        map["medidas"] = SAM::toVariant(medidas);

        return map;
    }

    void Link::fromVariantMap(const QVariantMap & in)
    {
        TRACE(STATE);

        JVariant map(in);

        id = map["id"];
        slot = map["slot"];
        tipoInterfaz = map["tipoInterfaz"];
        fromVariant(canales, map["canales"]);
        fromVariant(medidas, map["medidas"]);
    }

    QVariantMap Gateway::toVariant() const
    {
        TRACE(STATE);

        QVariantMap map = Equipment::toVariant();

        map["interfaces"] = SAM::toVariant(interfaces);

        return map;
    }

    void Gateway::fromVariantMap(const QVariantMap & in)
    {
        TRACE(STATE);

        Equipment::fromVariantMap(in);

        JVariant map(in);

        fromVariant(interfaces, map["interfaces"]);
    }

    QVariantMap Battery::toVariant() const
    {
        TRACE(STATE);

        QVariantMap map;

        map["idBateria"] = idBateria;
        map["nombre"] = nombre;
        map["creacion"] = creacion;
        map["modificacion"] = modificacion;
        map["ultimaEjecucion"] = ultimaEjecucion;
        map["idConfig"] = idConfig;
        map["subIdConfig"] = subIdConfig;
        map["idCentro"] = idCentro;
        //map["config"] = config;
        map["posiciones"] = SAM::toVariant(posiciones);
        map["gateways"] = SAM::toVariant(gateways);

        return map;
    }

    QString Battery::toJson() const
    {
        TRACE(STATE);

        JVariant jVariant(toVariant());

        return jVariant.toJson();
    }

    void Battery::fromVariantMap(const QVariantMap & in)
    {
        TRACE(STATE);

        JVariant map(in);

        idBateria = map["idBateria"];
        nombre = map["nombre"].toString();
        creacion = map["creacion"];
        modificacion = map["modificacion"];
        ultimaEjecucion = map["ultimaEjecucion"];
        idConfig = map["idConfig"];
        subIdConfig = map["subIdConfig"];
        idCentro = map["idCentro"];
        //config = map["config"];
        fromVariant(posiciones, map["posiciones"]);
        fromVariant(gateways, map["gateways"]);
    }

    QVariantMap UserSession::toVariant() const
    {
        TRACE(STATE);

        QVariantMap map;

        map["idSesion"] = idSesion;
        map["rol"] = rol;
        map["creacion"] = creacion;
        map["modificacion"] = modificacion;
        map["parametros"] = SAM::toVariant(parametros);
        map["definicionMedidas"] = SAM::toVariant(definicionMedidas);
        map["baterias"] = SAM::toVariant(baterias);

        return map;
    }

    void UserSession::fromVariantMap(const QVariantMap & in)
    {
        TRACE(STATE);

        JVariant map(in);

        idSesion = map["idSesion"];
        rol = map["rol"].toString();
        creacion = map["creacion"];
        modificacion = map["modificacion"];
        fromVariant(parametros, map["parametros"]);
        fromVariant(definicionMedidas, map["definicionMedidas"]);
        fromVariant(baterias, map["baterias"]);
    }

    void UserSession::setDefaultMeasureDefinitions()
    {
        TRACE(STATE);

        /**
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

        /**
         *  5.2.2.10.2.2 Ruido
         *
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

        /**
         *  5.2.2.10.2.3 Distorsión
         *
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

        /**
         *  5.2.2.10.2.4 Retardo
         *
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

        /**
         *  5.2.2.10.2.4 Retardo
         *
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

        /**
         *  5.2.2.10.2.5 Índice eléctrico de calidad de voz (PESQ LQO)
         *
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

        /**
         *  5.2.2.10.2.6 Recorte temporal
         *
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

        /**
         *  5.2.2.10.2.6 Recorte temporal
         *
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

        /**
         *  5.2.2.10.2.7 Eco
         *
         *  El S-SAM, bajo petición, debe medir el eco de la señal de audio conforme establece ED-136, Sección 6.3.2.
         */
        MeasureDefinition defECO;
        defECO.tipoMedida = ECO;
        defECO.tipo = TIPO_AUDIO;
        defECO.nombre = "Eco";

        /**
         *  5.2.2.10.2.8 Diafonía
         *
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

        /**
         *  5.2.3.2.2 Medidas de Red
         *
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

        /**
         *  5.2.3.2.2.2 Pérdida de Paquetes
         *
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

        definicionMedidas[ATENUACION]       = defATENUACION;
        definicionMedidas[RUIDO]            = defRUIDO;
        definicionMedidas[DISTORSION]       = defDISTORSION;
        definicionMedidas[RETARDO_INTERNO]  = defRETARDO_INTERNO;
        definicionMedidas[RETARDO_EXTERNO]  = defRETARDO_EXTERNO;
        definicionMedidas[PESQ]             = defPESQ;
        definicionMedidas[TAM_RECORTES]     = defTAM_RECORTES;
        definicionMedidas[NUM_RECORTES]     = defNUM_RECORTES;
        definicionMedidas[ECO]              = defECO;
        definicionMedidas[DIAFONIA]         = defDIAFONIA;
        definicionMedidas[IPDV_ANALOGICO]   = defIPDV_ANALOGICO;
        definicionMedidas[IPDV_DIGITAL]     = defIPDV_DIGITAL;
        definicionMedidas[PAQUETES_PERDIDOS]= defPAQUETES_PERDIDOS;
    }

    QVariantMap SAM_DataBase::toVariant() const
    {
        TRACE(STATE);

        QVariantMap map;

        map["estado"] = estado;
        map["mensajes"] = mensajes; // SAM::toVariant(mensajes);
        map["sesionesUsuario"] = SAM::toVariant(sesionesUsuario);

        return map;
    }

    QString SAM_DataBase::toJson() const
    {
        TRACE(STATE);

        JVariant jVariant(toVariant());

        return jVariant.toJson();
    }

    void SAM_DataBase::fromVariantMap(const QVariantMap & in)
    {
        TRACE(STATE);

        JVariant map(in);

        estado = map["estado"];
        mensajes = map["mensajes"];
        fromVariant(sesionesUsuario, map["sesionesUsuario"]);
    }

    Battery * SAM_DataBase::getBattery()
    {
        return getBattery(estado.idSesion, estado.idBateria);
    }

    const Battery * SAM_DataBase::getBattery() const
    {
        return getBattery(estado.idSesion, estado.idBateria);
    }

    Battery * SAM_DataBase::getBattery(uint idSesion, uint idBateria)
    {
        TRACE(STATE);

        Battery *ret = 0;

        if(sesionesUsuario.contains(idSesion)
           && sesionesUsuario[idSesion].baterias.contains(idBateria)) {
            ret = &sesionesUsuario[idSesion].baterias[idBateria];
        }

        return ret;
    }

    const Battery * SAM_DataBase::getBattery(uint idSesion, uint idBateria) const
    {
        TRACE(STATE);

        const Battery *ret = 0;

        QMap<uint, UserSession>::const_iterator itSesion = sesionesUsuario.find(idSesion);
        if(itSesion != sesionesUsuario.end()) {
            QMap<uint, Battery>::const_iterator itBateria = itSesion->baterias.find(idBateria);
            if(itBateria != itSesion->baterias.end()) {
                ret = &itBateria.value();
            }
        }
//        if(sesionesUsuario.contains(idSesion)
//           && sesionesUsuario[idSesion].baterias.contains(idBateria)) {
//            ret = &sesionesUsuario.find(idSesion).baterias[idBateria];
//            //const Battery & bat = sesionesUsuario[idSesion].baterias[idBateria];
//            //ret = &bat;
//        }

        return ret;
    }

    bool SAM_DataBase::currentBatteryMeasureDevice(MeasureDevice * pMeasureDevice) const
    {
        MeasureDevice measureDevice;
        TRACE(STATE);

        bool bRet = false;
        const Battery * pBattery = getBattery();

        if(!pBattery)
        {
            LOG_WARN(STATE) << "No se puede obtener el dispositivo de medición "
                               "porque no hay seleccionada una batería: idSesion ="
                            << estado.idSesion << ", idBateria =" << estado.idBateria;
        }
        else if(!estado.ipEquipo.isEmpty())
        {
            QMap<QString, Position>::const_iterator itPos
                    = pBattery->posiciones.find(estado.ipEquipo);

            if(itPos != pBattery->posiciones.constEnd())
            {
                // Comprueba los dispositivos de medición (bucles) contenidos en las posiciones
                const Position & position = itPos.value();
                measureDevice.pPosition = &position;
                measureDevice.pCanalBucles = &position.bucles;

                if(!estado.tipoCanalBucle.isClear())
                {
                    canalbucles_type::const_iterator itCanalBucle
                            = position.bucles.find(estado.tipoCanalBucle);
                    if(itCanalBucle != position.bucles.constEnd())
                    {
                        const ChannelLoop & channelLoop = itCanalBucle.value();

                        if(!channelLoop.aMedir.isEmpty())
                        {
                            if(!estado.tipoMedida.isClear())
                            {
                                int idxMedida = channelLoop.aMedir.indexOf(estado.tipoMedida);

                                if(idxMedida >= 0) {
                                    bRet = true;
                                } else {
                                    LOG_ALERT_LN(STATE)
                                            << "No está definida la medida" >> estado.tipoMedida.toString()
                                            << "del bucle" >> channelLoop.tipoCanalBucle.toString()
                                            << "de la posición" >> position.nombre << '('
                                            << position.ip << ") de la batería" >> pBattery->nombre
                                            << '#' << estado.idBateria << "de la sesión"
                                            << estado.idSesion;
                                }
                            }
                        }
                        else
                        {
                            LOG_ALERT_LN(STATE)
                                    << "No está definido el bucle" >> channelLoop.tipoCanalBucle.toString()
                                    << "de la posición" >> position.nombre << '(' << position.ip
                                    << ") de la batería" >> pBattery->nombre << '#' << estado.idBateria
                                    << "de la sesión" << estado.idSesion;
                        }
                    }
                    else
                    {
                        LOG_ALERT_LN(STATE)
                                << "No está definida la posición" >> position.nombre << '('
                                << position.ip << ") de la batería" >> pBattery->nombre
                                << '#' << estado.idBateria << "de la sesión" << estado.idSesion;
                    }
                }
            }
            else
            {
                // Recorre todos los dispositivos de medición (canales) contenidos en los gateways
                QMap<QString, Gateway>::const_iterator itGw
                        = pBattery->gateways.find(estado.ipEquipo);

                if(itGw != pBattery->gateways.constEnd())
                {
                    const Gateway & gateway = itGw.value();
                    measureDevice.pGateway = &gateway;

                    if(estado.slot > 0)
                    {
                        QMap<ushort, Link>::const_iterator itInterfaz
                                = gateway.interfaces.find(estado.slot);

                        if(itInterfaz != gateway.interfaces.constEnd())
                        {
                            const Link & interfaz = itInterfaz.value();
                            measureDevice.pLink = &interfaz;
                            measureDevice.pCanalBucles = &interfaz.canales;

                            if(!estado.tipoCanalBucle.isClear())
                            {
                                canalbucles_type::const_iterator itCanalBucle
                                        = interfaz.canales.find(estado.tipoCanalBucle);

                                if(itCanalBucle != interfaz.canales.constEnd())
                                {
                                    const ChannelLoop & channelLoop = itCanalBucle.value();

                                    if(!channelLoop.aMedir.isEmpty())
                                    {
                                        int idxMedida = channelLoop.aMedir.indexOf(estado.tipoMedida);

                                        if(!estado.tipoMedida.isClear())
                                        {
                                            if(idxMedida >= 0) {
                                                bRet = true;
                                            } else {
                                                LOG_ALERT_LN(STATE)
                                                        << "No está definida la medida" >> estado.tipoMedida.toString()
                                                        << "del bucle" >> channelLoop.tipoCanalBucle.toString()
                                                        << "del interfaz" >> interfaz.tipoInterfaz.toString()
                                                        << "del gateway" >> gateway.nombre << '('
                                                        << gateway.ip << ") de la batería" >> pBattery->nombre
                                                        << '#' << estado.idBateria << "de la sesión"
                                                        << estado.idSesion;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        LOG_ALERT_LN(STATE)
                                                << "No está definido el bucle" >> channelLoop.tipoCanalBucle.toString()
                                                << "del interfaz" >> interfaz.tipoInterfaz.toString()
                                                << "del gateway" >> gateway.nombre << '(' << gateway.ip
                                                << ") de la batería" >> pBattery->nombre << '#' << estado.idBateria
                                                << "de la sesión" << estado.idSesion;
                                    }
                                }
                                else
                                {
                                    LOG_ALERT_LN(STATE)
                                            << "No está definida el gateway" >> gateway.nombre << '('
                                            << gateway.ip << ") de la batería" >> pBattery->nombre
                                            << '#' << estado.idBateria << "de la sesión" << estado.idSesion;
                                }
                            }
                        }
                    }
                }
            }
        }

        if(pMeasureDevice) {
            *pMeasureDevice = measureDevice;
        }

        return bRet;
    }

    bool SAM_DataBase::nextBatteryMeasure(MeasureDevice * pMeasureDevice)
    {
        TRACE(STATE);

        MeasureDevice measureDevice;
        bool bRet = false;

        currentBatteryMeasureDevice(&measureDevice);

        const Equipment * pEquipment = measureDevice.equipment();
        if(pEquipment && measureDevice.pCanalBucles)
        {
            canalbucles_type::const_iterator itCanalBucle
                    = measureDevice.pCanalBucles->find(estado.tipoCanalBucle);

            if(itCanalBucle != measureDevice.pCanalBucles->end())
            {
                const ChannelLoop & channelLoop = itCanalBucle.value();

                int idxMedida;
                if(estado.tipoMedida.isClear()) {
                    idxMedida = 0;
                } else {
                    idxMedida = channelLoop.aMedir.indexOf(estado.tipoMedida);
                }

                if(idxMedida >= 0)
                {
                    if(channelLoop.aMedir.size() > ++idxMedida)
                    {
                        EnumHolder<E_TIPO_MEDIDA> tipoMedida = channelLoop.aMedir[idxMedida];

                        estado.tipoMedida = tipoMedida;

                        bRet = true;
                    }
                    else
                    {
                        estado.tipoMedida.clear();
                    }
                }
                else
                {
                    LOG_WARN_LN(STATE)
                            << "Tipo de medida" >> estado.tipoMedida.toString()
                            << "inválido mientras se accede al bucle"
                            >> channelLoop.tipoCanalBucle.toString() << "del equipo"
                            >> pEquipment->nombre << '('
                            << pEquipment->ip << ") de la batería" << estado.idBateria
                            << "de la sesión" << estado.idSesion;
                }
            }
        }

        if(pMeasureDevice) {
            *pMeasureDevice = measureDevice;
        }

        return bRet;
    }

    bool SAM_DataBase::nextBatteryMeasureChannelLoop(MeasureDevice * pMeasureDevice)
    {
        TRACE(STATE);

        MeasureDevice measureDevice;
        bool bRet = false;

        currentBatteryMeasureDevice(&measureDevice);

        if(measureDevice.pCanalBucles)
        {
            canalbucles_type::const_iterator itCanalBucle;

            if(estado.tipoCanalBucle.isClear()) {
                // Coger el primer canal/bucle
                itCanalBucle = measureDevice.pCanalBucles->constBegin();
            } else {
                // Pasar al siguiente canal/bucle
                itCanalBucle = measureDevice.pCanalBucles->find(estado.tipoCanalBucle);
                if(itCanalBucle != measureDevice.pCanalBucles->end()) {
                    ++itCanalBucle;
                }
            }

            if(itCanalBucle != measureDevice.pCanalBucles->end())
            {
                const ChannelLoop & channelLoop = itCanalBucle.value();

                estado.tipoCanalBucle = channelLoop.tipoCanalBucle;

                bRet = true;
            }
            else
            {
                // Se ha acabado con los canales/bucles del dispositivo
                estado.tipoCanalBucle.clear();
            }

            estado.tipoMedida.clear();
        }

        if(pMeasureDevice) {
            *pMeasureDevice = measureDevice;
        }

        return bRet;
    }

    bool SAM_DataBase::nextBatteryMeasureLink(MeasureDevice * pMeasureDevice)
    {
        TRACE(STATE);

        MeasureDevice measureDevice;
        bool bRet = false;

        currentBatteryMeasureDevice(&measureDevice);

        if(measureDevice.equipment())
        {
            if(measureDevice.pGateway)
            {
                QMap<ushort, Link>::const_iterator itLink;
                if(estado.slot > 0) {
                    itLink = measureDevice.pGateway->interfaces.find(estado.slot);
                    if(itLink != measureDevice.pGateway->interfaces.end()) {
                        ++itLink;
                    }
                } else {
                    itLink = measureDevice.pGateway->interfaces.begin();
                }

                if(itLink != measureDevice.pGateway->interfaces.end())
                {
                    // Se continúa con la siguiente interfaz
                    const Link & link = itLink.value();
                    measureDevice.pLink = &link;
                    measureDevice.pCanalBucles = &link.canales;

                    estado.slot = link.slot;
                    estado.tipoInterfaz = link.tipoInterfaz;

                    bRet = true;
                }
                else
                {
                    // No quedan más interfaces en éste gateway
                    measureDevice.pLink = NULL;
                    measureDevice.pCanalBucles = NULL;

                    //estado.slot = 0;
                    estado.tipoInterfaz.clear();
                }

                estado.tipoCanalBucle.clear();
                estado.tipoMedida.clear();
            }
            else
            {
                LOG_ALERT(STATE) << "No se puede pasar al siguiente enlace porque "
                                    "no hay gateways seleccionados.";
            }
        }

        if(pMeasureDevice) {
            *pMeasureDevice = measureDevice;
        }

        return bRet;
    }

    bool SAM_DataBase::nextBatteryMeasureEquipment(MeasureDevice * pMeasureDevice)
    {
        TRACE(STATE);

        MeasureDevice measureDevice;

        bool bRet = false;
        Battery * pBattery = getBattery(); // Buscar el siguiente equipo a medir

        if(!pBattery)
        {
            LOG_WARN(STATE) << "No se puede medir equipos porque no hay seleccionada una batería.";
        }
        else
        {
            QMap<QString, Position>::iterator itPos;

            if(estado.ipEquipo.isEmpty()) {
                // Inicia el recorrido por las posiciones
                itPos = pBattery->posiciones.begin();
            } else {
                // Se busca la posición en la batería actual
                itPos = pBattery->posiciones.find(estado.ipEquipo);

                if(itPos != pBattery->posiciones.end()) {
                    ++itPos; // Selecciona la siguiente posición
                }
            }

            while(!bRet && itPos != pBattery->posiciones.end()) // Se comprueba que la posición existe
            {
                // Seleccionar los campos para empezar la medición de la posición
                Position & position = itPos.value();

                // Se selecciona ésta posición para medir
                resetBatteryMeasureDevice();
                estado.ipEquipo = position.ip;
                measureDevice.pPosition = &position;

                if(!position.bucles.isEmpty())
                {
                    ChannelLoop & channelLoop = position.bucles.begin().value();
                    measureDevice.pCanalBucles = &position.bucles;

                    if(!channelLoop.aMedir.isEmpty())
                    {
                        //EnumHolder<E_TIPO_MEDIDA> tipoMedida = channelLoop.aMedir.first();
                        bRet = true;
                    }
                    else
                    {
                        LOG_WARN(STATE) << "El bucle" >> channelLoop.tipoCanalBucle.toString()
                                        << "de la posición" >> position.nombre << '('
                                        << position.ip << ") de la batería" >> pBattery->nombre
                                        << '#' << pBattery->idBateria
                                        << "no tiene medidas definidas.";
                    }
                }
                else
                {
                    LOG_WARN(STATE) << "La posición" >> position.nombre << '('
                                    << position.ip << ") de la batería" >> pBattery->nombre
                                    << '#' << pBattery->idBateria
                                    << "no tiene bucles definidos para medir.";
                }

                ++itPos;
            }

            if(!bRet) // No hay posiciones que medir; se continúa con los gateways
            {
                QMap<QString, Gateway>::iterator itGw;
                itGw = pBattery->gateways.find(estado.ipEquipo);

                if(itGw == pBattery->gateways.end()) {
                    // Si no se encuentra, comenzar desde el primer gateway
                    itGw = pBattery->gateways.begin();
                } else {
                    ++itGw; // Selecciona el siguiente gateway
                }

                while(!bRet && itGw != pBattery->gateways.end()) // Se comprueba que el gateway existe
                {
                    // Seleccionar los campos para empezar la medición del gateway
                    Gateway & gateway = itGw.value();
                    resetBatteryMeasureDevice();
                    estado.ipEquipo = gateway.ip;
                    measureDevice.pGateway = &gateway;

                    // Comprobar que es un gateway adecuado para la medición
                    if(!gateway.interfaces.isEmpty())
                    {
                        Link & enlace = gateway.interfaces.begin().value();
                        if(!enlace.canales.isEmpty())
                        {
                            ChannelLoop & channelLoop = enlace.canales.begin().value();

                            if(!channelLoop.aMedir.isEmpty())
                            {
//                                EnumHolder<E_TIPO_MEDIDA> tipoMedida = channelLoop.aMedir.first();

//                                // Se selecciona éste enlace para medir
//                                estado.slot = enlace.slot;
//                                estado.tipoInterfaz = enlace.tipoInterfaz;
//                                estado.tipoCanalBucle = channelLoop.tipoCanalBucle;
//                                estado.tipoMedida = tipoMedida;

                                bRet = true;
                            }
                            else
                            {
                                LOG_WARN(STATE) << "El bucle" >> channelLoop.tipoCanalBucle.toString()
                                                << "del enlace" >> enlace.tipoInterfaz.toString()
                                                << '#' << enlace.slot
                                                << "del gateway" >> gateway.nombre << '('
                                                << gateway.ip << ") de la batería" >> pBattery->nombre
                                                << '#' << pBattery->idBateria
                                                << "no tiene medidas definidas.";
                            }
                        }
                        else
                        {
                            LOG_WARN(STATE) << "El enlace" >> enlace.tipoInterfaz.toString()
                                            << '#' << enlace.slot
                                            << "del gateway" >> gateway.nombre << '('
                                            << gateway.ip << ") de la batería" >> pBattery->nombre
                                            << '#' << pBattery->idBateria
                                            << "no tiene canales definidos para medir.";
                        }
                    }
                    else
                    {
                        LOG_WARN(STATE) << "El gateway" >> gateway.nombre << '('
                                        << gateway.ip << ") de la batería" >> pBattery->nombre
                                        << '#' << pBattery->idBateria
                                        << "no tiene enlaces definidos para medir.";
                    }

                    ++itGw;
                }
            }
        }

        if(pMeasureDevice)  {
            *pMeasureDevice = measureDevice;
        }
//        if(bRet) {
//            currentBatteryMeasureDevice(pMeasureDevice);
//        }

        return bRet;
    }

    void SAM_DataBase::resetBatteryMeasureDevice()
    {
        // Limpia todos los índices relativos a la batería
        estado.ipEquipo.clear();
        estado.slot = 0;
        estado.tipoInterfaz.clear();
        estado.tipoCanalBucle.clear();
        estado.tipoMedida.clear();
    }

    void SAM_DataBase::countInner(MeasuresCount *pMeasuresCount, MeasureDevice * pMeasureDevice)
    {
        while(nextBatteryMeasureChannelLoop(pMeasureDevice))
        {
            pMeasureDevice->pGateway ? ++pMeasuresCount->nChannels : ++pMeasuresCount->nLoops;
            while(nextBatteryMeasure(pMeasureDevice)) {
                ++pMeasuresCount->nMeasures;
            }
        }
    }

    uint SAM_DataBase::countBatteryMeasureDevices(MeasuresCount *pMeasuresCount)
    {
        MeasuresCount measuresCount;
        MeasureDevice measureDevice;

        // Limpia todos los índices relativos a la batería
        resetBatteryMeasureDevice();

        while(nextBatteryMeasureEquipment(&measureDevice))
        {
            if(measureDevice.pGateway)
            {
                ++measuresCount.nGateways;

                while(nextBatteryMeasureLink(&measureDevice))
                {
                    ++measuresCount.nLinks;

                    countInner(&measuresCount, &measureDevice);
                }
            }
            else
            {
                ++measuresCount.nPositions;
                countInner(&measuresCount, &measureDevice);
            }
        }

        if(pMeasuresCount) {
            *pMeasuresCount = measuresCount;
        }

        return measuresCount.nMeasures;
    }

    int SAM_DataBase::storeSessions(const QString & dirPath)
    {
        TRACE(STATE);

        int nStored = 0;
        QDir dir;
        if(!dirPath.isEmpty())
        {
            dir.cd(dirPath);

            if(!QDir().mkpath(dirPath)) {
                LOG_INFO(COMS) << "Creada la carpeta" >> dirPath << "para "
                                  "almacenar las sesiones, baterías, y mediciones del SAM.";
            }
        }

        QMapIterator<uint, UserSession> itSesion(sesionesUsuario);
        while (itSesion.hasNext())
        {
            itSesion.next();
            uint idSesion = itSesion.key();
            const UserSession & sesion = itSesion.value();

            QString fileName = dir.filePath(QString("session_%1.json").arg(idSesion));
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly))
            {
                JVariant jSesion;
                jSesion["sesion"] = sesion.toVariant();

                QString strSesion(jSesion.toJson());

                quint64 nWritten = file.write(strSesion.toUtf8());
                file.close();

                LOG_INFO(STATE) << "Escritos" << nWritten << "bytes del archivo de sesión"
                                >> fileName;

                ++nStored;
            }
            else
            {
                LOG_ERROR(STATE) << "No se puede crear el archivo" >> fileName
                                 << "para almacenar el estado.";
            }
        }

        /**
         * @todo 5.2.3.3 Resultado de Pruebas de Red
            Al finalizar la Ejecución del análisis de red correctamente, el servidor SAM debe generar y guardar localmente en un fichero los resultados de las pruebas de red con los resultados obtenidos.
            El servidor SAM debe guardar solamente el último resultado de pruebas de red, borrando el resultado anterior (si lo hubiera). [(parámetro de diseño), SAMDP_NumInformes]
            Cuando el cliente SAM solicite el último resultado de pruebas de red guardado en el servidor, el servidor SAM se lo debe enviar.
            El resultado de Pruebas de Red debe contener:
            * fecha: fecha de inicio de ejecución de las pruebas.
            * idCentro: ID del centro en el que se realizaron las pruebas.
            * idConfig: ID de la config. bajo la cual se realizaron las pruebas.
            * subIdConfig: SubID de la config. bajo la cual se realizaron las pruebas.
            * numMedidas: número de medidas realizadas.
            * medida: estructura RES_MEDIDA repetida numMedidas veces.
         */
        return nStored;
    }

    int SAM_DataBase::loadSessions(const QString & dirPath)
    {
        TRACE(STATE);

        int nLoaded = 0;
        QDir dir(dirPath);

        // Carga los archivos de sesión que pueden contener opcionalmente baterías y mediciones
        QStringList sessionFileNames = dir.entryList(
                    QStringList() << "session_*.json" << "session_*.json.zip",
                    QDir::Files | QDir::Hidden | QDir::NoSymLinks,
                    QDir::Time);

        int nSesiones = sessionFileNames.size();

        if(nSesiones <= 0)
        {
            LOG_ALERT(STATE) << "No se han encontrado sesiones en el directorio"
                             >> dirPath;
        }
        else
        {
            LOG_INFO(STATE) << "Cargando las" << nSesiones
                            << "sesiones desde el directorio" >> dirPath;

            for (int i = 0; i < nSesiones; ++i)
            {
                QString fileName(sessionFileNames.at(i));

                // Recupera los números de sesión
                QRegExp rx("session_(\\d+)\\.json(?:\\.zip)?");

                int pos = rx.indexIn(fileName);

                if (pos > -1)
                {
                    JVariant jSesion;
                    fileName = dir.filePath(fileName);
                    jSesion.parseFile(fileName);

                    if(jSesion.isNull())
                    {
                        LOG_ALERT(STATE) << "No se ha podido cargar la sesión" >> fileName;
                    }
                    else
                    {
                        uint idSesion = rx.cap(1).toUInt();

                        // Crea o sobreescribe la sesión desde el archivo
                        UserSession & sesion = sesionesUsuario[idSesion];
                        sesion = jSesion["sesion"];

                        ++nLoaded;
                        LOG_INFO(STATE) << "Cargada la sesión" << fileName;
                    }
                }
            }
        }

        // Carga archivos de baterías y mediciones.
        // Es necesario que las sesiones cargadas ya existan
        QStringList batteryFileNames = dir.entryList(
                    QStringList() << "session_*_battery_*.json" << "session_*_battery_*.json.zip",
                    QDir::Files | QDir::Hidden | QDir::NoSymLinks,
                    QDir::Time);

        int nBaterias = batteryFileNames.size();

        if(nBaterias <= 0)
        {
            LOG_DEBUG(STATE) << "No se han encontrado archivos de baterías en el directorio"
                             >> dirPath;
        }
        else
        {
            LOG_INFO(STATE) << "Cargando las" << nBaterias
                            << "baterías desde el directorio" >> dirPath;

            for (int i = 0; i < nBaterias; ++i)
            {
                QString fileName(batteryFileNames.at(i));

                // Recupera los números de sesión y de batería
                QRegExp rx("session_(\\d+)_battery_(\\d+)\\.json(?:\\.zip)?");

                int pos = rx.indexIn(fileName);

                if (pos > -1)
                {
                    JVariant jBateria;
                    jBateria.parseFile(fileName);

                    if(jBateria.isNull())
                    {
                        LOG_ALERT(STATE) << "No se ha podido cargar la batería" >> fileName;
                    }
                    else
                    {
                        uint idSesion = rx.cap(1).toUInt();
                        uint idBateria = rx.cap(2).toUInt();

                        if(sesionesUsuario.contains(idSesion))
                        {
                            // Crea o sobreescribe la batería desde el archivo
                            Battery & bateria = sesionesUsuario[idSesion].baterias[idBateria];
                            bateria = jBateria["bateria"];

                            ++nLoaded;
                            LOG_INFO(STATE) << "Cargada la batería" >> fileName;
                        }
                        else
                        {
                            LOG_ALERT(STATE) << "No se puede cargar la batería " >> fileName
                                             << "porque no se ha encontrado la sesión" << idSesion;
                        }
                    }
                }
            }
        }

        return nLoaded;
    }
}
