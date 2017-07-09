#include <QString>
#include <QProcess>
#include <QtConcurrentMap>
#include <QFutureWatcher>

#include "SAM_Log.h"
#include "SAM_Ping.h"

namespace SAM
{
Ping::Ping(QObject *parent) : QObject(parent)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(pingHosts()));
    connect(&m_watcher, SIGNAL(finished()), this, SLOT(onFinished()));
}

Ping::~Ping()
{
}

void Ping::pingHosts()
{
    // Ejecuta en paralelo los pings para todos los anfitriones supervisados
    QFuture<void> future = QtConcurrent::map(m_watched, Ping::pingInner);
    m_watcher.setFuture(future);
}

void Ping::start(int interval)
{ 
    if(interval > 0) {
        m_timer.start(interval*1000);
    } else {
        m_timer.stop();
    }

    pingHosts();
}

void Ping::stop()
{
    m_timer.stop();
}

void Ping::watch(const QString & hostname)
{
    HostInfo & hostInfo = m_watched[hostname];
    hostInfo.host = hostname;
    hostInfo.bWatch = true;
}

void Ping::watch(const QStringList & hostnames)
{
    foreach(QString name, hostnames) {
        watch(name);
    }
}

void Ping::unWatch(const QString & hostname)
{
    m_watched.remove(hostname);
}

void Ping::unWatch(const QStringList & hostnames)
{
    foreach(QString name, hostnames) {
        unWatch(name);
    }
}

void Ping::clear()
{
    m_watched.clear();
}

void Ping::onFinished()
{
    int percentageLost = 0;
    int percentageLostCount = 0;

    foreach(const HostInfo & hostInfo, m_watched) {
        if(!hostInfo.bLastOk) {
            ++percentageLostCount;
        }
    }

    if(!m_watched.isEmpty())
    {
        percentageLost = (100 * percentageLostCount) / m_watched.count();

        LOG_DEBUG(COMS) << "Pérdida total de pings:" << percentageLost << "% ("
                        << percentageLostCount << "de" << m_watched.count() << "anfitriones han fallado).";
    }

    emit hostsWatched(percentageLost);
}

void Ping::pingInner(HostInfo & info)
{
    if(info.bWatch)
    {
        qint64 timeDiff = QDateTime::currentDateTime().toMSecsSinceEpoch()
                - info.lastPing.toMSecsSinceEpoch();

        if(timeDiff > 10)
        {
            ++info.nIssued;

            int ret = ping(info.host, 1);

            info.bLastOk = (ret == 0);

            if(info.bLastOk) {
                ++info.nCommited;
                info.lastPing = QDateTime::currentDateTime();
            }

            LOG_DEBUG(COMS) << "Ping a" >> info.host << "=" << (ret == 0);
        }
    }
}

int Ping::ping(const QString &hostname, int wait, int count)
{
    TRACE(COMS);

#ifdef Q_OS_WIN
    // En Windows también se podría usar la función `IcmpSendEcho2()`
    // http://msdn.microsoft.com/en-us/library/aa366051%28VS.85%29.aspx
    // How to perform a simple ping with qt classes?
    // http://lists.trolltech.com/qt-interest/2002-10/thread00351-0.html
    QString command(QString("ping -w %1 -n %2 %3").arg(wait).arg(count).arg(hostname));
#else
    // En Linux sería necesario tener privilegios de `root` para poder realizar
    // un ping, por lo que es mucho más cómodo usar la orden `ping`.
    QString command(QString("ping -w%1 -c%2 %3").arg(wait).arg(count).arg(hostname));
#endif
    // Llamada síncrona a 'ping'
    int exitCode = QProcess::execute(command);

    if(exitCode == -2) {
        LOG_WARN(COMS) << "No se puede ejecutar la orden" >> command << ".";
    } else if(exitCode == -1) {
        LOG_WARN(COMS) << "La orden" >> command << "ha fallado.";
    }

    return exitCode;
}

QVariantMap Ping::getWatched() const
{
    QVariantMap map;
    foreach(const HostInfo & hostInfo, m_watched) {
        map[hostInfo.host] = hostInfo;
    }

    return map;
}

QVariant Ping::HostInfo::toVariant() const
{
    QVariantMap map;
    map["host"] = host;
    map["lastPing"] = lastPing;
    map["nIssued"] = nIssued;
    map["nCommited"] = nCommited;
    map["bWatch"] = bWatch;
    map["bLastOk"] = bLastOk;

    return map;
}

}
