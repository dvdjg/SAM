
#include <QtCore/QCoreApplication>
#include <QDir>
#include <QTextCodec>
#include <QTimer>

#include "SAM_Debug.h"
#include "SAM_Log.h"
#include "SAM_MainTask.h"
#include "SAM_Helpers.h"

#if defined(Q_OS_WIN)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <sys/stat.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif


// Ejecutar como sniffer:
// --sniffer=udp://0.0.0.0:7003 --snifferpath=sniffer/messages.sniff.json --ini=nada


/**
 * @brief Establece los codecs de texto a UTF-8 excepto para la salida a consola
 *  ´stderr´ y ´stdout´ para los que selecciona la codificación habitual para el
 *  sistema que es la página CP850 en Windows español.
 */
void setProperCodecs();

using namespace SAM;

int main(int argc, char *argv[])
{
    TRACE(LOCAL);

    QCoreApplication a(argc, argv);

#ifndef Q_OS_WIN
    // Que todos puedan leer los directorios y archivos que se creen a partir de ahora
    umask(0666);
#endif

    setProperCodecs();

    QCoreApplication::setOrganizationName("Indra");
    QCoreApplication::setOrganizationDomain("indra.es");
    QCoreApplication::setApplicationName(SAM_TARGET);
    QCoreApplication::setApplicationVersion(SAM_VERSION);

    // Establece la ruta relativa del proceso a la que corresponda a la imagen en disco
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    // Configurar el manejador de señales: Ctrl+C, ...
    SignalHandler signalHandler(31); // Supervisar todas las señales

    // Tarea emparentada con la aplicación de tal forma que sea borrada por esta.
    g_pMainTask = new MainTask(&a);

    // Conectar el manejador de señales para ejecutarlo cuando llegue una.
    g_pMainTask->connect(&signalHandler, SIGNAL(handleSignal(int)), SLOT(handleSignal(int)));

    // Esto causará que la aplicación salga cuando la tarea señale finished.
    QObject::connect(g_pMainTask, SIGNAL(finished()), &a, SLOT(quit()));

    // Esto ejecuta la tarea desde el bucle de eventos de la aplicación.
    QTimer::singleShot(100, g_pMainTask, SLOT(run()));

    int ret = a.exec();

    LOG_DEBUG(LOCAL) << "Código de salida de la aplicación:" << ret;

    return ret;
}

void setProperCodecs()
{
    QTextCodec * oldCodec = QTextCodec::codecForLocale();

    QTextCodec *utf8Codec = QTextCodec::codecForName("UTF-8");

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    // El código fuente está escrito en UTF-8, se lo comunicacmos a Qt.
    QTextCodec::setCodecForCStrings(utf8Codec);
#endif

    // Forzar que el comportamiento por defecto sea el uso de UTF-8
    QTextCodec::setCodecForLocale(utf8Codec);

    LOG_INFO(LOCAL) << "«Iniciado " << infoString() << "»";

#if defined(Q_OS_WIN)
    CPINFOEXA infoACP, infoOEM;
    memset(&infoACP, 0, sizeof(infoACP));
    memset(&infoOEM, 0, sizeof(infoOEM));

    int cpACP = ::GetACP();
    ::GetCPInfoExA(cpACP, 0, &infoACP);

    int cpOEM = ::GetOEMCP();
    ::GetCPInfoExA(cpOEM, 0, &infoOEM);

    QString strConsoleCP = "CP" + QString::number(infoOEM.CodePage);
    QTextCodec * consoleCodec = QTextCodec::codecForName(qPrintable(strConsoleCP));

    if(consoleCodec)
    {
        // En Windows, usamos la página de códigos de la consola para la salida estándar.
        cout.setCodec(consoleCodec);
        cerr.setCodec(consoleCodec);

        //QTextCodec::setCodecForLocale(consoleCodec);

        LOG_INFO(LOCAL) << "Establecido el códec para los flujos de salida a consola por Qt a"
                        << QString(consoleCodec->name());
    }

    // Windows almacena internamente las cadenas ANSI en Latin1.
    LOG_INFO(LOCAL) << "Windows utiliza el código de página"
                    << QString::fromLatin1(infoACP.CodePageName);
    LOG_INFO(LOCAL) << "La consola utiliza el código de página"
                    << QString::fromLatin1(infoOEM.CodePageName);

    if(utf8Codec != oldCodec){
        LOG_INFO(LOCAL) << "Establecido el códec por defecto de" << QString(oldCodec->name())
                        << "a" << QString(utf8Codec->name());
    }

    // Volver a poner el códec por defecto para la salidas salidas estándar std::cout
    //QTextCodec::setCodecForLocale(oldCodec);
#endif
}


