#-------------------------------------------------
#
# Project created by QtCreator 2012-10-23T10:15:23
#
#-------------------------------------------------

NAME = zip


QT       -= core gui


TEMPLATE = lib
CONFIG += staticlib
CONFIG += exceptions
CONFIG -= rtti


win32:ARCH= Win32
unix:ARCH= Unix

CONFIG(debug, debug|release) {
    POST=d
    BUILD_POSTFIX=Debug
} else {
    POST=
    BUILD_POSTFIX=Release
}


TARGET = $${NAME}$${POST}

TOP_DIR = $$PWD/../..

DESTDIR = $$TOP_DIR/Libraries/$$ARCH

OBJECTS_DIR = $$TOP_DIR/Intermediate/$${NAME}_$${ARCH}_$${BUILD_POSTFIX}

win32 {

DEFINES += _WIN32_WINNT=0x0501
DEFINES += _SCL_SECURE_NO_DEPRECATE _CRT_SECURE_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_DEPRECATE _AFX_SECURE_NO_WARNINGS _ATL_SECURE_NO_WARNINGS
# _MBCS
#DEFINES -= UNICODE _UNICODE

}

HEADERS += \
    zlib.h \
    zutil.h \
    zlib.h \
    zip.h \
    zconf.in.h \
    zconf.h \
    unzip.h \
    trees.h \
    ioapi.h \
    inftrees.h \
    inflate.h \
    inffixed.h \
    inffast.h \
    gzguts.h \
    deflate.h \
    crypt.h \
    crc32.h

SOURCES += \
    zutil.c \
    zip.c \
    unzip.c \
    uncompr.c \
    trees.c \
    minigzip.c \
    ioapi.c \
    inftrees.c \
    inflate.c \
    inffast.c \
    infback.c \
    gzwrite.c \
    gzread.c \
    gzlib.c \
    gzclose.c \
    deflate.c \
    crc32.c \
    compress.c \
    adler32.c

unix {
    target.path = /usr/lib
    INSTALLS += target
}
