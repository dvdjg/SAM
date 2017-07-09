#include "SAM_Logger.h"
#include "SAM_LoggerAppender.h"

#include <QMutex>
#include <QList>
#include <QDateTime>
#include <cassert>

namespace SAM
{
    typedef QList<Appender> AppenderList;

    static const char TraceString[] = "TRACE";
    static const char DebugString[] = "DEBUG";
    static const char InfoString[]  = "INFO";
    static const char AlertString[] = "ALERT";
    static const char WarnString[]  = "WARN";
    static const char ErrorString[] = "ERROR";
    static const char FatalString[] = "FATAL";
    static const char NoneString[]  = "NONE";
    static const char OffString[]  = "OFF";

    static const char *s_levelToText[] = {
        TraceString,
        DebugString,
        InfoString,
        AlertString,
        WarnString,
        ErrorString,
        FatalString,
        NoneString,
        0
    };

    const char* Logger::levelToText(Level theLevel)
    {
        int level = 7 & (int) theLevel;

        return s_levelToText[level];
    }

    Logger::Level Logger::textToLevel(const QString & strLevel)
    {
        Logger::Level level = Logger::InvalidLevel;

        if(strLevel == OffString)
            level = Logger::FatalLevel; // El nivel fatal no se puede desactivar
        else if(strLevel == FatalString)
            level = Logger::FatalLevel;
        else if(strLevel == ErrorString)
            level = Logger::ErrorLevel;
        else if(strLevel == WarnString)
            level = Logger::WarnLevel;
        else if(strLevel == AlertString)
            level = Logger::AlertLevel;
        else if(strLevel == InfoString)
            level = Logger::InfoLevel;
        else if(strLevel == DebugString)
            level = Logger::DebugLevel;
        else if(strLevel == TraceString)
            level = Logger::TraceLevel;

        return level;
    }

    /**
     * @brief Implementación privada de SAM::Logger.
     */
    class LoggerPrivate : public QSharedData
    {
    public:

        LoggerPrivate() :
            m_level(Logger::InfoLevel),
            // not using g_dateTimeFormat because we need the milliseconds too
            m_fmtLog("%1 %2 %3 %4"),
            m_fmtDateTime("yyyy-MM-ddThh:mm:ss.zzz")
        {
        }

        mutable QMutex m_logMutex;
        Logger::Level m_level;
        AppenderList m_destList;
        QString m_fmtLog;
        QString m_fmtDateTime;
        QString m_name;
    };

    Logger::Logger() :
        m_ptr(new LoggerPrivate)
    {
    }

    Logger::Logger(const Logger & o) :
        m_ptr(o.m_ptr)
    {
    }

    Logger & Logger::operator = (const Logger & o)
    {
        m_ptr = o.m_ptr;
        return *this;
    }

    Logger::~Logger()
    {
    }


    void Logger::addAppender(Appender appender)
    {
        J_D(Logger);

        QMutexLocker lock(&d->m_logMutex);

        if(d->m_destList.indexOf(appender) < 0)
            d->m_destList.push_back(appender);
    }


    void Logger::removeAppender(Appender appender)
    {
        J_D(Logger);

        QMutexLocker lock(&d->m_logMutex);
        d->m_destList.removeAll(appender);
    }

    void Logger::setLoggingLevel(Level newLevel)
    {
        J_D(Logger);

        d->m_level = newLevel;
    }

    Logger::Level Logger::loggingLevel() const
    {
        J_D(const Logger);

        return d->m_level;
    }

    QString Logger::fmtDateTime() const
    {
        J_D(const Logger);

        return d->m_fmtDateTime;
    }

    void Logger::setFmtDateTime(QString fmtDateTime)
    {
        J_D(Logger);

        d->m_fmtDateTime = fmtDateTime;
    }

    QString Logger::fmtLog() const
    {
        J_D(const Logger);

        return d->m_fmtLog;
    }


    void Logger::setFmtLog(QString fmtLog)
    {
        J_D(Logger);

        d->m_fmtLog = fmtLog;
    }


    QString Logger::name() const
    {
        J_D(const Logger);

        return d->m_name;
    }

    void Logger::setName(QString name)
    {
        J_D(Logger);

        d->m_name = name;
    }

    Logger::Helper::Helper(Logger::Level logLevel, Logger &logger) :
        m_level(logLevel),
        m_qtDebug(&m_buffer),
        m_logger(logger)
    {
    }

    //! creates the complete log message and passes it to the logger
    void Logger::Helper::writeToLog()
    {
        const LoggerPrivate *d = m_logger.d_func();

        const char* const levelName = Logger::levelToText(m_level);
        QString dateTime = QDateTime::currentDateTime().toString(d->m_fmtDateTime);

        const QString completeMessage(QString(d->m_fmtLog)
                                      .arg(d->m_name, 6)
                                      .arg(levelName, 5)
                                      .arg(dateTime)
                                      .arg(m_buffer)
                                      );

        QMutexLocker lock(&d->m_logMutex);
        m_logger.write(completeMessage);
    }

    /*!
     \brief

    */
    Logger::Helper::~Helper()
    {
        writeToLog();
    }

    Logger::TraceHelper::TraceHelper(Logger::Level level, Logger & logger, const char * szFunction, const char * szFile, int line)
        : m_level(level), m_logger(logger), m_szFunction(szFunction)
    {
        QLOG_LOGGER_LEVEL(m_logger, m_level) << "{ ENTER"<< szFile << '@' << line << ':' << m_szFunction;
    }

    Logger::TraceHelper::~TraceHelper()
    {
        QLOG_LOGGER_LEVEL(m_logger, m_level) << "} EXIT " << m_szFunction;
    }

    //! sends the message to all the destinations
    /*!
     \brief

     \param message
    */
    void Logger::write(const QString& message)
    {
        J_D(Logger);

        for(AppenderList::iterator it = d->m_destList.begin(),
            endIt = d->m_destList.end();it != endIt;++it)
        {
            it->write(message);
        }
    }

} // end namespace
