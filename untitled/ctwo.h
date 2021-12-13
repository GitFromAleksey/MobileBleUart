#ifndef CTWO_H
#define CTWO_H

#include <QObject>
#include <iostream>
#include <cone.h>

class cTwo : public QObject
{
    Q_OBJECT
public:
    explicit cTwo(QObject *parent = nullptr);

    cOne *GetOne();

signals:

public slots:
    void SlotFromTwo(const QString &message );
    void SlotFromTwo_2(const QString &message );

private:
    cOne *m_one;
};

#endif // CTWO_H


