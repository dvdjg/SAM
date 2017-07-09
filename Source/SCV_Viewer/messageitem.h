#ifndef MESSAGEITEM_H
#define MESSAGEITEM_H

#include <listmodel.h>

class StateModel;
class ProgramItem;

class MessageItem : public ListItem
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE setid NOTIFY idChanged)
    Q_PROPERTY(QString type READ type WRITE settype NOTIFY typeChanged)
    Q_PROPERTY(QString name READ name WRITE setname NOTIFY nameChanged)
    Q_PROPERTY(QObject* source READ source WRITE setsource NOTIFY sourceChanged)
    Q_PROPERTY(QObject* target READ target WRITE settarget NOTIFY targetChanged)
    Q_PROPERTY(ProgramItem* programFrom READ programFrom WRITE setprogramFrom NOTIFY programFromChanged)
    Q_PROPERTY(ProgramItem* programTo READ programTo WRITE setprogramTo NOTIFY programToChanged)

public:
    MessageItem(QObject *parent = 0);
    explicit MessageItem(const QString &id, const QString &type, const QString &name, QObject *source = 0, QObject *target = 0, QObject *parent = 0);

    enum Roles {
        IdRole = Qt::UserRole+1,
        NameRole,
        TypeRole,
        SourceRole,
        TargetRole,
        ProgramFromRole,
        ProgramToRole
    };

public:
    QVariant data(int role) const;
    bool setData(const QVariant &value, int role);
    virtual QHash<int, QByteArray> roleNames() const;

    QString name() const
    {
        return m_name;
    }

    QString type() const
    {
        return m_type;
    }

    QString id() const
    {
        return m_id;
    }

    QObject* source() const
    {
        return m_source;
    }

    QObject* target() const
    {
        return m_target;
    }

    ProgramItem* programFrom() const
    {
        return m_programFrom;
    }

    ProgramItem* programTo() const
    {
        return m_programTo;
    }

public slots:

    void setname(QString arg)
    {
        if (m_name != arg) {
            m_name = arg;
            emit nameChanged(arg);
            emit dataChanged(NameRole);
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

    void setid(QString arg)
    {
        if (m_id != arg) {
            m_id = arg;
            emit idChanged(arg);
            emit dataChanged(IdRole);
        }
    }

    void setsource(QObject* arg)
    {
        if (m_source != arg) {
            m_source = arg;
            emit sourceChanged(arg);
            emit dataChanged(SourceRole);
        }
    }

    void settarget(QObject* arg)
    {
        if (m_target != arg) {
            m_target = arg;
            emit targetChanged(arg);
            emit dataChanged(TargetRole);
        }
    }

    void setprogramFrom(ProgramItem* arg, bool forceSignal = false)
    {
        if (m_programFrom != arg || forceSignal) {
            m_programFrom = arg;
            emit programFromChanged(arg);
            emit dataChanged(ProgramFromRole);
        }
    }

    void setprogramTo(ProgramItem* arg, bool forceSignal = false)
    {
        if (m_programTo != arg || forceSignal) {
            m_programTo = arg;
            emit programToChanged(arg);
            emit dataChanged(ProgramToRole);
        }
    }

signals:
    void nameChanged(QString arg);
    void typeChanged(QString arg);
    void idChanged(QString arg);
    void sourceChanged(QObject* arg);
    void targetChanged(QObject* arg);
    void programFromChanged(ProgramItem* arg);

    void programToChanged(ProgramItem* arg);

private:
    QString m_id;
    QString m_type;
    QString m_name;
    QObject* m_source;
    QObject* m_target;
    ProgramItem* m_programFrom;
    ProgramItem* m_programTo;
};

#endif // MESSAGEITEM_H
