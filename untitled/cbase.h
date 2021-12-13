#ifndef CBASE_H
#define CBASE_H

#include <QObject>
#include <QString>
#include <iostream>

class cBase : public QObject
{
    Q_OBJECT
public:
    explicit cBase(QObject *parent = nullptr, const QString &name = "");

signals:
    void SignalFunc();

public slots:
    void SlotFunc();

private:
    QString m_name;
};

#endif // CBASE_H
//class _cBase : public QObject
//{
//    Q_OBJECT
//public:
//    _cBase(){}

//signals:
//    void SignalFunc();

//public slots:
//    void SlotFunc()
//    {
//        emit SignalFunc();
//    }

//private:

//};
