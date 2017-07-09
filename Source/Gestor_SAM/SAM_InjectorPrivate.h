#ifndef SAM_INJECTORPRIVATE_H
#define SAM_INJECTORPRIVATE_H

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QTimer>
#include "SAM_JVariant.h"
#include "qtiocompressor.h"
#include "SAM_CommandObjectPrivate.h"

namespace SAM
{
    /**
     * @brief Implementación privada de la clase SAM::Injector, con las variables necesarias
     * para
     *
     */
    class InjectorPrivate : public CommandObjectPrivate
    {
        friend class Injector;

    protected:
        InjectorPrivate();
        virtual ~InjectorPrivate() {}

        QFile          m_injectFile;    /**< Archivo del que se leerán los paquetes */
        QtIOCompressor m_ioCompressor;  /**< Ayuda a la descompresión del archivo de entrada */
        QIODevice    * m_pDevice;       /**< Dispositivo de lectura que puede ser m_injectFile o m_ioCompressor */
        QTextStream    m_textStream;    /**< Lectura del archivo de texto JSON */
        qint64         m_fileOffset;    ///< Offset en milisegundos del primer mensaje
        JVariant       m_packet;        /**< Siguiente paquete a inyectar */
        uint           m_packetInjectedCount;   /**< Número de paquetes inyectados */
        qint64         m_bytesInjectedCount;    /**< Número de bytes inyectados */
        QTimer*        m_pTimer;
    };

}

#endif // SAM_INJECTORPRIVATE_H
