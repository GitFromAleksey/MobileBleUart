QT       += core gui
QT += bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Ble/cblecontroller.cpp \
    Ble/cbledev.cpp \
    Ble/cbledevice_old.cpp \
    Ble/cblediscoverydevicesagent.cpp \
    Ble/cbleservice.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Ble/BleDataStructures.h \
    Ble/cblecontroller.h \
    Ble/cbledev.h \
    Ble/cbledevice_old.h \
    Ble/cblediscoverydevicesagent.h \
    Ble/cbleservice.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
