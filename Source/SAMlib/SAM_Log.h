/** @file SAM_Log.h
*   @brief Funcionalidades para hacer log.
*   @date   15-10-2012 (dd-mm-yyyy)
*   @author David Jurado González
*   @version 0.1
*/

#ifndef SAM_LOG_H
#define SAM_LOG_H

#include "SAM_Logger.h"
#include "SAM_LoggerAppender.h"
#include "SAM_LoggerContainer.h"

#include <QString>

/** @def LOG(logger, logEvent, logLevel)
  * @brief Utilidad de Loggin.
  *
  * @a logger Nombre del módulo [AUDIO, RED, LOCAL, CONF, CLIENT, SERV, COMS, PARSNG, STATE].
  * + AUDIO.- Analizador de Audio Analógico
  * + RED.- Analizador de Red
  * + LOCAL.- Administración local
  * + CONF.- Configuración local
  * + CLIENT.- Comunicaciones cliente
  * + SERV.- Comunicaciones servidor
  * + COMS.- Comunicaciones de control interno
  * + PARSNG.- Análisis de tramas y conversiones de datos complejos
  * + STATE.- Máquina de estados
  *
  * @a logLevel Uno de: [TRACE, DEBUG, INFO, ALERT, WARN, ERROR, FATAL]
  */
#define LOG(logger, logEvent, logLevel) \
    QLOG_ ## logLevel(SAM::Log::instance().m_log ## logger) << logEvent

#define LOG_TRACE(logger) QLOG_TRACE(SAM::Log::instance().m_log ## logger)
#define LOG_DEBUG(logger) QLOG_DEBUG(SAM::Log::instance().m_log ## logger)
#define LOG_INFO( logger) QLOG_INFO( SAM::Log::instance().m_log ## logger)
#define LOG_ALERT(logger) QLOG_ALERT(SAM::Log::instance().m_log ## logger)
#define LOG_WARN( logger) QLOG_WARN( SAM::Log::instance().m_log ## logger)
#define LOG_ERROR(logger) QLOG_ERROR(SAM::Log::instance().m_log ## logger)
#define LOG_FATAL(logger) QLOG_FATAL(SAM::Log::instance().m_log ## logger)

#define LOG_TRACE_LN(logger) QLOG_TRACE_LN(SAM::Log::instance().m_log ## logger)
#define LOG_DEBUG_LN(logger) QLOG_DEBUG_LN(SAM::Log::instance().m_log ## logger)
#define LOG_INFO_LN( logger) QLOG_INFO_LN( SAM::Log::instance().m_log ## logger)
#define LOG_ALERT_LN(logger) QLOG_ALERT_LN(SAM::Log::instance().m_log ## logger)
#define LOG_WARN_LN( logger) QLOG_WARN_LN( SAM::Log::instance().m_log ## logger)
#define LOG_ERROR_LN(logger) QLOG_ERROR_LN(SAM::Log::instance().m_log ## logger)
#define LOG_FATAL_LN(logger) QLOG_FATAL_LN(SAM::Log::instance().m_log ## logger)

#define IF_LOG(logger, level) if( SAM::Log::instance().m_log ## logger.loggingLevel() > level )

#define TRACE(logger) QTRACE(SAM::Log::instance().m_log ## logger)

namespace SAM
{
    /**
     * @brief Encapsula la funcionalidad del log.
     */
    class Log : public LoggerContainer
    {
    public:
        Log();
        virtual ~Log();

        /**
         * @brief Implementa un singleton sencillo de la clase Log.
         *
         * @return Log
         */
        static Log & instance();

     /** @brief Establece el tamaño máximo del fichero de trazas.
      *  @param logFileMaxSize Tamaño máximo del fichero de trazas en Bytes.
      */
        void setMaxLogFileSize(long logFileMaxSize);

    /** @brief Establece la ruta donde almacenar los log.
      * @param logPath Ruta y nombre del archivo de log.
      */
        void setLogPath(const QString & logPath);

    public:
        Logger m_logAUDIO;  /**< AUDIO.- Analizador de Audio Analógico */
        Logger m_logRED;    /**< RED.- Analizador de Red */
        Logger m_logLOCAL;  /**< LOCAL.- Administración local */
        Logger m_logCONF;   /**< CONF.- Configuración local */
        Logger m_logCLIENT; /**< CLIENT.- Comunicaciones actuando como cliente */
        Logger m_logSERV;   /**< SERV.- Comunicaciones actuando como servidor */
        Logger m_logCOMS;   /**< COMS.- Comunicaciones de control interno */
        Logger m_logPARSNG; /**< PARSNG.- Análisis de tramas y conversiones de datos complejos */
        Logger m_logSTATE;  /**< STATE.- Máquina de estados */
        Logger m_logTELNET;  /**< TELNET.- Comunicaciones con Telnet */

        FileAppender m_fileAppender;
    };

    void logDebug(const char * str);
}

#endif // SAM_LOG_H
