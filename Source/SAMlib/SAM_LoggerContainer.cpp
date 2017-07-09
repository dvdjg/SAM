#include "SAM_LoggerContainer.h"

namespace SAM
{

    LoggerContainer::LoggerContainer()
    {
    }

    LoggerContainer::~LoggerContainer()
    {
    }

    Logger LoggerContainer::getLogger(const QString & module)
    {
        Logger logger(m_logModules[module]);

        if(logger.name().isEmpty())
            logger.setName(module);

        return logger;
    }

    QList<QString> LoggerContainer::getLoggerNames() const
    {
        return m_logModules.keys();
    }

    void LoggerContainer::releaseLogger(const QString & module)
    {
        m_logModules.remove(module);
    }

    void LoggerContainer::releaseLoggers()
    {
        m_logModules.clear();
    }


//    void LoggerContainer::setModuleLevel(Logger::Level level, const QString & module)
//    {
//        if(m_logModules.contains(module))
//        {
//            m_logModules[module].setLoggingLevel(level);
//        }
//        else
//        {
//            Debug(QtDebugMsg) << "Módulo" << module << "desconocido al cambiar nivel de trazas.";
//            return;
//        }
//    }

    void LoggerContainer::setModuleLevel(Logger::Level level)
    {
        foreach(Logger logger, m_logModules) {
            logger.setLoggingLevel(level);
        }
    }

    void LoggerContainer::setFmtDateTime(QString fmtDateTime)
    {
        foreach(Logger logger, m_logModules) {
            logger.setFmtDateTime(fmtDateTime);
        }
    }

    void LoggerContainer::setFmtLog(QString fmtLog)
    {
        foreach(Logger logger, m_logModules) {
            logger.setFmtLog(fmtLog);
        }
    }

    void LoggerContainer::addAppender(Appender appender)
    {
        foreach(Logger logger, m_logModules) {
            logger.addAppender(appender);
        }
    }


}
