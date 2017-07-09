#ifndef INTERFACEMODEL_H
#define INTERFACEMODEL_H

#include <listmodel.h>

class InterfaceModel : public ListModel
{
    Q_OBJECT
public:
    explicit InterfaceModel(QObject *parent = 0);
    
    Q_INVOKABLE QObject* programs(const QString &id) const;
    Q_INVOKABLE QObject* interface(const QString &id) const;

};

#endif // INTERFACEMODEL_H
