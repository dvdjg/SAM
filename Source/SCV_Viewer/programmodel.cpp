#include "programmodel.h"
#include "programitem.h"
#include "statemodel.h"

ProgramModel::ProgramModel(QObject *parent) :
    ListModel(new ProgramItem, parent)
{
}

QObject* ProgramModel::states(const QString &id) const
{
    ProgramItem *item = static_cast<ProgramItem*>(find(id));
    if(item) {
        return item->states();
    }
    return 0;
}
