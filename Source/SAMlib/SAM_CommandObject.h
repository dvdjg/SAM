#ifndef SAM_COMMANDOBJECT_H
#define SAM_COMMANDOBJECT_H

#include <QObject>
#include <QVariant>
#include <QStringList>

#include "SAM_JVariant.h"
#include "declare_private.h"

/**
 *  Crea la infraestructura para implementar un sistema de ocultación de la implementación
 *  a la interfaz pública de una clase.
 */
#define S_DECLARE_PRIVATE_D(Dptr, Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(qGetPtrHelper(Dptr)); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(qGetPtrHelper(Dptr)); } \
    friend class Class##Private;

#define S_DECLARE_PRIVATE(Class) S_DECLARE_PRIVATE_D(m_ptr, Class)

namespace SAM
{

    class CommandObjectPrivate;

    /**
     * @brief Clase base para implememntar un sistema de comunicación de mensajes entre objetos.
     *  Las clases derivadas de SAM::CommandObject son claras candidatas a tener su propio hilo.
     *  Mediante QObject::moveToThread() pueden engancharse a un thread en particular.
     */
    class CommandObject : public QObject
    {
        Q_OBJECT

    protected:
        J_DECLARE_PRIVATE(CommandObject)
        Q_DISABLE_COPY(CommandObject)

    protected:
        CommandObject(QObject * parent, int); // No inicia el objeto privado
        void construct();

    public:
        explicit CommandObject(QObject *parent = 0);
        virtual ~CommandObject();

        /**
         * @brief Devuelve información interna del objeto.
         *
         * @return QVariantMap
         */
        virtual QVariantMap getInfo() const;

        /**
         * @brief Devuelve el número de objetos de tipo CommandObject que se han creado.
         *
         * @return int
         */
        static int createdCommandObjectCount();

        /**
         * @brief Establece el nombre del objeto. Notifica el cambio de nombre
         * mediante la emisión de una señal de tipo CommandObject::info().
         *
         * @param name
         */
        void setObjectName(const QString &name);

        /**
         * @brief Devuelve la configuración de la aplicación.
         *
         * @return JVariant
         */
        const JVariant & appSettingsConst() const;

    protected:
        /**
         * @brief Devuelve la configuración de la aplicación.
         *
         * @return JVariant
         */
        JVariant & appSettings();


    signals:
        /**
         * @brief Comunicación de información del objeto al mundo exterior.
         *
         * @param who Identifica al objeto mediante una cadena de caracteres
         * @param what Identifica qué se está comunicando.
         * @param data Datos asociados a la información que se comunique.
         */
        void info(QString who, QString what, QVariant data);

        /**
         * @brief Finalización definitiva.
         */
        void finished();

    public slots:
        /**
         * @brief Inicializaciones dependientes del contexto del hilo.
         */
        virtual void init();

        /**
         * @brief Llamar para hacer una terminación ordenada del objeto.
         */
        virtual void quit();

        /**
         * @brief Procesa las órdenes que se le envíen a éste objeto.
         * Ejemplo:
         * @code processCommand(".*", "quit"); // hace que el objeto se libere. @endcode
         * @param strCommand Orden que se da al objeto:
         *  - `quit` .- Provoca que el objeto se auto-destruya.
         *  - `info` .- Provoca que el objeto emita la señal info().
         * @param namePattern Patrón QRegEx con el que se comparará el nombre del objeto.
         *  Si no tiene un match completo, el comando será ignorado.
         * @see processCommand(const QStringList &)
         */
        virtual void processCommand(QString strCommand, QString namePattern = ".*");

        /**
         * @brief Procesa las órdenes que se le envíen a éste objeto.
         *
         * @param command Comando separado por los espacios.
         * @sa processCommand(QString strCommand, QString namePattern)
         */
        virtual void processCommand(QStringList command);

        /**
         * @brief Actualiza la configuración de la aplicación.
         *  Usar esta función como método por defecto de actualizar la configuración.
         *
         * @param settings Configuración de la aplicación.
         */
        virtual void setAppSettings(QVariant settings, QStringList path = QStringList());

    protected:

        /**
         * @brief Único puntero que se ofrece en la interfaz pública del objeto.
         *  Así se eliminan dependencias de elementos internos y permiten una compatibilidad binaria.
         */
        QScopedPointer<CommandObjectPrivate> m_ptr;
    };

}

#endif // SAM_COMMANDOBJECT_H
