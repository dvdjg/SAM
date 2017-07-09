#ifndef SAM_COMMANDOBJECTPRIVATE_H
#define SAM_COMMANDOBJECTPRIVATE_H

#include "SAM_JVariant.h"

namespace SAM
{
    class CommandObjectPrivate
    {
        friend class CommandObject;

    public:
        virtual ~CommandObjectPrivate() {}

        const JVariant & appSettings() const
        {
            return m_appSettings;
        }

    protected:

//        /**
//         * @brief Llamado cuando se actualiza la copia de las configuraciones de la aplicación.
//         *  Se puede sobrecargar en clases derivadas si se desea ejecutar código que haga uso de ellas.
//         *
//         * @param settings Configuraciones de la aplicación.
//         */
//        virtual void setAppSettings(JVariant settings)
//        {
//            m_appSettings = settings;
//        }

        JVariant m_appSettings;
    };
}

#endif // SAM_COMMANDOBJECTPRIVATE_H
