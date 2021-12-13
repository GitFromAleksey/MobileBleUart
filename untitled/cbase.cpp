#include "cbase.h"

cBase::cBase(QObject *parent, const QString &name) :
    QObject(parent),
    m_name(name)
{

}

void cBase::SlotFunc()
{
    std::cout << m_name.toStdString() << std::endl;
    emit SignalFunc();
}
