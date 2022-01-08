#ifndef BLEDATASTRUCTURES_H
#define BLEDATASTRUCTURES_H

#include <QObject>

struct CharacteristicDescription
{
    QString uuid;
    QList<QString> property_type;
};

struct ServiceDescription
{
    QString name;
    QString uuid;
    QList<CharacteristicDescription> characteristics;
};


#endif // BLEDATASTRUCTURES_H
