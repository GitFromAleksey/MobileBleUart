#include "cone.h"

cOne::cOne(QObject *parent) : QObject(parent)
{

}

void cOne::SlotFromOne()
{
    m_message = "message from one.";
    emit SignalFromOne(m_message);
}
