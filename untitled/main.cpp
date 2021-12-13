#include <QCoreApplication>
#include <QObject>
#include <iostream>

#include "cbase.h"
#include "cone.h"
#include "ctwo.h"

using namespace std;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    cout << "dfgsfg" << endl;

//    cBase b1(nullptr, "1"), b2(nullptr, "2");

//    QObject::connect(&b1, &cBase::SignalFunc,
//                     &b2, &cBase::SlotFunc);

//    b1.SlotFunc();

    cTwo two;

    two.GetOne()->SlotFromOne();


    return 0; // a.exec();
}
