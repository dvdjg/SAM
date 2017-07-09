#include <QHash>
#include <QByteArray>
#include "interfaceitem.h"
#include "statemodel.h"
#include "programmodel.h"

InterfaceItem::InterfaceItem(QObject *parent) :
    ListItem(parent), m_stateModel(new StateModel(this)), m_programModel(new ProgramModel(this))
{
}

InterfaceItem::InterfaceItem(const QString &ip, const QString &name, const QString &turned, QObject *parent) :
    ListItem(parent), m_ip(ip), m_name(name), m_turned(turned),
    m_stateModel(new StateModel(this)), m_programModel(new ProgramModel(this))
{
}

QHash<int, QByteArray> InterfaceItem::roleNames() const
{
    QHash<int, QByteArray> names;
    names[IdRole] = "id";
    names[NameRole] = "name";
    names[IpRole] = "ip";
    names[TurnedRole] = "turned";
    names[ProgramsRole] = "programs";
    names[StatesRole] = "props";
    return names;
}

QVariant InterfaceItem::data(int role) const
{
    QVariant ret;
    ProgramModel * programModel = 0;
    StateModel * stateModel = 0;

    switch(role)
    {
    case IdRole:
        ret = ip();
        break;
    case NameRole:
        ret = name();
        break;
    case IpRole:
        return ip();
        break;
    case TurnedRole:
        return turned();
        break;
    case StatesRole:
        stateModel = states();
        if(stateModel) {
            ret = QVariant::fromValue(stateModel);
        }
        break;
    case ProgramsRole:
        programModel = programs();
        if(programModel) {
            ret = QVariant::fromValue(programModel);
        }
        break;
    default:
        ;
    }

    return ret;
}

