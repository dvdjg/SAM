#ifndef PROGRAMMODEL_H
#define PROGRAMMODEL_H

#include <listmodel.h>
class ProgramItem;

class ProgramModel : public ListModel
{
    Q_OBJECT
public:
    explicit ProgramModel(QObject *parent = 0);
    
    ProgramItem * find(const QString &id) const
    { return (ProgramItem*) ListModel::find(id); }

    Q_INVOKABLE QObject* states(const QString &id) const;
};

#endif // PROGRAMMODEL_H
