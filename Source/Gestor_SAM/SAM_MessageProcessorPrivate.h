#ifndef SAM_MESSAGEPROCESSORPRIVATE_H
#define SAM_MESSAGEPROCESSORPRIVATE_H

#include <QMap>
#include <QFile>

#include "qtiocompressor.h"
#include "SAM_JVariant.h"
#include "SAM_Declaration.h"
#include "SAM_CommandObjectPrivate.h"
#include "SAM_MessageProcessor.h"

class QStateMachine;
class QTimer;

namespace SAM
{
    using namespace SAM_Declaration;

    class TcpUdpClient;
    class Packet;
    class StateMachine;

    class MessageProcessorPrivate : public CommandObjectPrivate
    {
        friend class MessageProcessor;

    protected:

        explicit MessageProcessorPrivate();

        /**
         * @brief Devuelve las configuraciones del objeto.
         *
         * @return JVariant
         */
        JVariant & settings() { return m_settings; }

        /**
         * @brief Devuelve las configuraciones del objeto.
         *
         * @return JVariant
         */
        const JVariant & constSettings() const { return m_settings; }

        /**
         * @brief Devuelve el número total de mensajes SCV que se han recibido
         *  y estaban bien formados.
         *  Se incluyen los mensajes que no serán procesados.
         *
         * @return qint64
         */
        qint64 receivedMsgCount() const { return m_receivedMsgCount; }

        /**
         * @brief Devuelve el número total de mensajes SAM que se han procesado.
         *
         * @return qint64
         */
        qint64 processedMsgCount() const { return m_processedMsgCount; }

    protected:

        QStateMachine & stateMachineInternal() { return m_stateMachine->stateMachineInternal(); }
        const QStateMachine & stateMachineInternal() const { return m_stateMachine->stateMachineInternal(); }

        E_MODO_SAM modoSAM() const;

        /**
         * @brief Rellena la cabecera de un nuevo mensaje en respuesta a otro
         *  recibido previamente del que toma algunos valores.
         * @note Se asume que se usa la codificacion BigEndian para la estructura de salida.
         *
         * @param msg Referencia de la estrcutura cuya cabecera será rellenada.
         * @param pMsg dirección de una estructura de la que coger algunos datos de la cabecera.
         * @param idMsg Identificador del mensaje.
         * @param lng longitud de la esctructura resultante
         * @param nMessage Número de mensaje que se va a usar.
         */
        void initResponseMessageBE(MSG_TH & msg,
                const MSG_TH *pMsg,
                E_ID_MSG idMsg,
                short lng);

    protected:

        /**
         * @brief Clase auxiliar para reconstruir los archivos de configuración
         *
         */
        class ReceivingFile
        {
        public:
            ReceivingFile() : blockCounter(0) {}

            unsigned int blockCounter; /**< Cuenta de bloques que se debe corresponder con la recibida */
            QByteArray data; /**< Buffer para reconstruir los archivos recibidos */

            void clear() { blockCounter = 0; data.clear(); }
        };

        qint64 m_receivedMsgCount; /**< Cuenta el número total de mensajes SCV que se han recibido correctamente*/
        qint64 m_processedMsgCount; /**< Cuenta el número total de mensajes SAM que se han procesado*/
        qint64 m_nRejectedBytes; /**< Número de bytes que se han rechazado en el análisis */
        qint64 m_nReceivedBytes; /**< Número de bytes que se han recibido para ser procesados */

        QMap<QString, ReceivingFile> m_receivingFiles; /**< Contenedor para reconstruir los archivos que se reciban */

        unsigned int m_sessionCounter; /**< Cuenta el número de sesiones de usuario que se han concedido */
        unsigned int m_batteryCounter; /**< Cuenta el número de baterías de prueba que se han creado hasta el momento */
        unsigned int m_sessionOpened; /**< Almacena el número de la sesión actualmente abierta. A 0 no hay sesión */

        JVariant m_settings;

        StateMachine * m_stateMachine;

        QTimer * m_timerPresWatchdog; ///< Temporizador para los mensajes de presencia del Watchdog
        QTimer * m_timerPresEstad; ///< Temporizador para los mensajes de presencia del agente de Estadísticas

        /** Cliente para enviar los mensajes de presencia al Watchdog
         *  Éste cliente no necesita buffering ni ser emitido desde otros hilos, por lo
         *  que se usa una instancia directamente en lugar de enviarlo a un objeto especializado.
         */
        TcpUdpClient * m_pWatchdogClient;
    };
}

#endif // SAM_MESSAGEPROCESSORPRIVATE_H
