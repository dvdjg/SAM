/** @file SAM_JVariant.h
*   @brief Tratamiento del formato JSON.
*   @date   15-10-2012 (dd-mm-yyyy)
*   @author David Jurado González
*   @version 0.1
*/

#ifndef SAM_JVariant_H
#define SAM_JVariant_H


#include <QVariant>
#include <QString>

class QTextStream;
class QStringList;

namespace SAM
{

    /**
     * @brief Clase para permitir una notación compacta en el acceso a objetos JSON.
     *  Es un _wrapper_ de `QVariant` con algunos operadores sobrecargados para
     *  mejorar la comodidad de desarrollo: Operadores de conversión a los tipos más
     *  habituales y operador corchetes de acceso a colecciones de datos.
     */
    class JVariant : public QVariant
    {
    public:
        inline JVariant() {}
        JVariant(const QVariant &other) : QVariant(other) {}

        ~JVariant() {}

        // Operadores de conversión r-value a los tipos básicos.
        operator int        () const { return toInt(); }
        operator long int   () const { return toInt(); }
        operator uint       () const { return toUInt(); }
        operator qint16     () const { return (qint16)  toInt(); }
        operator quint16    () const { return (quint16) toUInt(); }
        operator qint8      () const { return (qint8)   toInt(); }
        //operator char       () const { return (char)    toInt(); } // Problematic
        operator quint8     () const { return (quint8)  toUInt(); }
        operator qint64     () const { return toLongLong(); }
        operator quint64    () const { return toULongLong(); }
        operator double     () const { return toDouble(); }
        operator float      () const { return toFloat(); }
        //operator qreal      () const { return toReal(); } // Problematic
        operator QString    () const { return toString(); }
        operator QDateTime  () const;

        /**
         * @brief Conversión a `bool`.
         *  Se considera que un objeto SAM::JVariant es falso si es nulo o
         *  toBool() devuelve `false`.
         *  En caso contrario se considera verdadero.
         *
         * @return operator
         */
        operator bool       () const
        {
            return canConvert(QVariant::Bool) ? toBool() : !isNull();
        }

        // Operadores de conversión r-value a los tipos complejos.
        operator QList<QVariant> () const { return toList(); }
        operator QMap<QString, QVariant> () const { return toMap(); }

        /**
         * @brief Devuelve el tamaño del contenedor de datos interno o 0
         *  si no es una colección admitida.
         *
         * @return int
         */
        int size() const;

        /**
         * @brief Devuelve el tamaño del contenedor de datos interno o 0
         *  si no es una colección admitida.
         *
         * @return int
         */
        int length() const { return size(); }

        /**
         * @brief Mezcla los dos objetos recursivamente.
         *  Si hay miembros que se llaman igual, prevalecen los de `other`.
         *
         * @param other
         * @return JVariant
         */
        JVariant & merge(const JVariant & other);

        /**
         * @brief Mezcla los dos objetos recursivamente.
         *  Si hay miembros que se llaman igual, prevalecen los de `other`.
         *
         * @param other
         * @return JVariant &operator
         */
        JVariant & operator |= (const JVariant & other) { return merge(other); }

        // Conversión l-value a los tipos simples.
        /**
         * @brief Devuelve una referencia a un int. Si el objeto no lo fuera,
         *  lo transforma.
         *
         * @return int &
         */
        int & asInt();

        /**
         * @brief Devuelve una referencia a un uint. Si el objeto no lo fuera,
         *  lo transforma.
         *
         * @return uint &
         */
        uint & asUInt();

        /**
         * @brief Devuelve una referencia a un qlonglong. Si el objeto no lo fuera,
         *  lo transforma.
         *
         * @return qlonglong &
         */
        qlonglong & asLongLong();

        /**
         * @brief Devuelve una referencia a un qulonglong. Si el objeto no lo fuera,
         *  lo transforma.
         *
         * @return qulonglong &
         */
        qulonglong & asULongLong();

        /**
         * @brief Devuelve una referencia a un double. Si el objeto no lo fuera,
         *  lo transforma.
         *
         * @return double &
         */
        double & asDouble();

        /**
         * @brief Devuelve una referencia a un QString. Si el objeto no lo fuera,
         *  lo transforma.
         *
         * @return QString &
         */
        QString & asString();

        /**
         * @brief Devuelve una referencia a un QByteArray. Si el objeto no lo fuera,
         *  lo transforma.
         *
         * @return QByteArray &
         */
        QByteArray & asByteArray();

//        /**
//         * @brief Devuelve una referencia a un int. Si el objeto no lo fuera, lo transforma.
//         *
//         * @return int &
//         */
//        operator int & () { return asInt(); }

//        /**
//         * @brief Devuelve una referencia a un qlonglong. Si el objeto no lo fuera, lo transforma.
//         *
//         * @return qlonglong &
//         */
//        operator qlonglong & () { return aslonglong(); }

//        /**
//         * @brief Devuelve una referencia a un double. Si el objeto no lo fuera, lo transforma.
//         *
//         * @return double &
//         */
//        operator double & () { return asDouble(); }

        /**
         * @brief Devuelve una referencia a un QString. Si el objeto no lo fuera, lo transforma.
         *
         * @return QString &
         */
        operator QString & () { return asString(); }

        // Operadores de conversión l-value a los tipos complejos.
        /**
         * @brief Devuelve una referencia a un List. Si el objeto no lo fuera,
         *  lo transforma.
         *
         * @return QList<QVariant> &
         */
        QList<QVariant> & asList();

        /**
         * @brief Devuelve una referencia a un StringList. Si el objeto no lo fuera,
         *  lo transforma.
         *
         * @return QStringList &
         */
        QStringList & asStringList();

        /**
         * @brief Devuelve una referencia a un Map. Si el objeto no lo fuera,
         *  lo transforma.
         *
         * @return QMap<QString, QVariant> &
         */
        QMap<QString, QVariant> & asMap();

#if QT_VERSION >= 0x040800
        /**
         * @brief Devuelve una referencia a un HashMap. Si el objeto no lo fuera,
         *  lo transforma.
         *
         * @return QMap<QString, QVariant> &
         */
        QHash<QString, QVariant> & asHash();
#endif // QT_VERSION >= 0x040800

        /**
         * @brief Devuelve una referencia a un List. Si el objeto no lo fuera, lo transforma.
         *
         * @return QList<QVariant> &
         */
        operator QList<QVariant> & () { return asList(); }

        /**
         * @brief Devuelve una referencia a un StringList. Si el objeto no lo fuera, lo transforma.
         *
         * @return QStringList &
         */
        operator QStringList & () { return asStringList(); }

        /**
         * @brief Devuelve una referencia a un Map. Si el objeto no lo fuera, lo transforma.
         *
         * @return QMap<QString, QVariant> &
         */

        operator QMap<QString, QVariant> & () { return asMap(); }

        /**
         * @brief Elimina el elemento con índice `i` del array JSON correspondiente a éste nodo.
         *  Es necesario que el nodo sea de tipo JSON ARRAY, representado internamente
         *  mediante un `QList<QVariant>`.
         *
         * @param i Índice del elemento a borrar.
         *
         * @return int Número de objetos que se han borrado: 0 ó 1.
         */
        int remove(int i);

        /**
         * @brief Elimina el elemento con `s` del objeto JSON correspondiente a éste nodo.
         *  Es necesario que el nodo sea de tipo JSON OBJECT, representado internamente
         *  mediante un `QMap<QString, QVariant>`.
         *  En caso de que sea un nodo de tipo JSON ARRAY, borrará todos los elementos
         *  que contengan la cadena `s`.
         *
         * @param s Nombre del elemento a borrar.
         *
         * @return int Número de objetos que se han borrado.
         */
        int remove(const QString & s);

        /**
         * @brief Acceso al elemento `i` del array.
         *  Es necesario que el nodo sea de tipo JSON ARRAY, representado internamente
         *  mediante un `QList<QVariant>`.
         *
         * @param i Índice del elemento a devolver.
         *
         * @return JVariant `QVariant` correspondiente a ese elemento en el array.
         */
        JVariant at(int i) const;

        /**
         * @brief Acceso al elemento `i` del array.
         *  Es necesario que el nodo sea de tipo JSON ARRAY, representado internamente
         *  mediante un `QList<QVariant>`.
         *
         * @param i Índice del elemento a devolver.
         *
         * @return JVariant `QVariant` correspondiente a ese elemento en el array.
         */
        JVariant operator[](int i) const;

        /**
         * @brief Acceso al elemento `s` del objeto.
         *  Es necesario que el nodo sea de tipo JSON OBJECT, representado internamente
         *  mediante un `QMap<QString, QVariant>`.
         *
         * @param s Nombre del elemento a devolver.
         *
         * @return JVariant `QVariant` correspondiente a ese elemento en el objeto.
         */
        JVariant at(const JVariant & s) const { return at(s.toString()); }
        JVariant operator[](const JVariant & s) const { return at(s); }

        JVariant & at(const JVariant & s) { return at(s.toString()); }
        JVariant & operator[](const JVariant & s) { return at(s); }

        /**
         * @brief Acceso al elemento `s` del objeto.
         *  Es necesario que el nodo sea de tipo JSON OBJECT, representado internamente
         *  mediante un `QMap<QString, QVariant>`.
         *
         * @param s Nombre del elemento a devolver.
         *
         * @return JVariant `QVariant` correspondiente a ese elemento en el objeto.
         */
        JVariant at(const QString & s) const;

        /**
         * @brief Acceso al elemento `s` del objeto.
         *  Es necesario que el nodo sea de tipo JSON OBJECT, representado internamente
         *  mediante un `QMap<QString, QVariant>`.
         *
         * @param s Nombre del elemento a devolver.
         *
         * @return JVariant `QVariant` correspondiente a ese elemento en el objeto.
         */
        JVariant operator[](const QString & s) const;

        /**
         * @brief Acceso al elemento `sz` del objeto.
         *  Es necesario que el nodo sea de tipo JSON OBJECT, representado internamente
         *  mediante un `QMap<QString, QVariant>`.
         *
         * @param sz Nombre del elemento a devolver.
         *
         * @return JVariant `QVariant` correspondiente a ese elemento en el objeto.
         */
        JVariant at(const char * sz) const;

        /**
         * @brief Acceso al elemento `sz` del objeto.
         *  Es necesario que el nodo sea de tipo JSON OBJECT, representado internamente
         *  mediante un `QMap<QString, QVariant>`.
         *
         * @param sz Nombre del elemento a devolver.
         *
         * @return JVariant `QVariant` correspondiente a ese elemento en el objeto.
         */
        JVariant operator[](const char * sz) const;
        JVariant operator[](char * sz) const { return at(sz); }

        /**
         * @brief Acceso al elemento `i` del array como l-value.
         *  Es necesario que el nodo sea de tipo JSON ARRAY, representado internamente
         *  mediante un `QList<QVariant>`.
         *
         * @param i Índice del elemento a devolver.
         *
         * @return JVariant Referencia al `QVariant` correspondiente a ese elemento en el array.
         */
        JVariant & at(int i);

        /**
         * @brief Acceso al elemento `i` del array como l-value.
         *  Es necesario que el nodo sea de tipo JSON ARRAY, representado internamente
         *  mediante un `QList<QVariant>`.
         *
         * @param i Índice del elemento a devolver.
         *
         * @return JVariant Referencia al `QVariant` correspondiente a ese elemento en el array.
         */
        JVariant & operator[](int i);

        /**
         * @brief Devuelve `true` si el objeto es una colección que contiene un elemento
         *   con una clave `i`;
         *
         * @param i
         * @return bool
         */
        bool contains(int i) const;
        bool contains(uint i) const { return contains((int) i); }

        /**
         * @brief Devuelve `true` si el objeto es una colección que contiene un elemento
         *   con una clave `s`;
         *
         * @param s
         * @return bool
         */
        bool contains(const QString & s) const;

        /**
         * @brief Convierte el objeto en una Lista si no lo fuera ya y devuelve el primer elemento. Si
         *  la lista no tuviera elementos, crea uno vacío.
         *
         * @return JVariant
         */
        JVariant & first();
        JVariant first() const;

        /**
         * @brief Convierte el objeto en una Lista si no lo fuera ya y devuelve el último elemento. Si
         *  la lista no tuviera elementos, crea uno vacío.
         *
         * @return JVariant
         */
        JVariant & last();
        JVariant last() const;

        /**
         * @brief Acceso al elemento `s` del objeto como l-value.
         *  Es necesario que el nodo sea de tipo JSON OBJECT, representado internamente
         *  mediante un `QMap<QString, QVariant>` o `QHash<QString, QVariant>`.
         *
         * @param s Nombre del elemento a devolver.
         *
         * @return JVariant Referencia al `QVariant` correspondiente a ese elemento en el objeto.
         */
        JVariant & at(const QString & s);

        /**
         * @brief Acceso al elemento `s` del objeto como l-value.
         *  Es necesario que el nodo sea de tipo JSON OBJECT, representado internamente
         *  mediante un `QMap<QString, QVariant>` o `QHash<QString, QVariant>`.
         *
         * @param s Nombre del elemento a devolver.
         *
         * @return JVariant Referencia al `QVariant` correspondiente a ese elemento en el objeto.
         */
        JVariant & operator[](const QString & s);

        /**
         * @brief Acceso al elemento `sz` del objeto como l-value.
         *  Es necesario que el nodo sea de tipo JSON OBJECT, representado internamente
         *  mediante un `QMap<QString, QVariant>` o `QHash<QString, QVariant>`.
         *
         * @param sz Nombre del elemento a devolver.
         *
         * @return JVariant Referencia al `QVariant` correspondiente a ese elemento en el objeto.
         */
        JVariant & at(const char * sz);

        /**
         * @brief Acceso como l-value al elemento `sz`.
         *  Es necesario que el nodo sea de tipo JSON OBJECT, representado internamente
         *  mediante un `QMap<QString, QVariant>` o `QHash<QString, QVariant>`.
         *
         * @param sz Nombre del elemento a devolver.
         *
         * @return JVariant Referencia al `QVariant` correspondiente a ese elemento en el objeto.
         */
        JVariant & operator[](const char * sz);
        JVariant & operator[](char * sz) { return at(sz); }

        /**
         * @brief Acceso como l-value al elemento por la ruta `path`.
         *  Es necesario que el nodo sea de tipo JSON OBJECT, representado internamente
         *  mediante un `QMap<QString, QVariant>` o `QHash<QString, QVariant>`.
         *
         * @param sz Nombre del elemento a devolver.
         *
         * @return JVariant Referencia al `QVariant` correspondiente a ese elemento en el objeto.
         */
        JVariant & at(const QStringList & path);

        /**
         * @brief Acceso como l-value al elemento por la ruta `path`.
         *  Es necesario que el nodo sea de tipo JSON OBJECT, representado internamente
         *  mediante un `QMap<QString, QVariant>` o `QHash<QString, QVariant>`.
         *
         * @param sz Nombre del elemento a devolver.
         *
         * @return JVariant Referencia al `QVariant` correspondiente a ese elemento en el objeto.
         */
        JVariant & operator[] (const QStringList & path);

        /**
         * @brief Acceso constante como r-value al elemento por la ruta `path`.
         *  Es necesario que el nodo sea de tipo JSON OBJECT, representado internamente
         *  mediante un `QMap<QString, QVariant>` o `QHash<QString, QVariant>`.
         *
         * @param sz Nombre del elemento a devolver.
         *
         * @return JVariant Referencia al `QVariant` correspondiente a ese elemento en el objeto.
         */
        JVariant at(const QStringList & path) const;

        /**
         * @brief Acceso constante como r-value al elemento por la ruta `path`.
         *  Es necesario que el nodo sea de tipo JSON OBJECT, representado internamente
         *  mediante un `QMap<QString, QVariant>` o `QHash<QString, QVariant>`.
         *
         * @param sz Nombre del elemento a devolver.
         *
         * @return JVariant Referencia al `QVariant` correspondiente a ese elemento en el objeto.
         */
        JVariant operator[] (const QStringList & path) const;

        /**
         * @brief Operador de asignación.
         * @param v
         * @return JVariant & devuelve una referencia a sí mismo.
         */
        JVariant & operator=(const QVariant & v) { setValue(v); return *this; }

        template<typename T>
        JVariant & operator=(const QList<T> & v)
        {
            QList<QVariant> lst;

            QListIterator<T> it(v);

            while(it.hasNext()) {
                lst.append(it.next().toString());
            }

            setValue(lst);

            return *this;
        }

        JVariant & operator=(const QStringList & v);
        JVariant & operator=(const QList<QVariant> & v);

//        /**
//         * @brief Operador de asignación.
//         * @param v
//         * @return JVariant & devuelve una referencia a sí mismo.
//         */
//        JVariant & operator=(const QStringRef & v);

        /**
         * @brief Si el valor contenido es de tipo numérico con signo,
         *  devuelve una copia con ese valor negado. En caso contrario
         *  devuelve el mismo valor que tenía.
         *
         * @return JVariant
         */
        JVariant operator-() const;

        /**
         * @brief Devuelve una representación en forma de texto JSON del árbol de objetos
         *  que cuelgan desde éste nodo.
         *
         * @return QString Texto con el árbol en JSON.
         */
        QString toJson() const;

        /**
         * @brief Analiza el texto JSON y genera un árbol de objetos Qt que lo represente.
         *
         * @param s Flujo de texto de entrada que se va a analizar.
         *  Acepta también la dirección de un QString o de un QByteArray.
         *
         * @return JVariant El árbol de objetos Qt que se ha detectado
         *  o QVariant() si no lo ha podido hacer.
         */
        JVariant & parse(QTextStream & s);

        /**
         * @brief Analiza el texto JSON y genera un árbol de objetos Qt que lo represente.
         *
         * @param str Cadena de texto de entrada que se va a analizar.
         *
         * @return JVariant El árbol de objetos Qt que se ha detectado
         *  o QVariant() si no lo ha podido hacer.
         */
        JVariant & parse(QString & s);

        /**
         * @brief Analiza un archivo de texto en formato JSON
         *  y devuelve un árbol de objetos Qt equivalente.
         *
         * @param fileName Ruta dentro del sistema de archivos.
         *
         * @return JVariant El árbol de objetos Qt que se ha detectado
         *  o QVariant() si no lo ha podido hacer.
         */
        JVariant & parseFile(QString & fileName);

        /**
         * @brief Operador de comparación que convierte el argumento a QVariant
         *  para realizar la comparación.
         *
         * @param v
         * @return bool operator
         */
        template<typename T>
        bool operator==(const T & v)
        {
            return cmp(v);
        }

        /**
         * @brief Operador de comparación especializado para cadenas de caracteres.
         *
         * @param v
         * @return bool operator
         */
        bool operator==(const char * v)
        {
            return toString() == v;
        }

        template<typename T>
        bool operator!=(const T & v)
        {
            return !(*this == v);
        }

    protected:
        /**
         * @brief Obtiene el objeto envuelto por el `QVariant`.
         *  Da una funcionalidad similara a la que se tenía con Qt3 en el acceso a los
         *  objetos contenidos en un `QVariant`.
         * @note No hace comprobaciones en tiempo de ejecución.
         * @return T
         */
        template<typename T>
        T& as()
        {
            T* ptr = static_cast<T*>(data());

            return *ptr;
        }

    };


}

#endif // SAM_JVariant_H
