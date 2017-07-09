/** @file SAM_DataBase.h
*   @brief Estructuras auxiliares para representar tanto las baterías como las mediciones.
*   @date   13-02-2013 (dd-mm-yyyy)
*   @author David Jurado González
*   @version 0.1
*/

#ifndef SAM_DATABASE_H
#define SAM_DATABASE_H

#include <QLocale>
#include <QMap>

#include "SAM_DeclarationToString.h"
#include "SAM_JVariant.h"

namespace SAM
{
    using namespace SAM_DeclarationParser;

    struct MeasuresCount;

    /**
     * @brief Devuelve `true` si `number` es un número Real, es decir,
     *  está dentro de los límites permitidos para ese número.
     *  Devuelve `false` si es `inf` o `nan`.
     *
     * @param number
     * @return bool
     */
    bool isNumber(float number);

    /**
     * @brief Parámetros de la medida, con los umbrales que tuviera definidos.
     *  Se tendrá un array de definiciones de medidas por cada sesión de usuario clasificadas
     *  por E_TIPO_MEDIDA. Cada valor de medida que se obtenga se
     *
     *  5.2.4.3.1 Medidas
        Para cada una de las medidas, el servidor SAM debe definir los siguientes campos:

        * IDmedida: identificador de la medida.
        * Tipo: tipo de la medida:
            - AUDIO: si se trata de una medida que se realiza en el Análisis de Audio Analógico.
            - RED: si se trata de una medida que se realiza en el Análisis de Red.
        * NombreMedida: nombre de la medida.
        * ValorOptimo: valor óptimo de la medida. Indica cuál es el valor deseado para la medida, con dos posibles valores:
            - MINIMO: la medida es mejor cuanto menor sea.
            - MAXIMO: la medida es mejor cuanto mayor sea.
        * rMin: umbral inferior para la medida (campo opcional).
        * rMax: umbral superior para la medida (campo opcional).
        * unidad: unidad de medida

        Cuando el cliente SAM solicite el listado de medidas de audio, el servidor SAM le debe enviar los datos de las medidas de audio configuradas de tipo "AUDIO".
        Cuando el cliente SAM solicite el listado de medidas de red, el servidor SAM le debe enviar los datos de las medidas de audio configuradas de tipo "RED".
     */
    struct MeasureDefinition
    {
        EnumHolder<E_TIPO_MEDIDA> tipoMedida; ///< IDmedida: Identificador de la medida.
        EnumHolder<E_TIPO_AUDIO_RED> tipo; ///< Tipo audio o red

        QString nombre; ///< Nombre de la medida
        EnumHolder<E_VALOR_OPTIMO> valorOptimo; ///< Indica cuál es el valor deseado para la medida, con dos posibles valores

        float rMin; ///< umbral inferior para la medida (campo opcional). Usar qNaN para nulo.
        float rMax; ///< umbral superior para la medida (campo opcional). Usar qNaN para nulo.

        QString unidad; ///< unidad de medida

        MeasureDefinition();
        MeasureDefinition(const QVariantMap & in);
        MeasureDefinition(const QVariant & in);
        QVariantMap toVariant() const;
        operator QVariant () const { return toVariant(); }
        void fromVariantMap(const QVariantMap & in);
        MeasureDefinition & operator =(const QVariant & in) { fromVariantMap(in.toMap()); return *this; }
    };

    /**
     * @brief Estado de la máquina de estados
     */
    struct MeasuresState
    {
        uint idSesion;  ///< Identificador de la sesión actual
        uint idBateria; ///< Identificador de la batería actual.
        QString ipPatron; ///< IP del patrón actualmente seleccionado
        QString ipEquipo; ///< IP del puesto o del gateway actualmente seleccionado

        ushort slot; ///< Slot de la interfaz actualemnte en uso
        EnumHolder<E_TIPO_ENLACE> tipoInterfaz; ///< Tipo de interfaz

        EnumHolder<E_ID_CANAL_BUCLE> tipoCanalBucle; ///< Tipo de bucle o canal
        EnumHolder<E_TIPO_MEDIDA> tipoMedida; ///< Tipo de medida

        MeasuresState() : idSesion(0), idBateria(0) {}
        MeasuresState(const QVariantMap & in) : idSesion(0), idBateria(0) { fromVariantMap(in); }
        MeasuresState(const QVariant & in) : idSesion(0), idBateria(0) { fromVariantMap(in.toMap()); }
        QVariantMap toVariant() const;
        operator QVariant () const { return toVariant(); }
        void fromVariantMap(const QVariantMap & in);
        MeasuresState & operator =(const QVariant & in) { fromVariantMap(in.toMap()); return *this; }

        void clear();
    };

    /**
     * @brief Estructura para almacenar el estado de los equipos: puestos y gateways
     */
    struct Equipment
    {
        ushort id; /**< Identificador del equipo tal como se tenía en el archivo de configuración.
                     Tabla [T_PUES] o [T_GATE].
                     */
        QString ip; ///< Dirección IP característica del equipo.
        QString nombre; ///< Nombre del equipo

        Equipment() : id(0) {}
        Equipment(const QVariantMap & in) : id(0) { fromVariantMap(in); }
        Equipment(const QVariant & in) : id(0) { fromVariantMap(in.toMap()); }
        QVariantMap toVariant() const;
        operator QVariant () const { return toVariant(); }
        void fromVariantMap(const QVariantMap & in);
        Equipment & operator =(const QVariant & in) { fromVariantMap(in.toMap()); return *this; }
    };

    /**
     * @brief Estructura para almacenar el estado de bucles/canales
     */
    struct ChannelLoop
    {
        EnumHolder<E_ID_CANAL_BUCLE> tipoCanalBucle; ///< Tipo de bucle o canal
        QList< EnumHolder<E_TIPO_MEDIDA> > aMedir; ///< Medidas que se van a hacer en el bucle o canal

        ChannelLoop() {}
        ChannelLoop(const QVariantMap & in) { fromVariantMap(in); }
        ChannelLoop(const QVariant & in) { fromVariantMap(in.toMap()); }
        QVariantMap toVariant() const;
        operator QVariant () const { return toVariant(); }
        void fromVariantMap(const QVariantMap & in);
        ChannelLoop & operator =(const QVariant & in) { fromVariantMap(in.toMap()); return *this; }
    };

    /**
     * @brief Clasifica la medida según los umbrales proporcionados.
     *
     * @param valor La medida realizada
     * @param valorOptimo Indica cuál es el valor deseado para la medida, con dos posibles valores
     * @param rMin Umbral inferior para la medida (campo opcional). Usar qNaN para nulo.
     * @param rMax Umbral superior para la medida (campo opcional). Usar qNaN para nulo.
     * @return E_RES_MEDIDA Clasificación de la medida
     */
    E_RES_MEDIDA classifyMeasure(float valor,
                                 E_VALOR_OPTIMO valorOptimo,
                                 float rMin,
                                 float rMax);

    /**
     * @brief Estructura para almacenar el resultado de una medida
     */
    struct MeasureResult
    {
        EnumHolder<E_TIPO_MEDIDA> tipoMedida; ///< Identificador de la medida
        float valor; ///< Valor de la medida para el canal
        EnumHolder<E_RES_MEDIDA> clasificacion; /// Clasificación de la medida

        MeasureResult() {}
        MeasureResult(const QVariantMap & in) { fromVariantMap(in); }
        MeasureResult(const QVariant & in) { fromVariantMap(in.toMap()); }
        QVariantMap toVariant() const;
        operator QVariant () const { return toVariant(); }
        void fromVariantMap(const QVariantMap & in);
        MeasureResult & operator =(const QVariant & in) { fromVariantMap(in.toMap()); return *this; }
    };

    /**
     * @brief Estructura para almacenar el resultado de las medidas de canales/bucles
     */
    struct ChannelLoopMeasure
    {
        EnumHolder<E_ID_CANAL_BUCLE> tipoCanalBucle; ///< Tipo de bucle o canal
        QMap<EnumHolder<E_TIPO_MEDIDA>, MeasureResult> medidas;

        ChannelLoopMeasure() {}
        ChannelLoopMeasure(const QVariantMap & in) { fromVariantMap(in); }
        ChannelLoopMeasure(const QVariant & in) { fromVariantMap(in.toMap()); }
        QVariantMap toVariant() const;
        operator QVariant () const { return toVariant(); }
        void fromVariantMap(const QVariantMap & in);
        ChannelLoopMeasure & operator =(const QVariant & in) { fromVariantMap(in.toMap()); return *this; }
    };

    /**
     * @brief Estructura para almacenar el resultado de las medidas de los equipos
     */
    struct DeviceResults
    {
        typedef QMap<EnumHolder<E_ID_CANAL_BUCLE>, ChannelLoopMeasure> measure_container_type;
        QDateTime fecha; ///< Fecha de medición (inicio de la ejecución de las pruebas de éste equipo >= `Battery::ultimaEjecucion`)
        EnumHolder<E_RESULTADO> clasificacion; ///< Clasificación de resultados obtenidos
        measure_container_type medidaBucles; ///< Resultado de las medidas realizadas para una posición

        DeviceResults() {}
        DeviceResults(const QVariantMap & in) { fromVariantMap(in); }
        DeviceResults(const QVariant & in) { fromVariantMap(in.toMap()); }
        QVariantMap toVariant() const;
        operator QVariant () const { return toVariant(); }
        void fromVariantMap(const QVariantMap & in);
        DeviceResults & operator =(const QVariant & in) { fromVariantMap(in.toMap()); return *this; }
        /**
         * @brief Clasifica los resultados desde `medidaBucles`
         *  y actualiza la variable `clasificacion`.
         */
        void classify();
    };

    /**
     * @brief Tipo para almacenar una colección de bucles o de canales ya se use para
     *  posiciones como para interfaces.
     */
    typedef QMap<EnumHolder<E_ID_CANAL_BUCLE>, ChannelLoop> canalbucles_type;

    /**
     * @brief Estructura para almacenar el estado de una posición.
     */
    struct Position : public Equipment
    {
        canalbucles_type bucles; ///< Batería de mediciones para todos los bucles de la posición

        /**
         *  5.2.2.2 Informe de Pruebas de Audio
         *  El servidor SAM debe guardar solamente el último
         *  resultado de pruebas de una batería de pruebas ejecutada,
         *  borrando el informe anterior, si lo hubiera (parámetro de diseño)
         */
        // Sólo seria necesario almacenar la última medida, no una lista de las últimas ejecuciones,
        // pero se mantiene como una lista por si acaso se requierese en un futuro
        QList<DeviceResults> medidas; ///< Medidas de calidad realizadas

        Position() {}
        Position(const QVariantMap & in) { fromVariantMap(in); }
        Position(const QVariant & in) { fromVariantMap(in.toMap()); }
        QVariantMap toVariant() const;
        operator QVariant () const { return toVariant(); }
        void fromVariantMap(const QVariantMap & in);
        Position & operator =(const QVariant & in) { fromVariantMap(in.toMap()); return *this; }
    };

    /**
     * @brief Estructura para almacenar el estado de un enlace (interfaz)
     */
    struct Link
    {
        ushort id; ///< Identificador de la interfaz en la tabla [T_ENL]
        ushort slot; ///< Hueco de la interfaz en el gateway. 0 se considera nulo.
        EnumHolder<E_TIPO_ENLACE> tipoInterfaz;
        canalbucles_type canales;
        /**
         *  5.2.2.2 Informe de Pruebas de Audio
         *  El servidor SAM debe guardar solamente el último
         *  resultado de pruebas de una batería de pruebas ejecutada,
         *  borrando el informe anterior, si lo hubiera (parámetro de diseño)
         */
        // Sólo seria necesario almacenar la última medida, no una lista de las últimas ejecuciones,
        // pero se mantiene como una lista por si acaso se requierese en un futuro
        QList<DeviceResults> medidas; ///< Medidas de calidad realizadas

        Link() : id(0), slot(0) {}
        Link(const QVariantMap & in) : id(0), slot(0) { fromVariantMap(in); }
        Link(const QVariant & in) : id(0), slot(0) { fromVariantMap(in.toMap()); }
        QVariantMap toVariant() const;
        operator QVariant () const { return toVariant(); }
        void fromVariantMap(const QVariantMap & in);
        Link & operator =(const QVariant & in) { fromVariantMap(in.toMap()); return *this; }
    };

    /**
     * @brief Estructura para almacenar el estado de un gateway
     */
    struct Gateway : public Equipment
    {
        QMap<ushort, Link> interfaces; // Slot -> Interfaz

        Gateway() {}
        Gateway(const QVariantMap & in) { fromVariantMap(in); }
        Gateway(const QVariant & in) { fromVariantMap(in.toMap()); }
        QVariantMap toVariant() const;
        operator QVariant () const { return toVariant(); }
        void fromVariantMap(const QVariantMap & in);
        Gateway & operator =(const QVariant & in) { fromVariantMap(in.toMap()); return *this; }
    };


    /**
     * @brief Estructura para almacenar las baterías de pruebas y sus medidas asociadas.
     */
    struct Battery
    {
        uint idBateria; ///< ID de la batería
        QString nombre; ///< Nombre de la bateria de pruebas a crear
        QDateTime creacion; ///< Fecha de creación de la batería
        QDateTime modificacion; ///< Fecha de modificación de la batería
        QDateTime ultimaEjecucion; ///< Fecha del inicio de la última ejecución de la batería.
        ushort idConfig; ///< ID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        ushort subIdConfig; ///< SubID de la configuración bajo la cual se ejecutó la BdP (2 bytes)
        ushort idCentro; ///< Identificador del centro en el que se ejecutó la BdP

        /**
         * @brief Las posiciones pueden cambiar dinámicamente de ID por lo que se almacenan
         *  ordenadas por IP.
         */
        QMap<QString, Position> posiciones;

        /**
         * @brief Los gateways pueden cambiar dinámicamente de IP por lo que se almacenan
         *  ordenados por su IP característica.
         */
        QMap<QString, Gateway> gateways;

        Battery() : idBateria(0), idConfig(0), subIdConfig(0), idCentro(0) {}
        Battery(const QVariantMap & in) : idBateria(0), idConfig(0), subIdConfig(0), idCentro(0) { fromVariantMap(in); }
        Battery(const QVariant & in) : idBateria(0), idConfig(0), subIdConfig(0), idCentro(0) { fromVariantMap(in.toMap()); }
        QVariantMap toVariant() const;
        QString toJson() const;
        operator QVariant () const { return toVariant(); }
        void fromVariantMap(const QVariantMap & in);
        Battery & operator =(const QVariant & in) { fromVariantMap(in.toMap()); return *this; }
    };


    /**
     * @brief Almacena una sesión de usuario.
     */
    struct UserSession
    {
        uint idSesion; ///< Sesión de usuario
        QString rol; ///< Rol del usuario: "Administrador" o "Pruebas"
        QDateTime creacion; ///< Fecha de creación de la sesión
        QDateTime modificacion; ///< Fecha de modificación de la sesión, ya sea por baterías o mediciones
        QList<unsigned char> parametros; ///< @todo Averiguar qué son estos parámetros (Seguramente son las definiciones de medidas)
        QMap<EnumHolder<E_TIPO_MEDIDA>, MeasureDefinition> definicionMedidas; ///< Definición de umbrales para las medidas de toda la sesión

        QMap<uint, Battery> baterias;

        UserSession() : idSesion(0) { setDefaultMeasureDefinitions(); }
        UserSession(const QVariantMap & in) : idSesion(0) { fromVariantMap(in); }
        UserSession(const QVariant & in) : idSesion(0) { fromVariantMap(in.toMap()); }
        QVariantMap toVariant() const;
        operator QVariant () const { return toVariant(); }
        void fromVariantMap(const QVariantMap & in);
        UserSession & operator =(const QVariant & in) { fromVariantMap(in.toMap()); return *this; }
        void setDefaultMeasureDefinitions(); ///< Rellena el miembro definicionMedidas con valores por defecto
    };

    /**
     * @brief Mensaje capturado de la red, (PRES, ESTF, ESRD, ...)
     */
    struct EquipmentMessage
    {
        QDateTime fecha;
        JVariant mensaje; ///< Mensaje capturado de la red
    };

    /**
     * @brief Estructura auxiliar para iterar por todos los dispositivos (canales/bucles - tipoMedida)
     *  de la batería actual.
     */
    struct MeasureDevice
    {
        const Position * pPosition; ///< Posición en la que se está realizando la medición
        const Gateway * pGateway; ///< Gateway en el que se está realizando la medición
        const Link * pLink; ///< Interfaz del gateway (o NULL se es una posición)
        const canalbucles_type * pCanalBucles; ///< Colección de canal/bucles

        MeasureDevice() : pPosition(NULL), pGateway(NULL), pLink(NULL), pCanalBucles(NULL) {}

        const Equipment * equipment()
        {
            return pPosition ? static_cast<const Equipment*>(pPosition)
                             : static_cast<const Equipment*>(pGateway);
        }

        void clear()
        {
            pPosition = NULL;
            pGateway = NULL;
            pLink = NULL;
            pCanalBucles = NULL;
        }

    };

    /**
     * @brief Base de Datos de todo lo relativo a las Baterías y Mediciones,
     *  incluido el punto actual de ejecución.
     */
    struct SAM_DataBase
    {
        static uint s_idSesion; ///< Siguiente identificador de sesión

        MeasuresState estado;
        JVariant mensajes; ///< IP -> idMsg -> QVariant(Message)
        QMap<uint, UserSession> sesionesUsuario;

        SAM_DataBase() {}
        SAM_DataBase(const QVariantMap & in) { fromVariantMap(in); }
        SAM_DataBase(const QVariant & in) { fromVariantMap(in.toMap()); }
        QVariantMap toVariant() const;
        QString toJson() const;
        operator QVariant () const { return toVariant(); }
        void fromVariantMap(const QVariantMap & in);
        SAM_DataBase & operator =(const QVariant & in) { fromVariantMap(in.toMap()); return *this; }

        /**
         * @brief Carga las sesiones (baterías y mediciones) desde el sistema de archivos.
         *
         * @param filePath Ruta del directorio que contiene los archivos.
         *  Los archivos de sesión tienen el formato "session_*.json", mientras que los de
         *  batería tienen el formato "session_*_battery_*.json" donde los asteriscos son
         *  los números de sesión y batería respectivamente.
         * @return int Retorna el número de archivos que ha leído.
         */
        int loadSessions(const QString & dirPath);

        /**
         * @brief Almacena las sesiones (baterías y mediciones) al sistema de archivos.
         *
         * @param filePath Ruta del directorio donde se almacenarán los archivos.
         *  Los archivos de sesión tienen el formato "session_*.json", mientras que los de
         *  batería tienen el formato "session_*_battery_*.json" donde los asteriscos son
         *  los números de sesión y batería respectivamente.
         * @return int Retorna el número de archivos que ha escrito.
         */
        int storeSessions(const QString & dirPath);

        /**
         * @brief Obtiene la batería actual teniendo en cuenta el `estado`, o NULL si no hay batería actual.
         *
         * @return Battery
         */
        Battery * getBattery();
        const Battery * getBattery() const;

        /**
         * @brief Devuelve la batería identificada por idSesion e idBateria, o NULL si no existe.
         *
         * @param idSesion
         * @param idBateria
         * @return Battery
         */
        Battery * getBattery(uint idSesion, uint idBateria);
        const Battery * getBattery(uint idSesion, uint idBateria) const;

        /**
         * @brief Devuelve el actual dispositivo sobre el que realizar la lectura/escritura.
         *
         * @param pMeasureDevice Puntero a una variable que será rellenada con lo necesario
         *  para almacenar una nueva medida.
         * @return bool false si no se está referenciando a un dispositivo de medición válido.
         */
        bool currentBatteryMeasureDevice(MeasureDevice * pMeasureDevice = NULL) const;

        /**
         * @brief Devuelve el siguiente dispositivo sobre el que realizar la lectura/escritura.
         *  Avanza a la siguiente medida en la batería.
         *  // Si no quedan más medidas en el bucle/canal actual, pasa al siguiente bucle/canal.
         *  // Si no quedan más bucles/canales en el enlace o posición actual, pasa al siguiente enlace o posición.
         *  // Si se están midiendo interfaces y no quedan más enlaces en el gateway actual, pasa al siguiente gateway.
         *  // Si no quedan más equipos que medir (posiciones o gateways), devuelve false.
         *  Actualiza la variable miembro `estado` para reflejar la referencia al nuevo dispositivo.
         *
         * @param pMeasureDevice Puntero a una variable que será rellenada con lo necesario
         *  para almacenar una nueva medida si no se ha finalizado.
         * @return bool false si se han terminado con todas las medidas del canal/bucle actual.
         *   En éste caso no se rellenará el contenido apuntado por pMeasureDevice.
         */
        bool nextBatteryMeasure(MeasureDevice * pMeasureDevice = NULL);

        /**
         * @brief Devuelve el siguiente dispositivo sobre el que realizar la lectura/escritura.
         *  Avanza al siguiente canal/bucle de la interfaz o posición actual.
         *  //Si no quedan más bucles/canales en el enlace o posición actual, pasa al siguiente enlace o posición.
         *  //Si se están midiendo interfaces y no quedan más enlaces en el gateway actual, pasa al siguiente gateway.
         *  //Si no quedan más equipos que medir (posiciones o gateways), devuelve false.
         *  Actualiza la variable miembro `estado` para reflejar la referencia al nuevo dispositivo.
         *
         * @param pMeasureDevice Puntero a una variable que será rellenada con lo necesario
         *  para almacenar una nueva medida.
         * @return bool false si se han terminado con canal/bucles de éste dispositivo.
         *  En éste caso no se rellenará el contenido apuntado por pMeasureDevice.
         */
        bool nextBatteryMeasureChannelLoop(MeasureDevice * pMeasureDevice = NULL);

        /**
         * @brief Devuelve el siguiente dispositivo sobre el que realizar la lectura/escritura.
         *  Avanza a la siguiente interfaz del gateway actual.
         *  //Si no quedan más enlaces en el gateway actual, pasa al siguiente gateway.
         *  //Si no quedan más equipos que medir (posiciones o gateways), devuelve false.
         *  Actualiza la variable miembro `estado` para reflejar la referencia al nuevo dispositivo.
         *
         * @param pMeasureDevice Puntero a una variable que será rellenada con lo necesario
         *  para almacenar una nueva medida.
         * @return bool false si se han terminado con todas las interfaces de éste gateway.
         *  En éste caso no se rellenará el contenido apuntado por pMeasureDevice.
         */
        bool nextBatteryMeasureLink(MeasureDevice * pMeasureDevice = NULL);

        /**
         * @brief Devuelve el siguiente dispositivo sobre el que realizar la lectura/escritura.
         *  Avanza al siguiente equipo de la batería actual.
         *  Si no quedan más equipos que medir (posiciones o gateways), devuelve false.
         *  Actualiza la variable miembro `estado` para reflejar la referencia al nuevo dispositivo.
         *
         * @param pMeasureDevice Puntero a una variable que será rellenada con lo necesario
         *  para almacenar una nueva medida.
         * @return bool false si se han terminado con todos los equipos de ésta batería.
         *  En éste caso no se rellenará el contenido apuntado por pMeasureDevice.
         */
        bool nextBatteryMeasureEquipment(MeasureDevice * pMeasureDevice = NULL);

        /**
         * @brief Reinicia el cursor para la lectura/escritura del siguiente tipo de medida.
         *  Actualiza la variable miembro `estado` para eliminar referencias a dispositivos.
         */
        void resetBatteryMeasureDevice();

        /**
         * @brief Rellena la estructura ´MeasuresCount´ con la cuenta de todos los dispositivos
         *  de medida del sistema (canales, bucles, ...).
         *  Ésta función llama a `resetBatteryMeasureDevice()` que resetea los contadores
         *  y los índices a los dispositivos, por lo que no debe ser llamada en medio de
         *  una batería de pruebas.
         *
         * @param pMeasuresCount
         */
        uint countBatteryMeasureDevices(MeasuresCount * pMeasuresCount);

    private:
        void countInner(MeasuresCount *pMeasuresCount, MeasureDevice * pMeasureDevice);
    };
}

#endif // SAM_DATABASE_H
