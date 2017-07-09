#ifndef INTERFACEITEM_H
#define INTERFACEITEM_H

#include <listmodel.h>
class ProgramModel;
class StateModel;

class InterfaceItem : public ListItem
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE setip NOTIFY nameChanged)
    Q_PROPERTY(QString ip READ ip WRITE setip NOTIFY ipChanged)
    Q_PROPERTY(QString name READ name WRITE setname NOTIFY nameChanged)
    Q_PROPERTY(QString turned READ turned WRITE setturned NOTIFY turnedChanged)

public:
    InterfaceItem(QObject *parent = 0);
    explicit InterfaceItem(const QString &ip, const QString &name, const QString &turned, QObject *parent = 0);

    enum Roles {
        IdRole = Qt::DisplayRole,
        NameRole = Qt::UserRole+1,
        IpRole,
        TurnedRole,
        ProgramsRole,
        StatesRole
    };

public:
    virtual QHash<int, QByteArray> roleNames() const;
    QVariant data(int role) const;
    //bool setData(const QVariant & value, int role = Qt::EditRole);

    QString id() const { return ip(); }
    QString ip() const { return m_ip; }
    QString name() const { return m_name; }
    QString turned() const { return m_turned; }
    ProgramModel * programs() const { return m_programModel; }
    StateModel * states() const { return m_stateModel; }

public slots:
    void setid(QString arg)
    {
        setip(arg);
    }

    void setip(QString arg)
    {
        if (m_ip != arg) {
            m_ip = arg;
            emit ipChanged(arg);
            emit dataChanged(IpRole);
            emit dataChanged(IdRole);
        }
    }

    void setname(QString arg)
    {
        if (m_name != arg) {
            m_name = arg;
            emit nameChanged(arg);
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

signals:
    void ipChanged(QString arg);
    void nameChanged(QString arg);
    void turnedChanged(QString arg);

private:
    QString m_ip;
    QString m_name;
    QString m_turned;

    StateModel * m_stateModel;
    ProgramModel * m_programModel;
};

#endif // INTERFACEITEM_H
