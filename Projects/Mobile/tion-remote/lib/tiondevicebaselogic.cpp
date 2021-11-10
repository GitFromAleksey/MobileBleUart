#include "tiondevicebaselogic.h"
#include <QObject>

const QString TionDeviceBaseLogic::maServiceUuid =
        QString("{98f00001-3788-83ea-453e-f52244709ddb}");
const QString TionDeviceBaseLogic::maServiceCharTxUuid =
        QString("{98f00002-3788-83ea-453e-f52244709ddb}");
const QString TionDeviceBaseLogic::maServiceCharRxUuid =
        QString("{98f00003-3788-83ea-453e-f52244709ddb}");
const QString TionDeviceBaseLogic::maServiceNotifyDescUuid =
        QString("{00002902-0000-1000-8000-00805f9b34fb}");

TionDeviceBaseLogic::TionDeviceBaseLogic(QWidget *parent) :
    QWidget(parent),
    control(nullptr),
    service(nullptr),
    foundMaService(false),
    inUpdate(false),
    inConnect(false)
{

}

void TionDeviceBaseLogic::setDevice(const QString &name,
                            const QBluetoothAddress &addr)
{
    devName = name;

    if(control)
    {
        delete control;
        control = nullptr;
    }

    control = new QLowEnergyController(addr, this);
    control->setRemoteAddressType(QLowEnergyController::PublicAddress);

    connect(control, &QLowEnergyController::serviceDiscovered,
            this, &TionDeviceBaseLogic::serviceDiscovered);
    connect(control, &QLowEnergyController::discoveryFinished,
            this, &TionDeviceBaseLogic::serviceScanDone);
    connect(control, &QLowEnergyController::connected, this, [this]()
    {
        qDebug() << "connected";
        control->discoverServices();
    });

    // Connect
    control->connectToDevice();
}

void TionDeviceBaseLogic::serviceDiscovered(const QBluetoothUuid &gatt)
{
    if (gatt.toString() == maServiceUuid)
    {
       foundMaService = true;
    }
}

void TionDeviceBaseLogic::serviceScanDone()
{
    // Delete old service if available
    if (service) {
        delete service;
        service = nullptr;
    }

    // If heartRateService found, create new service
    if (foundMaService)
    {
        service = control->createServiceObject(QBluetoothUuid(maServiceUuid), this);
    }

    if (service)
    {
        connect(service, &QLowEnergyService::stateChanged,
                this, &TionDeviceBaseLogic::serviceStateChanged);
        connect(service, &QLowEnergyService::characteristicChanged,
                this, &TionDeviceBaseLogic::updateDeviceParams);

        connect(service,
                QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error),
                this, &TionDeviceBaseLogic::serviceError);

        //connect(service, &QLowEnergyService::characteristicWritten,
        //        this, []() {qDebug() << "Charac written";});
        service->discoverDetails();
    }

}

void TionDeviceBaseLogic::serviceStateChanged(QLowEnergyService::ServiceState s)
{
    switch (s)
    {
    case QLowEnergyService::DiscoveringServices:

        break;
    case QLowEnergyService::ServiceDiscovered:
    {
        QLowEnergyCharacteristic charRx =
                service->characteristic(QBluetoothUuid(maServiceCharRxUuid));

        notificationDesc = charRx.descriptor(QBluetoothUuid(maServiceNotifyDescUuid));
        if (notificationDesc.isValid())
        {
            qDebug() << "Write to descriptor";
            service->writeDescriptor(notificationDesc, QByteArray::fromHex("0100"));
        }

        inConnect = false;
        sendRequest();
        startPolling();
    }
        break;
    default:
        //nothing for now
        break;
    }
}


void TionDeviceBaseLogic::showUI()
{
    this->show();
}

void TionDeviceBaseLogic::startPolling()
{
    qDebug() << "Descriptor is written";
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this,
            SLOT(sendRequest()));
    timer->start(700);
}

void TionDeviceBaseLogic::sendOverBt(MABaseProtocol::RawDataList rwl)
{
    QLowEnergyCharacteristic charTx =
            service->characteristic(QBluetoothUuid(maServiceCharTxUuid));

   foreach(MABaseProtocol::RawData rw, rwl)
   {
      QByteArray ba((const char*)rw.p, rw.ln);
      //qDebug() << QTime::currentTime().second()
      //         << " " << QTime::currentTime().msec();

      service->writeCharacteristic(charTx, ba, QLowEnergyService::WriteWithoutResponse);
    }
    qDebug() << "Send packet";
}

void TionDeviceBaseLogic::serviceError(QLowEnergyService::ServiceError newError)
{
    qDebug() << "error" << newError;
}
