#ifndef MAINAPP_H
#define MAINAPP_H

#include <QGuiApplication>
#include <QTimer>

class QQuickView;
class QTimer;
class StateItem;
class InterfaceModel;
class MessageModel;

class MainApp : public QGuiApplication
{
    Q_OBJECT
public:
    //explicit MainApp(QQuickView * quickView, QObject *parent = 0);
    MainApp(int &argc, char **argv, int flags = ApplicationFlags);
    void setView(QQuickView * quickView) { m_viewer = quickView; }
    void init();

signals:
    void message(QVariant from, QVariant to, QVariant type);

public slots:
    void onTimer();

protected:
    QQuickView * m_viewer;

    StateItem * m_interfaceTurnedState;
    StateItem * m_programTurnedState;
    InterfaceModel * m_interfaceModel;
    MessageModel * m_messageModel;
    QTimer m_timer;
};

#endif // MAINAPP_H
