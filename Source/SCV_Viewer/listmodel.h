/*
 * Author: Christophe Dumez <dchris@gmail.com>
 * License: Public domain (No attribution required)
 * Website: http://cdumez.blogspot.com/
 * Version: 1.0
 */

#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QHash>
#include <QByteArray>

/**
 * @brief The ListItem class
 */
class ListItem: public QObject {
    Q_OBJECT

public:
    ListItem(QObject* parent = 0) : QObject(parent) {}
    virtual ~ListItem() {}
    virtual QString id() const = 0;
    virtual QVariant data(int role) const = 0;
    virtual bool setData(const QVariant &value, int role) { Q_UNUSED(value); Q_UNUSED(role); return false; }
    virtual QHash<int, QByteArray> roleNames() const = 0;

signals:
    void dataChanged(int role = -1);
};

/**
 * @brief The ListModel class
 */
class ListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ListModel(ListItem* prototype, QObject* parent = 0);
    ~ListModel();
    virtual QHash<int,QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    void appendRow(ListItem* item);
    void appendRows(const QList<ListItem*> &items);
    void insertRow(int row, ListItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    ListItem* row(int r) const;
    ListItem* find(const QString &id) const;
    QModelIndex indexFromItem( const ListItem* item) const;
    void clear();

    Q_INVOKABLE QObject* at(int r) const;

private slots:
    void handleItemChange(int role = -1);

private:
    ListItem* m_prototype;
    QList<ListItem*> m_list;
    //QHash<ListItem*> m_hash;
};

#endif // LISTMODEL_H
