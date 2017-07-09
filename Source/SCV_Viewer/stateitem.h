#ifndef STATEITEM_H
#define STATEITEM_H

#include <QDateTime>
#include "listmodel.h"

class StateItem : public ListItem
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE setid NOTIFY nameChanged)
    Q_PROPERTY(QString name READ name WRITE setname NOTIFY nameChanged)
    Q_PROPERTY(QVariant value READ value WRITE setvalue NOTIFY valueChanged)
    //Q_PROPERTY(date READ date WRITE setdate NOTIFY dateChanged);
public:
    StateItem(QObject *parent = 0);
    explicit StateItem(const QString &name, const QVariant &value, QObject *parent = 0);

    enum Roles {
        IdRole = Qt::DisplayRole,
        NameRole = Qt::UserRole+1,
        ValueRole
    };

public:
    QVariant data(int role) const;
    virtual QHash<int, QByteArray> roleNames() const;

    QString name() const
    {
        return m_name;
    }

    QVariant value() const
    {
        return m_value;
    }

    QString id() const
    {
        return m_name;
    }

public slots:
    void setname(QString arg)
    {
        if (m_name != arg) {
            m_name = arg;
            emit nameChanged(arg);
            emit dataChanged(NameRole);
            emit dataChanged(IdRole);
        }
    }

    void setvalue(QVariant arg)
    {
        if (m_value != arg) {
            m_value = arg;
            emit valueChanged(arg);
            emit dataChanged(ValueRole);
        }
    }

    void setid(QString arg)
    {
        setname(arg);
    }

signals:
    void nameChanged(QString arg);
    void valueChanged(QVariant arg);

private:
    QString m_name;
    QVariant m_value;
};

#endif // STATEITEM_H
