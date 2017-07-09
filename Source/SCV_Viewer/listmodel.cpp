/*
 * Author: Christophe Dumez <dchris@gmail.com>
 * License: Public domain (No attribution required)
 * Website: http://cdumez.blogspot.com/
 * Version: 1.0
 */

#include "listmodel.h"

ListModel::ListModel(ListItem* prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
}

QHash<int,QByteArray> ListModel::roleNames() const
{
    return m_prototype->roleNames();
}

int ListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.size();
}

QVariant ListModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}

bool ListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return false;
    return m_list.at(index.row())->setData(value, role);
}

ListModel::~ListModel() {
    delete m_prototype;
    clear();
}

void ListModel::appendRow(ListItem *item)
{
    appendRows(QList<ListItem*>() << item);
}

void ListModel::appendRows(const QList<ListItem *> &items)
{
    int rCount = rowCount();
    beginInsertRows(QModelIndex(), rCount, rCount+items.size()-1);
    foreach(ListItem *item, items) {
        if(item) {
            item->setParent(this);
            connect(item, SIGNAL(dataChanged(int)), SLOT(handleItemChange(int)));
            m_list.append(item);
        }
    }
    endInsertRows();
}

void ListModel::insertRow(int row, ListItem *item)
{
    if(item) {
        item->setParent(this);
        beginInsertRows(QModelIndex(), row, row);
        connect(item, SIGNAL(dataChanged(int)), SLOT(handleItemChange(int)));
        m_list.insert(row, item);
        endInsertRows();
    }
}

void ListModel::handleItemChange(int role)
{
    ListItem* item = static_cast<ListItem*>(sender());
    QModelIndex index = indexFromItem(item);
    if(index.isValid()) {
        if(role >= 0) {
            emit dataChanged(index, index, QVector<int>() << role);
        } else {
            emit dataChanged(index, index);
        }
    }
}

ListItem * ListModel::find(const QString &id) const
{
    foreach(ListItem* item, m_list) {
        if(item->id() == id) {
            return item;
        }
    }
    return 0;
}

QModelIndex ListModel::indexFromItem(const ListItem *item) const
{
    Q_ASSERT(item);
    for(int row=0; row<m_list.size(); ++row) {
        if(m_list.at(row) == item) return index(row);
    }
    return QModelIndex();
}

void ListModel::clear()
{
    qDeleteAll(m_list);
    m_list.clear();
}

bool ListModel::removeRow(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row);
    delete m_list.takeAt(row);
    endRemoveRows();
    return true;
}

bool ListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if(row < 0 || (row+count) >= m_list.size()) return false;
    beginRemoveRows(QModelIndex(), row, row+count-1);
    for(int i=0; i<count; ++i) {
        delete m_list.takeAt(row);
    }
    endRemoveRows();
    return true;
}

ListItem * ListModel::row(int r) const
{
    ListItem* item = m_list.at(r);
    return item;
}

QObject* ListModel::at(int r) const
{
    return (QObject*) row(r);
}
