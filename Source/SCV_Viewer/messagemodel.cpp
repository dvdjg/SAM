#include "messagemodel.h"
#include "messageitem.h"
#include "statemodel.h"

MessageModel::MessageModel(QObject *parent) :
    ListModel(new MessageItem, parent), m_nextid(0)
{
}


