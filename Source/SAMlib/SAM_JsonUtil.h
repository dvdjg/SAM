#ifndef SAM_JSONUTIL_H
#define SAM_JSONUTIL_H

#include <QVariant>
#include <QString>

class QTextStream;

namespace SAM
{
    /*!
        @class JsonUtil
        @brief Funciones de utilidad para procesar documentos JSON incluidas
            serialización/deserialización desde/hacia JSON.
            Se implementa el formato ligero de intercambio de datos JSON (JavaScript Object Notation).
        @see http://www.json.org/
        @see http://chris.photobooks.com/json/

        Tipo JSON   | Tipo Qt
        ------------|--------------------------------------
        object      | QMap<QString, QVariant>/QVariantHash
        array       | QList<QVariant>/QStringList
        string      | QString
        number      | int,double
        true        | bool
        false       | bool
        null        | QVariant()


    */
    class JsonUtil {
    public:
        typedef QVariant (*fn_parse_type)(QTextStream &);

        /**
         * @brief Analiza el texto JSON y genera un árbol de objetos Qt que lo represente.
         *
         * @param s Flujo de texto de entrada que se va a analizar.
         *  Acepta también la dirección de un QString o de un QByteArray.
         *
         * @return QVariant El árbol de objetos Qt que se ha detectado
         *  o QVariant() si no lo ha podido hacer.
         */
        static QVariant parseJson(QTextStream &s);

        /**
         * @brief Analiza el texto JSON y genera un árbol de objetos Qt que lo represente.
         *
         * @param str Cadena de texto de entrada que se va a analizar.
         *
         * @return QVariant El árbol de objetos Qt que se ha detectado
         *  o QVariant() si no lo ha podido hacer.
         */
        static QVariant parseJson(QString str);

        /**
         * @brief Función auxiliar para analizar un stream con el formato del SCV.
         *
         * @param in Flujo de texto de entrada que se va a analizar.
         *
         * @return QVariant El árbol de objetos Qt que se ha detectado
         *  o QVariant() si no lo ha podido hacer.
         */
        static QVariant parseScv(QTextStream & in);

        /**
         * @brief Función auxiliar para analizar una cadena de texto con el formato del SCV.
         *
         * @param str Cadena de texto de entrada que se va a analizar.
         *
         * @return QVariant El árbol de objetos Qt que se ha detectado
         *  o QVariant() si no lo ha podido hacer.
         */
        static QVariant parseScv(QString str);


        /**
         * @brief Convierte una cadena de texto en un tipo válido admitido por JSON.
         *
         * @param value Cadena de texto con un valor a convertir. Ejemplo:
         *  valor   | JSON
         *  --------|---------------
         *  "SI"    | true
         *  "234"   | 234
         *  "hola"  | "hola"
         *  ""      | null
         *  "as,d,v"| ["as","d","v"]
         *
         * @return QVariant Tipo compatible con JSON
         */
        static QVariant parseStringValue(QString value);

        /**
         * @brief Es la función opuesta a parse().
         *  Transforma un árbol de objetos Qt en una representación JSON en forma de texto.
         *
         * @param v Nodo raíz del árbol de objetos.
         *
         * @return QString Documento JSON generado.
         */
        static QString stringify (QVariant v);

        /**
         * @brief Transforma una cadena de texto a una representación JSON válida.
         *  Las secuencias de escape las codifica en modo texto como: \n, \r,...
         *  Los caracteres no imprimibles, los saca con la codificación JSON: \uXXXX.
         *  La cadena resultante queda entrecomillada.
         *
         * @param in Cadena a transformar codificada en UTF-8.
         * @param bInQuotations A true si se deses entecomillar la respuesta.
         *
         * @return QString Cadena equivalente con formato JSON.
         */
        static QString stringify (QString in, bool bInQuotations = true );

        static QVariant parseFile(QString settingsFilePath, fn_parse_type fn_parse);

        /**
         * @brief Analiza un archivo de texto con el formato del SCV
         *  y devuelve un árbol de objetos Qt equivalente.
         *
         * @param settingsFilePath Ruta del archivo dentro del sistema de archivos.
         *
         * @return QVariant El árbol de objetos Qt que se ha detectado
         *  o QVariant() si no lo ha podido hacer.
         */
        static QVariant parseScvFile(QString settingsFilePath)
        {
            return parseFile(settingsFilePath, &parseScv);
        }

        /**
         * @brief Analiza un archivo de texto en formato JSON
         *  y devuelve un árbol de objetos Qt equivalente.
         *
         * @param settingsFilePath Ruta del archivo dentro del sistema de archivos.
         *
         * @return QVariant El árbol de objetos Qt que se ha detectado
         *  o QVariant() si no lo ha podido hacer.
         */
        static QVariant parseJsonFile(QString settingsFilePath)
        {
            return parseFile(settingsFilePath, &parseJson);
        }
    };
}

#endif // SAM_JSONUTIL_H
