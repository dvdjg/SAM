#ifndef SAM_SAGE_H
#define SAM_SAGE_H

#include <QStringList>
#include <QAbstractSocket>
#include "SAM_CommandObject.h"

namespace SAM
{
    class SAGEPrivate;

    class SAGE : public CommandObject
    {
        Q_OBJECT

        Q_PROPERTY(QVariant measures READ measures)

        J_DECLARE_PRIVATE(SAGE)
        Q_DISABLE_COPY(SAGE)

    protected:
        SAGE(QObject * parent, int);


    public:

        SAGE(QObject * parent = 0);

        /**
         * @brief Inicia un ciclo de procesamiento de secuencias lo que implica a varias
         *  llamadas internas a SAGE::processSequenceElement().
         *  No debe llamarse mientras que exista otra secuencia en ejecución.
         * @param sequence Secuencia de mensajes que se pretende enviar al SAGE.
         *  Las secuencias tendrán un formato:
         *  [
         *    {"exec":"intfc", "parse":[{"name":"Initial State", "regexp":"(.*)"}]},
         *    {"wait":10000},
         *    {"exec":"dir", "parse":[{"name":"Directory Contents", "regexp":"(.*)"}]}
         *  ]
         *
         *  Es un array de órdenes que pueden ser una de las siguientes:
         *  - exec.- Envía el comando especificado al SAGE. En éste caso `intfc`. La respuesta se
         *      parseará con el array de expresiones regulares que se indican dentro de `parse`.
         *      > name.- Nombre que se dará al resultado de la expresión regular.
         *      > regexp.- Expresión regular que extraerá los datos requeridos del resultado de la
         *          ejecución de la orden en el SAGE.
         *  - wait.- Espera la cantidad de milisegundos especificada.
         */
        void processSequence(const JVariant & sequence);

        /**
         * @brief Devuelve la colección de todas las medidas que se llevan efectuadas.
         *  La secuencia de medidas se selecciona mediante la función SAGE::processSequence().
         */
        QVariant measures() const;

        /**
         * @brief Limpia todas las medidas.
         */
        void clearMeasures();

    private slots:
        /**
         * @brief Recibe los mensajes del telnet en crudo, es decir, con las secuencias de escape
         *  de terminal y retornos de carro.
         *  Para analizar las respuestas a los comandos enviados, usar la función
         *  SAGE::onCommandResponse() en su lugar.
         * @param msg
         */
        void telnetRawMessage(const QString &msg);

        /**
         * @brief Llamada automáticamente durante el proceso de logado,
         *  cuando se necesita el nombre y password.
         */
        void telnetLoginRequired();
        /**
         * @brief Fallo en el proceso de logado.
         */
        void telnetLoginFailed();

        /**
         * @brief Notificación de que se ha deslogado
         */
        void telnetLoggedOut();

        /**
         * @brief Cuando se haya conseguido logar al SAGE, se enviará la secuencia de
         *  iniciación "SAGE_INIT" leida desde el archivo de configuración .ini.
         *
         * @sa SAGE::processSequence()
         */
        void telnetLoggedIn();

        /**
         * @brief Se ha producido un error a nivel de Socket.
         * @param error
         */
        void telnetSocketError(QAbstractSocket::SocketError error);

        /**
         * @brief Ha cumplido un timeout al no responder el SAGE en un tiempo prudencial
         *  configurado mediante Telnet::setTimeToRetry(int).
         */
        void telnetResponseTimeout();

        /**
         * @brief Envía asíncronamente el comando exec correspondiente a la secuencia
         *  de mediciones en curso. En caso de recibir una respuesta válida se enviará
         *  la señal SAGE::measure(QString name, QString value).
         *  Normalmente no es necesario llamar a esta función directamente: Cuando se
         *  ordena el procesamiento de una nueva secuencia mediante SAGE::processSequence()
         *  se inicia automáticamente el ciclo de mediciones. Según se recibe la respuesta
         *  en SAGE::onCommandResponse() se continúa con el siguiente elemento de la secuencia
         *  hasta que se termine.
         *
         *  @return false si no ha podido procesar el elemento de la secuencia,
         *  posiblemente porque haya acabado.
         */
        bool processSequenceElement();

    public slots:

        virtual void setAppSettings(QVariant settings, QStringList path);

        /**
         * @brief Intenta conectase al host especificado por la URL strURL.
         *  En caso de no proporcionar esa URL se tomará el valor "SAGE_TELNET_URL"
         *  del archivo de configuración.
         */
        virtual void connectToHost(QString strURL = QString());

        /**
         * @brief Envía la orden de suspensión SUSP a nivel de Telnet.
         */
        virtual void suspend();

        /**
         * @brief Envía la orden de interrupción IP a nivel de Telnet.
         */
        virtual void quit();

        /**
         * @brief Envía la cadena str al servidor Telnet que se encuentre conectado.
         *  La respuesta se recibirá a través de la señal SAGE::onCommandResponse().
         *
         * @param str
         * @return El número de bytes que se han escrito al SAGE.
         */
        virtual qint64 sendCommand(const QString & str);

        /**
         * @brief Petición para deslogarse. Provocará una llamada a SAGE::telnetLoggedOut().
         */
        virtual void logout();

        /**
         * @brief Se ha recibido la respuesta al último comando solicitado.
         *  Si casa con un elemnto activo de la secuencia por la que actualmente se espera,
         *  se emitirá una o más señales SAGE::measure() indicando que se ha realizado
         *  correctamente la medición.
         *
         * @param command Eco del comando que se envió al SAGE
         * @param response Respuesta del SAGE
         */
        void onCommandResponse(QString command, QString response);

    signals:
        /**
         * @brief Señal emitida cada vez que se ha conseguido parsear un valor
         *  proviniente del SAGE. Los patrones son los que se seleccionan mediante
         *  la función SAGE::processSequence().
         * @param name
         * @param value
         */
        void measure(QString name, QString value);

        /**
         * @brief Se ha finalizado con la realización de la secuencia de medidas
         *  que estaba en curso.
         *
         * @param bError Indica si las mediciones son válidas o si por el contrario
         *  no se han podido realizar, posiblemente por un timeout.
         */
        void measureSequenceFinished(bool bError = false);
    };
}

#endif // SAM_SAGE_H
