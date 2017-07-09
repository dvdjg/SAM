#include "mainapp.h"
#include "qtquick2applicationviewer.h"
#include "interfacemodel.h"
#include "interfaceitem.h"
#include "programitem.h"
#include "programmodel.h"
#include "stateitem.h"
#include "statemodel.h"
#include "messageitem.h"
#include "messagemodel.h"

#include <QQmlContext>
#include <QQuickItem>

MainApp::MainApp(int &argc, char **argv, int flags) :
    QGuiApplication(argc, argv, flags), m_viewer(0),
    m_interfaceTurnedState(0), m_programTurnedState(0),
    m_interfaceModel(0), m_messageModel(0)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    m_timer.start(1000);
}

void MainApp::onTimer()
{
//    static int count = 0;
//    if(m_interfaceTurnedState) {
//        m_interfaceTurnedState->setValue(QString("Init_%1").arg(count++));
//    }
    QQmlContext *ctxt = m_viewer->rootContext();
    QVariant vProp = ctxt->contextProperty("interfaces");
    InterfaceModel * interfaceModel = vProp.value<InterfaceModel *>();
    vProp = ctxt->contextProperty("messages");
    MessageModel * messageModel = vProp.value<MessageModel *>();
    if(interfaceModel)
    {
        int rand = qrand() ^ (qrand() << 15);
        int nRows = interfaceModel->rowCount();
        if(nRows > 0)
        {
            InterfaceItem * interfaceItem = (InterfaceItem *) interfaceModel->row(rand % nRows);

            const char * aszTurned[] = {"On", "Off", "StandBy", "None"};
            const char * szTurned = aszTurned[(rand >> 5) % 4];
            interfaceItem->setturned(szTurned);
        }

        /////////////////////////////////////////////////////////////////////////////////

        const char * aszType[] = {"PRES", "ESTF", "ESRD", "SAM"};
        const char * szType = aszType[(rand >> 15) % 4];
        const char * aszName[] = {"Llamada", "Conversación", "Intervención", "Medida"};
        const char * szName = aszName[(rand >> 17) % 4];
        InterfaceItem * interfaceFrom = (InterfaceItem *) interfaceModel->row((rand >> 5) % nRows);
        InterfaceItem * interfaceTo = (InterfaceItem *) interfaceModel->row((rand >> 10) % nRows);

        if(interfaceFrom->programs()->rowCount() > 0 && interfaceTo->programs()->rowCount() > 0)
        {
            ProgramItem * programFrom = static_cast<ProgramItem *>(interfaceFrom->programs()->row(0));
            ProgramItem * programTo = static_cast<ProgramItem *>(interfaceTo->programs()->row(0));
            unsigned int id = messageModel->nextid();
            messageModel->setnextid(id+1);
            QString idMessage = QString::number(id);

            MessageItem * messageItem = 0;

            if(messageModel->rowCount() > 15) {
                messageItem = (MessageItem *) messageModel->row(id & 15);
                messageItem->setid(idMessage);
                messageItem->settype(szType);
                messageItem->setname(szName);
            } else {
                messageItem = new MessageItem(idMessage, szType, szName);
                messageModel->appendRow(messageItem);
            }

            messageItem->setprogramFrom(programFrom, true);
            messageItem->setprogramTo(programTo, true);
            programFrom->setlastMessageSent(messageItem, true);
            programTo->setlastMessageReceived(messageItem, true);
        }

//        emit message(QVariant::fromValue((QObject*)interfaceFrom),
//                     QVariant::fromValue((QObject*)interfaceTo),
//                     szType);
    }


}

void MainApp::init()
{
    ProgramItem * programItem = 0;
    InterfaceItem * interfaceItem = 0;
    m_interfaceModel = new InterfaceModel(this);
    interfaceItem = new InterfaceItem("192.168.1.2", "Router", "On");
    programItem = new ProgramItem("ROUT", "RoutGestion", "Admin", "On");
    m_programTurnedState = new StateItem("Turned", "On");
    programItem->states()->appendRow(m_programTurnedState);
    programItem->states()->appendRow(new StateItem("BandWidth", "1GB/s"));
    interfaceItem->programs()->appendRow(programItem);
    m_interfaceModel->appendRow(interfaceItem);
    interfaceItem = new InterfaceItem("192.168.5.3", "Q6600", "On");
    interfaceItem->programs()->appendRow(new ProgramItem("PUES", "Pos-01", "Abon-01", "On"));
    interfaceItem->programs()->appendRow(new ProgramItem("WATC", "Watchdog", "Watchdog", "On"));
    interfaceItem->states()->appendRow(new StateItem("Turned", "STAND-BY"));
    interfaceItem->states()->appendRow(new StateItem("Light", "Green"));
    m_interfaceTurnedState = (StateItem *) interfaceItem->states()->find("Turned");
    if(programItem){
        programItem = interfaceItem->programs()->find("Pos-01");
    }
    programItem->states()->appendRow(new StateItem("Turned", "WRONG"));
    m_interfaceModel->appendRow(interfaceItem);
    interfaceItem = new InterfaceItem("192.168.3.4", "P4", "On");
    m_interfaceModel->appendRow(interfaceItem);
    interfaceItem = new InterfaceItem("192.168.3.10", "Z-10", "On");
    interfaceItem->programs()->appendRow(new ProgramItem("PUES", "Pos-04", "Abon-04", "On"));
    m_interfaceModel->appendRow(interfaceItem);
    interfaceItem = new InterfaceItem("192.168.3.12", "Z-11", "On");
    interfaceItem->programs()->appendRow(new ProgramItem("PUES", "Pos-05", "Abon-05", "On"));
    m_interfaceModel->appendRow(interfaceItem);
    interfaceItem = new InterfaceItem("192.168.3.13", "Z-12", "Off");
    interfaceItem->programs()->appendRow(new ProgramItem("PUES", "Pos-06", "Abon-06", "On"));
    m_interfaceModel->appendRow(interfaceItem);
    interfaceItem = new InterfaceItem("192.168.3.14", "Z-13", "StandBy");
    interfaceItem->programs()->appendRow(new ProgramItem("PUES", "Pos-07", "Abon-07", "On"));
    m_interfaceModel->appendRow(interfaceItem);
    interfaceItem = new InterfaceItem("192.168.3.15", "Z-14", "Off");
    interfaceItem->programs()->appendRow(new ProgramItem("PUES", "Pos-08", "Abon-08", "On"));
    m_interfaceModel->appendRow(interfaceItem);
    interfaceItem = new InterfaceItem("192.168.3.16", "Z-15", "On");
    interfaceItem->programs()->appendRow(new ProgramItem("PUES", "Pos-09", "Abon-09", "On"));
    m_interfaceModel->appendRow(interfaceItem);

    //m_viewer->setMainQmlFile(QStringLiteral("main.qml"));

    QQmlContext *ctxt = m_viewer->rootContext();
    ctxt->setContextProperty("interfaces", m_interfaceModel);

    //////////////////////////////////////////////////////////////////////////

    m_messageModel = new MessageModel(this);
    ctxt->setContextProperty("messages", m_messageModel);

//    QQuickItem * rootObject = m_viewer->rootObject();
//    connect(this, SIGNAL(message(QVariant, QVariant, QVariant)),
//                     rootObject, SLOT(extmessage(QVariant, QVariant, QVariant)));
}
