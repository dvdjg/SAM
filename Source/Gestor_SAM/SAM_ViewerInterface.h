#ifndef SAM_VIEWERINTERFACE_H
#define SAM_VIEWERINTERFACE_H

#include "SAM_CommandObject.h"

namespace SAM
{
class ViewerInterfacePrivate;

class ViewerInterface : public CommandObject
{
    Q_OBJECT
protected:
    J_DECLARE_PRIVATE(ViewerInterface)
    Q_DISABLE_COPY(ViewerInterface)

public:
    explicit ViewerInterface(QObject *parent = 0);
    
signals:
    
public slots:
    /**
     * @brief Inicializaciones dependientes del contexto del hilo.
     */
    void init();
};

}

#endif // SAM_VIEWERINTERFACE_H
