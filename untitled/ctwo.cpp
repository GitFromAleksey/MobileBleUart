#include "ctwo.h"

cTwo::cTwo(QObject *parent) :
    QObject(parent)
{
    m_one = new cOne();

    QObject::connect(m_one, &cOne::SignalFromOne,
                     this, &cTwo::SlotFromTwo);
    QObject::connect(m_one, &cOne::SignalFromOne,
                     this, &cTwo::SlotFromTwo_2);
}

void cTwo::SlotFromTwo(const QString &message )
{
    std::cout << "Catch emit signal from class cOne. \nMessage: " << message.toStdString() << std::endl;
}

void cTwo::SlotFromTwo_2(const QString &message )
{
    std::cout << "Catch emit signal from class cOne. \nMessage: " << message.toStdString() << std::endl;
}

cOne *cTwo::GetOne()
{
    return m_one;
}
