#include "SAM_Log.h"
#include "SAM_MainTask.h"
#include "SAM_TcpServer.h"
#include "SAM_TcpUdpClient.h"
#include "SAM_CommandObjectPrivate.h"
#include "SAM_ViewerInterface.h"

namespace SAM
{

class ViewerInterfacePrivate : public CommandObjectPrivate
{
public:
    ViewerInterfacePrivate(QObject * parent = 0) :
        m_client(0), m_server(0) {}
    TcpUdpClient * m_client;
    TcpServer * m_server;
};

ViewerInterface::ViewerInterface(QObject *parent) :
    CommandObject(parent)
{
    TRACE(LOCAL);

    m_ptr.reset(new ViewerInterfacePrivate(parent));
}

void ViewerInterface::init()
{
    J_D(ViewerInterface);

    QString url;

    d->m_client = new TcpUdpClient(url, this);
    d->m_server = new TcpServer(this);

    // Escucha las conexiones entrantes TCP de comandos
    d->m_server->setUrl(url);
    d->m_server->setObjectName(url);

    // Establecer la clase que escuchará los mensajes que lleguen
    //d->m_server->setRunnableFactory<TcpServerCommandProcessor>();

    g_pMainTask->registerLocalCommandObject(d->m_server);
    //registerCommandObject(tcpSamServerCommands)->start();

    // Inicia la escucha
    QMetaObject::invokeMethod( d->m_server, "listen", Qt::QueuedConnection );
}

}
