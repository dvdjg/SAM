# Add more folders to ship with the application, here
folder_01.source = qml/SCV_Viewer
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    statemodel.cpp \
    stateitem.cpp \
    programmodel.cpp \
    programitem.cpp \
    messagemodel.cpp \
    messageitem.cpp \
    mainapp.cpp \
    listmodel.cpp \
    interfacemodel.cpp \
    interfaceitem.cpp

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    statemodel.h \
    stateitem.h \
    programmodel.h \
    programitem.h \
    messagemodel.h \
    messageitem.h \
    mainapp.h \
    listmodel.h \
    interfacemodel.h \
    interfaceitem.h
