#-------------------------------------------------
#
# Project created by QtCreator 2018-05-13T15:37:02
#
#-------------------------------------------------

QT       += core gui bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tion-remote
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    IQ/iq_autoindication.cpp \
    IQ/iq_mainwindow.cpp\
    IQ/iq_protocol.cpp\
    IQ/iq_service.cpp\
    br4s/br4s_mainwindow.cpp \
    br4s/br4s_protocol.cpp \
    br4s/br4s_service.cpp \
    lib/mabaseprotocol.cpp \
    lib/tiondevicebaselogic.cpp \
        main.cpp \
        deviceselector.cpp \
    lib/maupdater.cpp \
    brlt/remotecontroller.cpp \
    basewindow.cpp \
    brlt/maprotocol.cpp \
    brlt/configurator.cpp \
    lib/firmware.cpp \
        br4s/timer_service.cpp \
    br4s/timer_tab.cpp

HEADERS += \
    IQ/iq_autoindication.h \
    IQ/iq_mainwindow.h\
    IQ/iq_protocol.h\
    IQ/iq_service.h\
    br4s/br4s_mainwindow.h \
    br4s/br4s_protocol.h \
    br4s/br4s_service.h \
        deviceselector.h \
    lib/mabaseprotocol.h \
    lib/maupdater.h \
    brlt/remotecontroller.h \
    basewindow.h \
    brlt/maprotocol.h \
    lib/crc_calculate.h \
    brlt/configurator.h \
    lib/test_firm_array.h \
    lib/firmware.h \
        br4s/timer_service.h \
    br4s/timer_tab.h \
    lib/tiondevicebaselogic.h

FORMS += \
    IQ/iq_autoindication.ui \
    IQ/iq_mainwindow.ui\
    br4s/br4s_mainwindow.ui \
    br4s/br4s_service.ui \
    IQ/iq_service.ui \
        deviceselector.ui \
    brlt/remotecontroller.ui \
    basewindow.ui \
    brlt/configurator.ui \
        br4s/timer_service.ui \
    br4s/timer_tab.ui

CONFIG += mobility
MOBILITY = 

ANDROID_ABIS = armeabi-v7a

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

