VERSION = 0.6

NAME = SAM

!include( ../SAM.pri ) {
    error( "¡No puedo encontrar el archivo SAM.pri!" )
}

TEMPLATE = lib
CONFIG += staticlib
CONFIG += exceptions
CONFIG -= rtti
QT       += core network concurrent
QT       -= gui

TOP_DIR = $$PWD/../..

TARGET = $${NAME}$${POST}

message("Generando $$BUILD_POSTFIX $$TARGET")

OBJECTS_DIR = $$TOP_DIR/Intermediate/$${NAME}_$${ARCH}_$${BUILD_POSTFIX}

INCLUDEPATH += $$PWD/../zlib
DEPENDPATH += $$PWD/../zlib

DESTDIR = $$TOP_DIR/Libraries/$$ARCH


# Entrecomillar las macros que se van a pasar al compilador
STRNAME = '\\"$${NAME}\\"'
STRVERSION = '\\"$${VERSION}. Svn: $${SVN_VERSION}\\"'
DEFINES += SAM_TARGET=\"$${STRNAME}\" SAM_VERSION=\"$${STRVERSION}\" LONGLONG_EXISTS
DEFINES += SAM_COMPILER_VERSION=\"$${COMPILER_ID}\"
DEFINES += SAM_HOST_ARCH=\"\\\"$${QMAKE_HOST.arch}\\\"\"
DEFINES += SAM_HOST_OS=\"\\\"$${QMAKE_HOST.os}\\\"\"
DEFINES += SAM_HOST_VERSION=\"\\\"$${QMAKE_HOST.version}\\\"\"
DEFINES += SAM_HOST_NAME=\"\\\"$${QMAKE_HOST.name}\\\"\"

SOURCES += \
    SAM_Log.cpp \
    SAM_Helpers.cpp \
    SAM_TcpServer.cpp \
    SAM_TcpServerThread.cpp \
    SAM_UdpServerThread.cpp \
    SAM_UdpServer.cpp \
    SAM_TcpUdpClient.cpp \
    SAM_SignalHandler.cpp \
    SAM_CommandObject.cpp \
    SAM_Logger.cpp \
    SAM_TcpUdpServer.cpp \
    SAM_LoggerAppender.cpp \
    SAM_ZIP.cpp \
    basename.cpp \
    qtiocompressor.cpp \
    SAM_LoggerContainer.cpp \
    SAM_Packet.cpp \
    SAM_JsonUtil.cpp \
    SAM_NetworkOrder.cpp \
    memoryStats.cpp \
    getCPUTime.cpp \
    SAM_CPUInfo.cpp \
    SAM_Telnet.cpp \
    SAM_JVariant.cpp \
    keepSocketAlive.cpp \
    SAM_Ping.cpp \
    MurmurHash2_64.cpp


HEADERS += \
    SAM_Log.h \
    SAM_Helpers.h \
    SAM_TcpServer.h \
    SAM_TcpServerThread.h \
    SAM_UdpServerThread.h \
    SAM_UdpServer.h \
    SAM_Packet.h \
    SAM_TcpUdpClient.h \
    SAM_SignalHandler.h \
    SAM_Types.h \
    SAM_CommandObject.h \
    qtiocompressor.h \
    SAM_Logger.h \
    SAM_TcpUdpServer.h \
    SAM_LoggerAppender.h \
    basename.h \
    SAM_ZIP.h \
    SAM_LoggerContainer.h \
    SAM_NetworkOrder.h \
    typet.hpp \
    ifthenelse.hpp \
    get_typet.hpp \
    SAM_DeclarationCommon.h \
    SAM_Debug.h \
    declare_private.h \
    SAM_ExplicitlySharedDataPointer.h \
    SAM_CommandObjectPrivate.h \
    SAM_InjectorPrivate.h \
    SAM_TcpUdpServerPrivate.h \
    SAM_TcpServerPrivate.h \
    SAM_JsonUtil.h \
    SAM_GetUngetC.h \
    SAM_UdpServerPrivate.h \
    SAM_MessageProcessorPrivate.h \
    SAM_TcpUdpClientPrivate.h \
    memoryStats.h \
    getCPUTime.h \
    SAM_QEnumToString.h \
    SAM_CPUInfo.h \
    SAM_Telnet.h \
    SAM_JVariant.h \
    keepSocketAlive.h \
    SAM_Ping.h \
    SAM_SleeperThread.h \
    MurmurHash.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

