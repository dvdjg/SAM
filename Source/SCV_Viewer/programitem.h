#ifndef PROGRAMITEM_H
#define PROGRAMITEM_H

#include <listmodel.h>

class StateModel;

class ProgramItem : public ListItem
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE setid NOTIFY nameChanged)
    Q_PROPERTY(QString name READ name WRITE setname NOTIFY nameChanged)
    Q_PROPERTY(QString turned READ turned WRITE setturned NOTIFY turnedChanged)
    Q_PROPERTY(QString user READ user WRITE setuser NOTIFY userChanged)
    Q_PROPERTY(QString type READ type WRITE settype NOTIFY typeChanged)
    Q_PROPERTY(QObject* lastMessageSent READ lastMessageSent WRITE setlastMessageSent NOTIFY lastMessageSentChanged)
    Q_PROPERTY(QObject* lastMessageReceived READ lastMessageReceived WRITE setlastMessageReceived NOTIFY lastMessageReceivedChanged)

public:
    ProgramItem(QObject *parent = 0);
    explicit ProgramItem(const QString &type, const QString &name, const QString &user, const QString &turned, QObject *parent = 0);

    enum Roles {
        IdRole = Qt::DisplayRole,
        NameRole = Qt::UserRole+1,
        UserRole,
        TypeRole,
        TurnedRole,
        LastMessageSentRole,
        LastMessageReceivedRole,
        StatesRole
    };

public:
    QVariant data(int role) const;
    virtual QHash<int, QByteArray> roleNames() const;


    StateModel * states() const { return m_stateModel; }

    QString id() const
    {
        return name();
    }

    QString name() const
    {
        return m_name;
    }

    QString turned() const
    {
        return m_turned;
    }

    QString user() const
    {
        return m_user;
    }

    QString type() const
    {
        return m_type;
    }

    QObject* lastMessageSent() const
    {
        return m_lastMessageSent;
    }

    QObject* lastMessageReceived() const
    {
        return m_lastMessageReceived;
    }

public slots:
    void setid(QString arg)
    {
        setname(arg);
    }

    void setname(QString arg)
    {
        if (m_name != arg) {
            m_name = arg;
            emit nameChanged(arg);
            emit dataChanged(IdRole);
            emit dataChanged(NameRole);
        }
    }

    void setturned(QString arg)
    {
        if (m_turned != arg) {
            m_turned = arg;
            emit turnedChanged(arg);
            emit dataChanged(TurnedRole);
        }
    }

    void setuser(QString arg)
    {
        if (m_user != arg) {
            m_user = arg;
            emit userChanged(arg);
            emit dataChanged(UserRole);
        }
    }

    void settype(QString arg)
    {
        if (m_type != arg) {
            m_type = arg;
            emit typeChanged(arg);
            emit dataChanged(TypeRole);
        }
    }

    void setlastMessageSent(QObject* arg, bool forceSignal = false)
    {
        if (m_lastMessageSent != arg || forceSignal) {
            m_lastMessageSent = arg;
            emit lastMessageSentChanged(arg);
            emit dataChanged(LastMessageSentRole);
        }
    }

    void setlastMessageReceived(QObject* arg, bool forceSignal = false)
    {
        if (m_lastMessageReceived != arg || forceSignal) {
            m_lastMessageReceived = arg;
            emit lastMessageReceivedChanged(arg);
            emit dataChanged(LastMessageReceivedRole);
        }
    }

signals:
    void nameChanged(QString arg);
    void turnedChanged(QString arg);
    void userChanged(QString arg);
    void typeChanged(QString arg);
    void lastMessageSentChanged(QObject* arg);
    void lastMessageReceivedChanged(QObject* arg);

private:
    QString m_type;
    QString m_name;
    QString m_user;
    QString m_turned;
    QObject* m_lastMessageSent;
    QObject* m_lastMessageReceived;

    StateModel * m_stateModel;
};

#endif // PROGRAMITEM_H
