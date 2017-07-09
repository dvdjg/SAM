#ifndef SAM_PING_H
#define SAM_PING_H

#include <QObject>
#include <QTimer>
#include <QFutureWatcher>
#include <QDateTime>
#include <QVariant>
#include <QMap>
#include <QStringList>

class QString;

namespace SAM
{
/**
 * @brief Supervisa mediante ping un conjunto de anfitriones (interfaces, realmente).
 */
class Ping : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMap watched READ getWatched)

public:

    /**
     * @brief Información del estado de un anfitrión respecto a su `ping`.
     */
    struct HostInfo
    {
        HostInfo() : nIssued(0), nCommited(0), bWatch(true), bLastOk(false) {}

        QString host;       ///< Dirección IP de la interfaz de un anfitrión
        QDateTime lastPing; ///< Última vez que se recibió un ping
        int nIssued;        ///< Cantidad de peticiones de ping que se ha hecho
        int nCommited;      ///< Veces que se ha devuelto que existe el anfitrión
        bool bWatch;        ///< Habilita o deshabilita la supervisión de éste anfitrión
        bool bLastOk;       ///< La última comprobación recibió bien la respuesta del `ping`

        QVariant toVariant() const;
        operator QVariant () const { return toVariant(); }
    };

    typedef QMap<QString, HostInfo> WatchedHosts;

protected:
    QTimer m_timer;
    QFutureWatcher<void> m_watcher;
    WatchedHosts m_watched;

public:

    Ping(QObject * parent = NULL);
    virtual ~Ping();

    /**
     * @brief Comienza la emisión de pings. Si `interval` es mayor de 0, reprograma
     *  una emisión de pings para dentro de `interval` segundos.
     *
     * @param interval
     */
    void start(int interval = 0);

    /**
     * @brief Detiene la emisión de pings si comenzó con una llamada de start() con un
     *  intervalo mayor de 0 segundos.
     */
    void stop();

    /**
     * @brief Supervisa la presencia de `hostname` a nivel de ping.
     *
     * @param hostname
     */
    void watch(const QString & hostname);
    void watch(const QStringList & hostnames);

    /**
     * @brief Elimina a `hostname` de la lista de anfitriones supervisados.
     *
     * @param hostname
     */
    void unWatch(const QString & hostname);
    void unWatch(const QStringList & hostnames);

    /**
     * @brief Limpia la colección de anfitriones supervisados.
     *
     */
    void clear();

    //WatchedHosts & getWatchedHosts() { return m_watched; }
    /**
     * @brief Devuelve la colección de hosts supervisados por ping.
     *
     * @return const WatchedHosts
     */
    const WatchedHosts & getWatchedHosts() const { return m_watched; }
    QVariantMap getWatched() const;

    static void pingInner(HostInfo &info);

    /**
     * @brief Hace ping al anfitrión especificado
     *
     * @param hostname Nombre del anfitrión
     * @param wait Segundos a la espera de la respuesta del ping
     * @param count Número de intentos
     * @return int 0 indica que está "vivo"
     */
    static int ping(const QString & hostname, int wait = 2, int count = 1);

protected slots:
    /**
     * @brief Se recibe cuando han terminado de ejecutarse los pings sobre los anfitriones
     *  en la tanda actual.
     */
    void onFinished();

    /**
     * @brief Ejecuta una tanda de pings para todos los anfitriones activos
     *  configurados mediante Ping::watch().
     */
    void pingHosts();

signals:
    /**
     * @brief Se emite cuando se ha terminado de hacer ping a todas las interfaces.
     *  Para comenzar a hacer la batería de pings, llamara a Ping::start().
     */
    void hostsWatched(int percentageLost);
};
}
#endif // SAM_PING_H
