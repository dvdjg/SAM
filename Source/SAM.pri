
# win32:SVN_VERSION = $$system(svn info -r HEAD . | findstr Revis)
# unix:SVN_VERSION = $$system(svn info -r HEAD . | grep Revis)

win32-msvc*:COMPILER_ID=$$system(cl 2>&1 | findstr C++)
else:win32-g*:COMPILER_ID=$$system(g++ --version | findstr g++)
else:linux-g*:COMPILER_ID=$$system(g++ --version | grep g++)
else:COMPILER_ID=Desconocido
COMPILER_ID='\\\"$${COMPILER_ID}\\\"'

SVN_VERSION = $$system(svnversion -n)

CONFIG(debug, debug|release) {
    POST=d
    BUILD_POSTFIX=Debug
} else {
    POST=
    BUILD_POSTFIX=Release
}

win32*:ARCH= Win32
unix:ARCH= Unix

unix {

INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/include/c++/4.1.1

}

win32 {

DEFINES += _WIN32_WINNT=0x0501
DEFINES += \
    _SCL_SECURE_NO_DEPRECATE \
    _CRT_SECURE_NO_DEPRECATE \
    _CRT_SECURE_NO_WARNINGS \
    _CRT_NONSTDC_NO_DEPRECATE \
    _AFX_SECURE_NO_WARNINGS \
    _ATL_SECURE_NO_WARNINGS \
    _USE_32BIT_TIME_T

}
