#ifndef SAM_LOGGERCONTAINER_H
#define SAM_LOGGERCONTAINER_H

#include <QMap>
#include <QString>

#include "SAM_Logger.h"


namespace SAM
{
    /**
     * @brief Contenedor de instancias de SAM::Logger.
     */
    class LoggerContainer
    {
    public:
        LoggerContainer();
        virtual ~LoggerContainer();

        /**
         *  @brief Devuelve el logger cuyo nombre se pasa por parámetro.
         *   Si el módulo no existe, lo crea.
         *  @param module Nombre del módulo.
         */
        Logger getLogger(const QString & module);

        /**
         * @brief Devuelve los nombres de los loggers actualmente registrados.
         *
         * @return QList<QString>
         */
        QList<QString> getLoggerNames() const;

        /**
         * @brief Libera un logger.
         *
         * @param module Nombre del logger que se quiere borrar.
         */
        void releaseLogger(const QString & module);

        /**
         * @brief Elimina todos los logggers registrados.
         *
         */
        void releaseLoggers();

       /** @brief Establece el nivel de log para todos los módulos.
        *  @param level Nivel de log a establecer.
        */
        void setModuleLevel(Logger::Level level);

        /**
        * @brief Establece el formato de fecha/hora usado para todos los módulos.
        *
        * @param fmtDateTime
        */
        void setFmtDateTime(QString fmtDateTime);

        /**
        * @brief Establece el formato para todos los módulos.
        *
        * @param fmtLog
        */
        void setFmtLog(QString fmtLog);

        /**
         * @brief Añade un destino de mensajes de log para todos los módulos.
         *
         * @param appender
         */
        void addAppender(Appender appender);


    protected:
        QMap<QString, Logger> m_logModules;
    };
}

#endif // SAM_LOGGERCONTAINER_H
