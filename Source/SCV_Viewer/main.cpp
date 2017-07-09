#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include "mainapp.h"

int main(int argc, char *argv[])
{
    MainApp app(argc, argv);
    //QGuiApplication app(argc, argv);

    QtQuick2ApplicationViewer viewer;

    app.setView(&viewer);
    //viewer.setResizeMode(QtQuick2ApplicationViewer::SizeRootObjectToView);
    app.init();
    viewer.setMainQmlFile(QStringLiteral("main.qml"));

    viewer.showExpanded();

    return app.exec();
}
