
#ifndef SAM_LOGGER_H
#define SAM_LOGGER_H

#include <QString>
#include <QSharedData>
#include "SAM_ExplicitlySharedDataPointer.h"
#include "SAM_Debug.h"
#include "SAM_LoggerAppender.h"

#define QLOG_LOGGER_LEVEL(logger, level) \
    if( logger.loggingLevel() > level ){} \
    else SAM::Logger::Helper(level, logger).stream()

#define QLOG_LOGGER_LEVEL_LN(logger, level) \
    QLOG_LOGGER_LEVEL(logger, level) LOCATION_STREAM

#define QLOG_TRACE(logger) QLOG_LOGGER_LEVEL(logger, SAM::Logger::TraceLevel)
#define QLOG_DEBUG(logger) QLOG_LOGGER_LEVEL(logger, SAM::Logger::DebugLevel)
#define QLOG_INFO(logger)  QLOG_LOGGER_LEVEL(logger, SAM::Logger::InfoLevel)
#define QLOG_ALERT(logger) QLOG_LOGGER_LEVEL(logger, SAM::Logger::AlertLevel)
#define QLOG_WARN(logger)  QLOG_LOGGER_LEVEL(logger, SAM::Logger::WarnLevel)
#define QLOG_ERROR(logger) QLOG_LOGGER_LEVEL(logger, SAM::Logger::ErrorLevel)
#define QLOG_FATAL(logger) SAM::Logger::Helper(SAM::Logger::FatalLevel, logger).stream()

#define QLOG_TRACE_LN(logger) QLOG_LOGGER_LEVEL_LN(logger, SAM::Logger::TraceLevel)
#define QLOG_DEBUG_LN(logger) QLOG_LOGGER_LEVEL_LN(logger, SAM::Logger::DebugLevel)
#define QLOG_INFO_LN(logger)  QLOG_LOGGER_LEVEL_LN(logger, SAM::Logger::InfoLevel)
#define QLOG_ALERT_LN(logger) QLOG_LOGGER_LEVEL_LN(logger, SAM::Logger::AlertLevel)
#define QLOG_WARN_LN(logger)  QLOG_LOGGER_LEVEL_LN(logger, SAM::Logger::WarnLevel)
#define QLOG_ERROR_LN(logger) QLOG_LOGGER_LEVEL_LN(logger, SAM::Logger::ErrorLevel)
#define QLOG_FATAL_LN(logger) SAM::Logger::Helper(SAM::Logger::FatalLevel, logger).stream()\
    LOCATION_STREAM

#ifdef NO_QTRACE_LEVEL
#define QTRACE_LEVEL(logger, level)
#else
#define QTRACE_LEVEL(logger, level) \
    SAM::Logger::TraceHelper _tracer##__LINE__(level, logger, Q_FUNC_INFO, SAM::thisFile.base, __LINE__)
#endif

#define QTRACE(logger) QTRACE_LEVEL(logger, SAM::Logger::TraceLevel)

namespace SAM
{

    class LoggerPrivate;
    /**
     * @brief Módulo para hacer log.
     *  A continuación se muestra un programa que hace uso de las facilidades de log.
     *  En éste ejemplo se usa una única instancia del SAM::Logger obtenida mediante
     *   Logger::instance(). En una aplicación que se requieran más se pueden crear
     *   tantos como se necesiten.
     * @code
      #include "SAM_Logger.h"
      #include "SAM_LoggerAppender.h"
      #include <QtCore/QCoreApplication>
      #include <QTextCodec>
      #include <QString>
      #include <iostream>

      using namespace SAM_Log;

      // Programa de ejemplo de la librería de log hecha para el proyecto SAM
      int main(int argc, char *argv[])
      {
          for(int i = 0; i < 100000000; i++);

          QCoreApplication a(argc, argv);

          // El código fuente está escrito en UTF-8, aquí se lo decimos a Qt.
          QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

          QTextStream cout(stdout); // Stream hacia la salida estándar

          // Inicia el mecanismo de loggin
          Logger& logger = Logger::instance();
          logger.setLoggingLevel(Logger::TraceLevel);
          logger.setName("LOGGER");
          logger.setFmtDateTime("dd/MM/yyyy hh:mm:ss.zzz");
          logger.setFmtLog("%3 [%1] %2: %4");

          FileAppender fd("log.txt");
          DebugOutputAppender dd;
          StreamOutputAppender so(cout);
          logger.addAppender(fd);
          logger.addAppender(dd);
          logger.addAppender(so);

          // Crear un nuevo archivo de log cuando se superen los 1000000 bytes.
          fd.setMaxFileSize(1000000);

          // Establecer el formato de fecha que se pondrá a los archivos viejos de log.
          fd.setFmtDateTime("-(yyyy-MM-dd hh.mm.ss)");

          // En la primera línea imprime unos caracteres con codificación especial UTF8
          // De esta forma los procesadores de texto inteligentes elegirán la codificación adecuada.
          QLOG_INFO(logger) << "«Comienzo del programa»";
          QLOG_INFO(logger) << "Construido con Qt" << QT_VERSION_STR << "corriendo en" << qVersion();

          QLOG_TRACE(logger) << "Un mensaje de" << QString("trace") << ".";
          QLOG_DEBUG(logger) << "Un mensaje de" << static_cast<int>(Logger::DebugLevel) << ".";

          fd.setFileName("log/log.txt"); // Mover el log a otro directorio

          QLOG_WARN(logger)  << "¡Ja-ja!";
          qDebug() << "Éste mensaje no será tratado por el logger";
          QLOG_ERROR(logger) << "Ha ocurrido un error";
          qWarning() << "Tampoco será tratado éste";
          QLOG_FATAL(logger) << "¡Error fatal!";

          const int ret = 0;

          // Salida estándar usando la bibloteca de C++
          std::cout << std::endl << "Presione <RETURN>...";
          std::cin.get();
          QLOG_INFO(logger) << "Programa terminado con código de retorno" << ret;

          // Salida estándar usando la bibloteca de Qt
          cout << endl << "Hasta pronto." << endl;

          return ret;
      }

     * @endcode
     */
    class Logger
    {
        Q_GADGET

        Q_ENUMS(Level)
        J_DECLARE_PRIVATE(Logger)

    public:
        /**
         * @brief Niveles de log permitidos.
         *
         */
        enum Level
        {
            TraceLevel,
            DebugLevel,
            InfoLevel,
            AlertLevel,
            WarnLevel,
            ErrorLevel,
            FatalLevel,
            InvalidLevel
        };

        /**
         * @brief Devuelve una representación en forma de texto del nivel de log
         *  que se pasa por parámetro.
         *
         * @param theLevel
         * @return const char
         */
        static const char* levelToText(Level theLevel);

        /** @brief Devuelve el Logger::Level correspondiente al nivel de log
         *  que se pasa por parámetro.
         *
         * @param level Nivel de log.
         */
        static Level textToLevel(const QString & level);

    public:
        /**
         * @brief Usado para obtener un ejemplar único del logger.
         *  En las aplicaciones en las que existan varios logger no es de utilidad
         *  ya que debería existir un mecanismo adicional para administrarlos.
         *
         * @return Logger
         */
        static Logger& instance()
        {
            static Logger staticLog;
            return staticLog;
        }

        Logger();
        Logger(const Logger & o);
        ~Logger();

        Logger & operator = (const Logger & o);

        /**
         * @brief Añade un destino de mensajes de log.
         *
         * @param destination
         */
        void addAppender(Appender appender);

        /**
         * @brief Borra un destino de mensajes de log.
         *
         * @param destination
         */
        void removeAppender(Appender appender);

        /**
         * @brief Establece el nivel mínimo para que se impriman mensajes de login.
         *
         * @param newLevel
         */
        void setLoggingLevel(Level newLevel);

        //! The default level is INFO
        Level loggingLevel() const;

        /**
        * @brief Devuelve el formato de fecha/hora usado para éste logger.
        *
        * @return QString
        */
        QString fmtDateTime() const;

        /**
        * @brief Establece el formato de fecha/hora usado para éste logger.
        *
        * @param fmtDateTime
        */
        void setFmtDateTime(QString fmtDateTime);

        /**
        * @brief Devuelve el formato usado para éste logger.
        *
        * @return QString
        */
        QString fmtLog() const;

        /**
        * @brief Establece el formato para éste logger.
        *
        * @param fmtLog
        */
        void setFmtLog(QString fmtLog);

        /**
        * @brief Devuelve el nombre del logger.
        *
        * @return QString
        */
        QString name() const;

        /**
        * @brief Establece el nombre del logger.
        *
        * @param name
        */
        void setName(QString name);

        /**
         * @brief Lleva el flujo hacia SAM::Debug y construye el mensaje log final.
         */
        class Helper
        {
            void writeToLog();

            Level m_level; /**< Nivel del log que se va a escribir */
            QString m_buffer; /**< Buffer donde se va almacenar el mensaje de log que se forme */
            SAM::Debug m_qtDebug; /**< _Stream_ de depuración. */
            Logger & m_logger; /**< Referencia al logger que está escribiendo el mensaje. */

        public:
            explicit Helper(Level logLevel, Logger & logger);

            ~Helper();

            /**
             * @brief Devuelve un _stream_ al que se pueden serializar objetos para
             *  que se escriban como parte del mensaje de log.
             *
             * @return SAM::Debug
             */
            SAM::Debug& stream() { return m_qtDebug; }
        };

        /**
         * @brief Ayuda para obtener trazas de entrada y salida a funciones C++
         *
         */
        class TraceHelper
        {
            Level m_level;
            Logger & m_logger;
            const char * m_szFunction;

        public:
            TraceHelper(Level level, Logger & logger, const char * szFunction, const char * szFile, int line);
            ~TraceHelper();
        };

    private:
        /**
         * @brief Escribe el mensaje en todos los `appenders` a los que esté suscrito
         * éste logger.
         *
         * @param message Mensaje de log a escribir.
         */
        void write(const QString& message);

        ExplicitlySharedDataPointer<LoggerPrivate> m_ptr;
    };

} // end namespace



#endif
