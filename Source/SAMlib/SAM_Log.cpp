#include "SAM_Helpers.h"
#include "SAM_Logger.h"

#include "SAM_Log.h"

#define INITIAL_MAX_SIZE 5*1024*1024

namespace SAM
{
    Log::Log() :
        m_fileAppender(SAM_TARGET ".log")
    {
        // Iniciar los logger
        m_logAUDIO   = getLogger("AUDIO");
        m_logRED     = getLogger("RED");
        m_logLOCAL   = getLogger("LOCAL");
        m_logCONF    = getLogger("CONF");
        m_logCLIENT  = getLogger("CLIENT");
        m_logSERV    = getLogger("SERV");
        m_logCOMS    = getLogger("COMS");
        m_logPARSNG  = getLogger("PARSNG");
        m_logSTATE   = getLogger("STATE");
        m_logTELNET  = getLogger("TELNET");

        // Creamos los appenders.
        FileAppender fileAppender(m_fileAppender);
        DebugOutputAppender debugAppender;

        fileAppender.setMaxFileSize(INITIAL_MAX_SIZE);

        // Establecer el formato de fecha que se pondrá a los archivos viejos de log.
        fileAppender.setFmtDateTime("-(yyyy-MM-dd_hh.mm.ss)");

        foreach(Logger logger, m_logModules)
        {
            logger.setLoggingLevel(Logger::InfoLevel); // Todo menos trazas

            // Establecer el patrón de las fechas de las trazas
            logger.setFmtDateTime("dd/MM/yyyy hh:mm:ss.zzz");

            // Establecemos el patrón para las trazas.
            logger.setFmtLog("%3 [%1] %2: %4");

            logger.addAppender(fileAppender);
            logger.addAppender(debugAppender);
        }
    }

    Log::~Log()
    {
    }

    Log & Log::instance()
    {
        static Log log;
        return log;
    }

    void Log::setMaxLogFileSize(long logFileMaxSize)
    {
        m_fileAppender.setMaxFileSize(logFileMaxSize);
        LOG_INFO(LOCAL) <<  "Modificando el tamaño máximo del archivo de logs a "
            << logFileMaxSize;
    }


    void Log::setLogPath(const QString & logPath)
    {
        if(m_fileAppender.fileName() != logPath)
        {
            m_fileAppender.setFileName(logPath);
            LOG_INFO(LOCAL) <<  "Modificada la ruta del archivo de logs a" >> logPath << ".";
        }
    }

    void logDebug(const char *str)
    {
        LOG_DEBUG(LOCAL) << str;
    }

}
