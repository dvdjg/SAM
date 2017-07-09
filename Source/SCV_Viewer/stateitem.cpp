#include <QHash>
#include <QByteArray>
#include "stateitem.h"

StateItem::StateItem(QObject *parent) :
    ListItem(parent)
{
}

StateItem::StateItem(const QString &name, const QVariant &value, QObject *parent) :
    ListItem(parent), m_name(name), m_value(value)
{
}

QHash<int, QByteArray> StateItem::roleNames() const
{
    QHash<int, QByteArray> names;
    names[IdRole]    = "id";
    names[NameRole]  = "name";
    names[ValueRole] = "value";
    return names;
}

QVariant StateItem::data(int role) const
{
    QVariant ret;
    switch(role) {
    case IdRole:
        ret = id();
        break;
    case NameRole:
        ret = name();
        break;
    case ValueRole:
        ret = value();
        break;
    default:
        ;
    }
    return ret;
}


