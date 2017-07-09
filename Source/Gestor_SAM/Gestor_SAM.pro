VERSION = 0.9

NAME = SAM_Gestor

!include( ../SAM.pri ) {
    error( "¡No puedo encontrar el archivo SAM.pri!" )
}

TEMPLATE  = app
CONFIG += static
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += exceptions
CONFIG   -= rtti
QT       += core network concurrent
QT       -= gui

TOP_DIR = $$PWD/../..

TARGET = $${NAME}$${POST}

message("Generando $$BUILD_POSTFIX $$TARGET")

OBJECTS_DIR = $$TOP_DIR/Intermediate/$${NAME}_$${ARCH}_$${BUILD_POSTFIX}

win32-msvc*{
LIBS += $$TOP_DIR/Libraries/$${ARCH}/SAM$${POST}.lib
LIBS += $$TOP_DIR/Libraries/$${ARCH}/zip$${POST}.lib
}
else {
LIBS += $$TOP_DIR/Libraries/$${ARCH}/libSAM$${POST}.a
LIBS += $$TOP_DIR/Libraries/$${ARCH}/libzip$${POST}.a
}

INCLUDEPATH += $$PWD/../SAMlib $$PWD/../zlib
DEPENDPATH += $$PWD/../SAMlib $$PWD/../zlib

DESTDIR = $$TOP_DIR/Binaries/$$ARCH

win32-msvc*: PRE_TARGETDEPS += $$TOP_DIR/Libraries/$$ARCH/SAM$${POST}.lib \
                               $$TOP_DIR/Libraries/$$ARCH/zip$${POST}.lib
else: PRE_TARGETDEPS += $$TOP_DIR/Libraries/$$ARCH/libSAM$${POST}.a \
                        $$TOP_DIR/Libraries/$$ARCH/libzip$${POST}.a


# Entrecomillar las macros que se van a pasar al compilador
STRNAME = '\\"$${NAME}\\"'
STRVERSION = '\\"$${VERSION}.$${SVN_VERSION}\\"'
DEFINES += SAM_TARGET=\"$${STRNAME}\" SAM_VERSION=\"$${STRVERSION}\" LONGLONG_EXISTS
DEFINES += SAM_COMPILER_VERSION=\"$${COMPILER_ID}\"
DEFINES += SAM_HOST_ARCH=\"\\\"$${QMAKE_HOST.arch}\\\"\"
DEFINES += SAM_HOST_OS=\"\\\"$${QMAKE_HOST.os}\\\"\"
DEFINES += SAM_HOST_VERSION=\"\\\"$${QMAKE_HOST.version}\\\"\"
DEFINES += SAM_HOST_NAME=\"\\\"$${QMAKE_HOST.name}\\\"\"

# DEFINES += QT_USE_QSTRINGBUILDER

SOURCES += \
    main.cpp \
    SAM_MessageProcessorPrivate.cpp\
    SAM_MessageProcessor.cpp \
    SAM_StateMachine.cpp \
    SAM_StateMachineResponseTransition.cpp \
    SAM_StateMachineResponseEvent.cpp \
    SAM_UdpServerMessageProcessor.cpp \
    SAM_TcpServerCommandProcessor.cpp \
    SAM_Injector.cpp \
    SAM_MainTask.cpp \
    SAM_Testing.cpp \
    SAM_DataBase.cpp \
    SAM_StateMachineEvents.cpp \
    SAM_Sniffer.cpp \
    SAM_UdpServerSniffer.cpp \
    SAM_SAGE.cpp \
    SAM_ViewerInterface.cpp


HEADERS += \
    SAM_StateMachinePrivate.h \
    SAM_MainTask.h \
    SAM_Archive.h \
    SAM_Declaration.h \
    SAM_DeclarationParser.h \
    SAM_UdpServerMessageProcessor.h \
    SAM_TcpServerCommandProcessor.h \
    SAM_FilterText.h \
    SAM_MessageProcessor.h \
    SAM_Archive.h \
    SAM_Injector.h \
    SAM_StateMachine.h \
    SAM_StateMachineResponseEvent.h \
    SAM_StateMachineResponseTransition.h \
    SAM_DeclarationCommon.h \
    SAM_DeclarationToString.h \
    SAM_Testing.h \
    SAM_MessageProcessorPrivate.h \
    SAM_DataBase.h \
    SAM_MeasuresCount.h \
    SAM_Sniffer.h \
    SAM_InjectorPrivate.h \
    SAM_UdpServerSniffer.h \
    SAM_SAGE.h \
    SAM_ViewerInterface.h


unix {

LIBS += -lpthread -lrt

}


win32 {

LIBS += -lWs2_32 -lpsapi -lpdh -lAdvapi32

}


# Copies the given files to the destination directory after success linking
FILES_TO_DESTDIR = $$PWD/SAM_Gestor.ini

for(FILE, FILES_TO_DESTDIR) {
    DDIR = $$DESTDIR

    # Replace slashes in paths with backslashes for Windows
    win32:FILE ~= s,/,\\,g
    win32:DDIR ~= s,/,\\,g

    QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
}

OTHER_FILES += \
    SAM_Gestor.ini \
    SAM_DeclarationParser.xsl \
    SAM_Declaration.xsl \
    SAM_Declaration.xml

