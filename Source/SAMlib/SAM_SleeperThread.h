#ifndef SAM_SLEEPERTHREAD_H
#define SAM_SLEEPERTHREAD_H

#include <QThread>

namespace SAM
{
    class SleeperThread : public QThread
    {
    public:
        static void msleep(unsigned long msecs)
        {
            QThread::msleep(msecs);
        }
    };
}

#endif // SAM_SLEEPERTHREAD_H
