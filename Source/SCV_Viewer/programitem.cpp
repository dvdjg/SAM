#include <QHash>
#include <QByteArray>
#include "statemodel.h"
#include "messageitem.h"
#include "programitem.h"

static MessageItem s_messageItem(0);

ProgramItem::ProgramItem(QObject *parent) :
    ListItem(parent),
    m_lastMessageSent((QObject*)(&s_messageItem)),
    m_lastMessageReceived((QObject*)(&s_messageItem)),
    m_stateModel(new StateModel(this))
{
}

ProgramItem::ProgramItem(const QString &type, const QString &name, const QString &user, const QString &turned, QObject *parent) :
    ListItem(parent), m_type(type), m_name(name), m_user(user), m_turned(turned),
    m_lastMessageSent((QObject*)(&s_messageItem)),
    m_lastMessageReceived((QObject*)(&s_messageItem)),
    m_stateModel(new StateModel(this))
{

}

QHash<int, QByteArray> ProgramItem::roleNames() const
{
    QHash<int, QByteArray> names;
    names[IdRole]   = "id";
    names[NameRole] = "name";
    names[UserRole] = "user";
    names[TypeRole] = "type";
    names[TurnedRole] = "turned";
    names[LastMessageSentRole] = "lastMessageSent";
    names[LastMessageReceivedRole] = "lastMessageReceived";
    names[StatesRole] = "props";

    return names;
}

QVariant ProgramItem::data(int role) const
{
    QVariant ret;
    switch(role) {
    case IdRole:
        ret = id();
        break;
    case NameRole:
        ret = name();
        break;
    case UserRole:
        ret = user();
        break;
    case TypeRole:
        ret = type();
        break;
    case TurnedRole:
        ret = turned();
        break;
    case LastMessageSentRole:
        ret = QVariant::fromValue(lastMessageSent());
        break;
    case LastMessageReceivedRole:
        ret = QVariant::fromValue(lastMessageReceived());
        break;
    case StatesRole:
    {
        StateModel * stateModel = states();
        if(stateModel) {
            ret = QVariant::fromValue(stateModel);
        }
        break;
    }
    default:
        ;
    }
    return ret;
}

