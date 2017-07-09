#include "statemodel.h"
#include "stateitem.h"

StateModel::StateModel(QObject *parent) :
    ListModel(new StateItem, parent)
{
}

