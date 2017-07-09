#ifndef STATEMODEL_H
#define STATEMODEL_H

#include <listmodel.h>

class StateModel : public ListModel
{
    Q_OBJECT
public:
    explicit StateModel(QObject *parent = 0);
};

#endif // STATEMODEL_H
