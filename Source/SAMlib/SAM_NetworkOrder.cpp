#include <QHostAddress>

#include "SAM_NetworkOrder.h"


namespace SAM
{

    unsigned int inet_addr(const QString & addr)
    {
        QHostAddress ip(addr);

        return ntoh(ip.toIPv4Address());
    }

    QString inet_ntoa(unsigned int in)
    {
        unsigned int host = ntoh(in);

        return QHostAddress(host).toString();
    }

}
