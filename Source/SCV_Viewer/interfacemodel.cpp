#include "interfacemodel.h"
#include "interfaceitem.h"
#include "programmodel.h"

InterfaceModel::InterfaceModel(QObject *parent) :
    ListModel(new InterfaceItem, parent)
{
}

QObject* InterfaceModel::programs(const QString &id) const
{
    InterfaceItem *item = static_cast<InterfaceItem*>(find(id));
    if(item) {
        return item->programs();
    }
    return 0;
}

QObject* InterfaceModel::interface(const QString &id) const
{
    InterfaceItem *item = static_cast<InterfaceItem*>(find(id));

    return item;
}


