#ifndef DEVICESELECTOR_H
#define DEVICESELECTOR_H

#include <QWidget>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QListWidgetItem>
#include <QBluetoothLocalDevice>
#include "brlt/remotecontroller.h"
#include "br4s/br4s_mainwindow.h"
#include "IQ/iq_mainwindow.h"
#include <QList>
#include <QString>
#include <QSettings>

namespace Ui {
class DeviceSelector;
}

class DeviceSelector : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceSelector(QWidget *parent = 0, QVBoxLayout* layout = 0);
    ~DeviceSelector();

private slots:
    void addDevice(const QBluetoothDeviceInfo& device);
    void scanFinished();
    void deviceSelected(QListWidgetItem*);
    void on_searchButton_clicked();
    void pairingDone(QBluetoothAddress, QBluetoothLocalDevice::Pairing);
    void on_unpairButton_clicked();
    
signals:
    void sHide();
private:
    QMap<QString, TionDeviceBaseLogic*> Devices;

    void scanStart();
    Ui::DeviceSelector* ui;
    QBluetoothDeviceDiscoveryAgent* discoveryDevAgent;
    QBluetoothDeviceInfo devices;
    QBluetoothLocalDevice *localDevice;
    QVBoxLayout* parent_layout;
};

#endif // DEVICESELECTOR_H
