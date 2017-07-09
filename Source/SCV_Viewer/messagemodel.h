#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include <listmodel.h>
class MessageItem;

class MessageModel : public ListModel
{
    Q_OBJECT

    Q_PROPERTY(unsigned int nextid READ nextid WRITE setnextid NOTIFY nextidChanged)
    unsigned int m_nextid;

public:
    explicit MessageModel(QObject *parent = 0);
    
    MessageItem * find(const QString &id) const
    { return (MessageItem*) ListModel::find(id); }

    //Q_INVOKABLE QObject* states(const QString &id) const;
    unsigned int nextid() const
    {
        return m_nextid;
    }


public slots:
    void setnextid(unsigned int arg)
    {
        if (m_nextid != arg) {
            m_nextid = arg;
            emit nextidChanged(arg);
        }
    }

signals:
    void nextidChanged(unsigned int arg);
};

#endif // MESSAGEMODEL_H
