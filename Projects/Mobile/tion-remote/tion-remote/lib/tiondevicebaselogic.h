#ifndef TIONDEVICEBASELOGIC_H
#define TIONDEVICEBASELOGIC_H

#include <QWidget>
#include <QObject>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QBluetoothAddress>
#include <lib/firmware.h>
#include <lib/mabaseprotocol.h>

class TionDeviceBaseLogic : public QWidget
{
public:
    TionDeviceBaseLogic(QWidget *parent);

public:
      void sendOverBt(MABaseProtocol::RawDataList rw);
      virtual MABaseProtocol *getMaRc() = 0;
      void setDevice(const QString &name,
                                  const QBluetoothAddress &addr);
      void showUI();
protected:
    QLowEnergyController* control;
    QLowEnergyService *service;
    QLowEnergyDescriptor notificationDesc;
    bool foundMaService;
    QString devName;
    QTimer *timer;
    QTimer *timeout_timer;
    bool inUpdate;
    bool inConnect;
    Firmware* firmware;

    static const QString maServiceUuid;
    static const QString maServiceCharTxUuid;
    static const QString maServiceCharRxUuid;
    static const QString maServiceNotifyDescUuid;
    static const QString defaultFirmDir;


    void serviceError(QLowEnergyService::ServiceError newError);
signals:
    void discButton();

protected slots:
    virtual void updateDeviceParams(const QLowEnergyCharacteristic &c,
                                    const QByteArray &value) = 0;
    void serviceDiscovered(const QBluetoothUuid &gatt);
    void serviceScanDone();
    void serviceStateChanged(QLowEnergyService::ServiceState s);
    void startPolling();
    void UpdateHandler();
    virtual void sendRequest() = 0;

};

#endif // TIONDEVICEBASELOGIC_H
