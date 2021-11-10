#include <QMap>

#include "deviceselector.h"
#include "lib/tiondevicebaselogic.h"
#include "ui_deviceselector.h"

DeviceSelector::DeviceSelector(QWidget *parent, QVBoxLayout* layout) :
    QWidget(parent),
    ui(new Ui::DeviceSelector),
    localDevice(new QBluetoothLocalDevice(this)),
    parent_layout(layout)
{
    ui->setupUi(this);

    connect(localDevice,
            SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing))
            ,this, SLOT(pairingDone(QBluetoothAddress,QBluetoothLocalDevice::Pairing)));

    discoveryDevAgent = new QBluetoothDeviceDiscoveryAgent(this);
    discoveryDevAgent->setLowEnergyDiscoveryTimeout(20000);

    connect(discoveryDevAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &DeviceSelector::addDevice);
    connect(discoveryDevAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &DeviceSelector::scanFinished);

    connect(ui->devicesList, SIGNAL(itemPressed(QListWidgetItem*)),
            this, SLOT(deviceSelected(QListWidgetItem*)));

    QListWidgetItem *item = new QListWidgetItem("TEST ff:ff:ff:ff:ff:ff");
    ui->devicesList->addItem(item);

    RemoteController* rc = new RemoteController(this, parent_layout);
    br4s_mainwindow* br4s_rc = new br4s_mainwindow(this, parent_layout);
    iq_mainwindow*  iq_rc = new iq_mainwindow(this, parent_layout);

    connect(rc, SIGNAL(discButton()), this, SLOT(show()));
    connect(br4s_rc, SIGNAL(discButton()), this, SLOT(show()));
    connect(iq_rc, SIGNAL(discButton()), this, SLOT(show()));

    parent_layout->addWidget(rc);
    parent_layout->addWidget(br4s_rc);
    parent_layout->addWidget(iq_rc);

    rc->hide();
    br4s_rc->hide();
    iq_rc->hide();

    Devices.insert("Br Lite", rc);
    Devices.insert("Breezer 4S", br4s_rc);
    Devices.insert("Tion IQ200", iq_rc);
    Devices.insert("Tion IQ400", iq_rc);
}


DeviceSelector::~DeviceSelector()
{
    delete ui;
}

void DeviceSelector::addDevice(const QBluetoothDeviceInfo &device)
{
    // If device is LowEnergy-device, add it to the list
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
    {
        QString name = device.name();
        qDebug() << "rssi" << device.rssi() << device.isCached() << name
                 << device.address();

        QMap<QString, TionDeviceBaseLogic*>::iterator iter = Devices.find(name);
        if(iter == Devices.end())
        {
            return;
        }

        QString label = QString("%1 %2 rssi %3").arg(device.address().toString()).
                                                arg(name).arg(device.rssi());
        QList<QListWidgetItem *> items = ui->devicesList->findItems(label,
                                                                    Qt::MatchExactly);

        if (items.empty())
        {
            QListWidgetItem *item = new QListWidgetItem(label);

            QBluetoothLocalDevice::Pairing pairingStatus =
                                        localDevice->pairingStatus(device.address());
            if (pairingStatus == QBluetoothLocalDevice::Paired ||
                    pairingStatus == QBluetoothLocalDevice::AuthorizedPaired )
                //item->setTextColor(QColor(Qt::green));
                item->setForeground(QColor(Qt::green));
            else
                //item->setTextColor(QColor(Qt::black));
                item->setForeground(QColor(Qt::black));
            ui->devicesList->addItem(item);
        }
    }

}

void DeviceSelector::scanStart()
{
    ui->devicesList->clear();
//    ui->searchButton->setEnabled(false);
    discoveryDevAgent->stop();
    discoveryDevAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void DeviceSelector::scanFinished()
{
    //scanStart();
    //ui->searchButton->setEnabled(true);
}

void DeviceSelector::on_searchButton_clicked()
{
    scanStart();
}

void DeviceSelector::deviceSelected(QListWidgetItem* item)
{
    QString text = item->text();

    int index = text.indexOf(' ');

    if (index == -1)
        return;

    discoveryDevAgent->stop();

    QBluetoothAddress address(text.left(index));
    QString temp(text.mid(index + 1));

    QString name (temp.left(temp.indexOf(" rssi")));

    QBluetoothLocalDevice::Pairing pairingStatus = localDevice->pairingStatus(address);

    if (pairingStatus == QBluetoothLocalDevice::Paired ||
            pairingStatus == QBluetoothLocalDevice::AuthorizedPaired ||
            item->text() == "TEST ff:ff:ff:ff:ff:ff")
    {
        QMap<QString, TionDeviceBaseLogic*>::iterator iter = Devices.find(name);
        if(iter != Devices.end())
        {
            iter.value()->setDevice(name, address);
            iter.value()->show();
            this->hide();
        }

    }
    else
    {
        localDevice->requestPairing(address, QBluetoothLocalDevice::Paired);
    }
}

void DeviceSelector::pairingDone(QBluetoothAddress address,
                                 QBluetoothLocalDevice::Pairing pairing)
{
    QList<QListWidgetItem *> items = ui->devicesList->findItems(address.toString(),
                                                                Qt::MatchContains);
    QString name;

    if (pairing == QBluetoothLocalDevice::Paired ||
        pairing == QBluetoothLocalDevice::AuthorizedPaired ) {
        for (int var = 0; var < items.count(); ++var) {
            QListWidgetItem *item = items.at(var);
            //item->setTextColor(QColor(Qt::green));
            item->setForeground(QColor(Qt::green));
        }
    } else {
        for (int var = 0; var < items.count(); ++var) {
            QListWidgetItem *item = items.at(var);
            //item->setTextColor(QColor(Qt::red));
            item->setForeground(QColor(Qt::red));
        }
    }
}


void DeviceSelector::on_unpairButton_clicked()
{
    QList<QListWidgetItem *> items = ui->devicesList->findItems("IQ", Qt::MatchContains);
    QString text, adress;
    QBluetoothAddress devAddr;

    for (int i = 0; i < items.size(); ++i)
    {
        text = items.at(i)->text();
        adress = text.left(text.indexOf(" "));
        devAddr = QBluetoothAddress(adress);
        localDevice->requestPairing(devAddr, QBluetoothLocalDevice::Unpaired);
    }
}
