/** @file SAM_Helpers.h
*   @brief Funciones auxiliares.
*   @date   15-10-2012 (dd-mm-yyyy)
*   @author David Jurado González
*   @version 0.1
*/

#ifndef SAM_HELPERS_H
#define SAM_HELPERS_H

#include <ostream>
#include <QTextStream>
#include <QString>
#include <QStringList>

#include "SAM_Debug.h"

class QIODevice;

namespace SAM
{
    // Flujos estándar al estilo Qt4.
    extern QTextStream cin;
    extern QTextStream cout;
    extern QTextStream cerr;

    /// Serializa cadenas Qt hacia std::ostream
    inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QString& str) {
        return os << qPrintable(str);
    }

    /// Función auxiliar para serializar QStringList hacia QTextStream
    QTextStream& operator<<(QTextStream& os, const QStringList& lst);

    /// Función auxiliar para serializar QVariant hacia QTextStream
    QTextStream& operator<<(QTextStream& os, const QVariant& var);

    /// Función auxiliar para serializar QStringList hacia SAM::Debug
    Debug& operator<<(Debug& os, const QStringList& lst);

    /// Función auxiliar para serializar QVariant hacia SAM::Debug
    Debug& operator<<(Debug& os, const QVariant& var);

    /// Función auxiliar para serializar const char * hacia QTextStream
    //QTextStream& operator<<(QTextStream& os, const char * sz);

    /**
     * @brief Borra Síncronamente todos los archivos del directorio
     *  `path` que sean anteriores a la fecha `timeThreshold` usando
     *  el filtro por nombres `wildcard`.
     *
     * @param path
     * @param timeThreshold
     * @param wildcard
     * @return int Devuelve el número de archivos borrados.
     */
    int deleteOldFiles(QString path,
                       const QDateTime &timeThreshold,
                       QStringList wildcard = QStringList("*"));

    /**
     * @brief Borra asíncronamente todos los archivos del directorio
     *  `path` que sean anteriores a la fecha `timeThreshold` usando
     *  el filtro por nombres `wildcard`.
     *
     * @param path
     * @param timeThreshold
     * @param wildcard
     */
    void deleteOldFilesAsync(QString path,
                             const QDateTime &timeThreshold,
                             QStringList wildcard = QStringList("*"));

    /**
     * @brief Hace una copia en bloques de 4kb de un Dispositivo de Entrada/Salida a otro.
     *
     * @param src Origen
     * @param dst Destino
     * @return quint64 Número de bytes copiados.
     */
    quint64 deviceCopy(QIODevice &src, QIODevice &dst);

    /**
     * @brief Hace una copia de un Dispositivo de Entrada/Salida a otro comprimiendo la información en gzip.
     *
     * @param src Origen
     * @param dst Destino
     * @return quint64 Número de bytes copiados.
     */
    quint64 deviceCopyCompress(QIODevice & src, QIODevice &dst);

    /**
     * @brief Comprime en gzip el archivo fileName y borra el archivo original.
     *  La función retorna inmediatamente porque realiza la compresión en un hilo trabajador.
     *  Es seguro seguir usando el nombre `fileName` para otros archivos, ya que
     * esta función renombra el archivo original durante la compresión.
     *
     * @param fileName Nombre del archivo que se pretende comprimir.
     */
    void compressFileAsync(QString fileName);


    /**
     * @brief Almacena el archivo fileName con un nuevo nombre y posiblemente
     *  nueva ubicación con una marca de fecha.
     *  Si el nombre de archivo `fileName` no tiene la extensión `.gz`
     *  lo comprime con gzip.
     *
     * @param fileName Nombre de un archivo que no esté en uso.
     * @param newName Base para el nombre del nuevo archivo. Si no se proporciona
     *  se toma como base fileName.
     * @param strStamp Estampa que irá en el nombre del nuevo archivo almacenado.
     *  Usar una marca temporal. Por defecto la estampa es "yyyy-MM-dd_hh.mm.ss".
     * @return bool
     */
    bool store(QString fileName, QString newName = QString(), QString strStamp = QString());

    extern QString g_dateTimeFormat;

    char * mini_strcpy(char * p, const char * q);

//    template<typename E>
//    const char * getEnumName(E en)
//    {

//    }

    /**
     * @brief Ofrece un sencillo mecanismo para transformar a un objeto JSON cualquier
     *  objeto Qt que tenga propiedades definidas mediante `Q_PROPERTY`.
     *  También busca recursivamente en todos los objetos hijos de éste.
     *
     * @param t
     * @return QVariantMap
     */
    QVariantMap toVariant(const QObject * t);
}

#endif // SAM_HELPERS_H
