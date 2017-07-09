#include <QHash>
#include <QByteArray>
#include "statemodel.h"
#include "messageitem.h"
#include "programitem.h"
//#include "qquickitem.h"
static ProgramItem s_programItem(0);

MessageItem::MessageItem(QObject *parent) :
    ListItem(parent), m_source((QObject*)0x0), m_target((QObject*)0x0), m_programFrom(0), m_programTo(0)
{
}

MessageItem::MessageItem(const QString &id, const QString &type, const QString &name, QObject* sender, QObject* receiver, QObject *parent) :
    ListItem(parent), m_id(id), m_type(type), m_name(name), m_source(sender), m_target(receiver), m_programFrom(0), m_programTo(0)
{
    if(!m_source) {
        m_source = (QObject*)0x0;
    }
    if(!m_target) {
        m_target = (QObject*)0x0;
    }
}

QHash<int, QByteArray> MessageItem::roleNames() const
{
    QHash<int, QByteArray> names;
    names[IdRole]   = "id";
    names[NameRole] = "name";
    names[TypeRole] = "type";
    names[SourceRole] = "source";
    names[TargetRole] = "target";
    names[ProgramFromRole] = "programFrom";
    names[ProgramToRole] = "programTo";

    return names;
}

QVariant MessageItem::data(int role) const
{
    QVariant ret;
    switch(role) {
    case IdRole:
    case Qt::DisplayRole:
        ret = id();
        break;
    case TypeRole:
        ret = type();
        break;
    case NameRole:
        ret = name();
        break;
    case SourceRole:
        ret = QVariant::fromValue(source());
        break;
    case TargetRole:
        ret = QVariant::fromValue(target());
        break;
    case ProgramFromRole:
        ret = QVariant::fromValue((QObject *)programFrom());
        break;
    case ProgramToRole:
        ret = QVariant::fromValue((QObject *)programTo());
        break;
    default:
        ;
    }
    return ret;
}

bool MessageItem::setData(const QVariant &value, int role)
{
    switch(role) {
    case IdRole:
    case Qt::EditRole:
        setid(value.toString());
        break;
    case TypeRole:
        settype(value.toString());
        break;
    case NameRole:
        setname(value.toString());
        break;
    case SourceRole:
        setsource(value.value<QObject *>());
        break;
    case TargetRole:
        settarget(value.value<QObject *>());
        break;
    case ProgramFromRole:
        setprogramFrom(value.value<ProgramItem *>());
        break;
    case ProgramToRole:
        setprogramTo(value.value<ProgramItem *>());
        break;
    default:
        return false;
    }
    return false;
}

