#include "remotecontroller.h"
#include "ui_remotecontroller.h"
#include "brlt/configurator.h"
#include <QDebug>
#include <QtEndian>
#include <QTimer>
#include <QTime>
#include <QFileDialog>

RemoteController::RemoteController(QWidget *parent, QVBoxLayout* layout):
    TionDeviceBaseLogic(parent),
    ui(new Ui::RemoteController),
    parent_layout(layout)
{
    ui->setupUi(this);
    /*connect(control, &QLowEnergyController::disconnected,
           this, &RemoteController::on_disconnectButton_clicked);*/

    Configurator* conf = new Configurator(this, this);

    connect(this, SIGNAL(optButton()), conf, SLOT(show()));
    connect(conf, SIGNAL(contButton()), this, SLOT(show()));
    connect(this, SIGNAL(discButton()), conf, SLOT(hide()));

    parent_layout->addWidget(conf);
    conf->hide();

    QString msg = "Connecting to " + devName + "...";
    ui->status->setText(msg);
    setUiState(false);
}


RemoteController::~RemoteController()
{
    if(firmware)
        delete firmware;
    delete ui;
}

void RemoteController::setUiState(bool st)
{
    ui->heaterSwitcher->setEnabled(st);
    ui->powerSwitcher->setEnabled(st);
    ui->speedSlider->setEnabled(st);
    ui->tempSlider->setEnabled(st);
    ui->optButton->setEnabled(st);
    //ui->disconnectButton->setEnabled(st);
}

void RemoteController::on_disconnectButton_clicked()
{
    if(control)
        control->disconnectFromDevice();

    foundMaService = false;
    inUpdate = false;

    //disable notifications
    if (notificationDesc.isValid() && service
            && notificationDesc.value() == QByteArray::fromHex("0100"))
        service->writeDescriptor(notificationDesc, QByteArray::fromHex("0000"));

    this->hide();

    timer->stop();

    emit discButton();
    //emit sHide();
}

void RemoteController::updateDeviceParams(const QLowEnergyCharacteristic &c,
                                          const QByteArray &value)
{   
    if(!inConnect)
    {
        ui->status->setText(devName);
        setUiState(true);
        inConnect = true;
        connect(control, &QLowEnergyController::disconnected,
               this, &RemoteController::on_disconnectButton_clicked);
    }

    // ignore any other characteristic change -> shouldn't really happen though
    if (c.uuid() != QBluetoothUuid(maServiceCharRxUuid))
        return;

    //qDebug() << QTime::currentTime().second()
    //         << " " << QTime::currentTime().msec();

    quint8 *data = (quint8*)value.constData();//reinterpret_cast<const quint8 *>(value.constData());

    MABaseProtocol::RawData rw;
    rw.p = data;
    rw.ln = value.size();

    if(inUpdate)
    {
        maUpd.mainDataCallback(rw);

        MABaseProtocol::RawDataList temp = maUpd.doUpdate();
        if(!temp.empty())
            sendOverBt(temp);

        ui->updButton->setText(QString("Upd progress: %1 %").arg(maUpd.progress()));

        if(maUpd.progress() == MAUpdater::UPDATE_DONE)
        {
            delete firmware;
            firmware = nullptr;

            on_disconnectButton_clicked();
        }

        return;
    }

    maRc.mainDataCallback(rw);

    ui->powerSwitcher->setChecked(maRc.powerState());
    ui->heaterSwitcher->setChecked(maRc.heaterState());

    if(!ui->speedSlider->isSliderDown())
        ui->speedSlider->setSliderPosition(maRc.speed());

    if(!ui->tempSlider->isSliderDown())
        ui->tempSlider->setSliderPosition((maRc.temp()));

    QString label = QString("Speed: %1").arg(maRc.speed());
    ui->speed->setText(label);
    label = QString("Temperature: %1").arg(maRc.temp());
    ui->temperature->setText(label);

    ui->speedSlider->setEnabled(maRc.powerState());
    ui->tempSlider->setEnabled(maRc.powerState() && maRc.heaterState());
    ui->heaterSwitcher->setEnabled(maRc.powerState());

    label = QString("T in : %1 c").arg(maRc.tempIn());
    ui->tinLabel->setText(label);
    label = QString("T out : %1 c").arg(maRc.tempOut());
    ui->toutLable->setText(label);
    label = QString("Heater power : %1 %").arg(maRc.heaterVar());
    ui->heaterLabel->setText(label);

    ui->firmLabel->setText(QString("Current Fw ver : %1").arg(maRc.firmVer(), 0, 16));
    ui->speedLimitLabel->setText(QString("Speed limit : %1").arg(maRc.speedLimit()));

    emit updParam();
}

void RemoteController::sendRequest()
{
    // soryan
    if(!inUpdate)
        ui->updButton->setText("Update Firmware");

    if(foundMaService)
    {
        sendOverBt(maRc.paramRequest());
        sendOverBt(maRc.devRequest());
    }
}

void RemoteController::on_powerSwitcher_clicked()
{
    sendOverBt(maRc.setPowerState(ui->powerSwitcher->isChecked()));
}

void RemoteController::on_heaterSwitcher_clicked()
{
   sendOverBt(maRc.setHeaterState(ui->heaterSwitcher->isChecked()));
}

void RemoteController::on_speedSlider_sliderReleased()
{
    sendOverBt(maRc.setSpeed(ui->speedSlider->sliderPosition()));
}

void RemoteController::on_tempSlider_sliderReleased()
{
    sendOverBt(maRc.setTemp(ui->tempSlider->sliderPosition()));
}

void RemoteController::on_optButton_clicked()
{
    this->hide();
    emit optButton();
}

MaRc *RemoteController::getMaRc()
{
    return &maRc;
}

void RemoteController::timer_timeout_handler()
{
    maUpd.timeout_handler();
}


void RemoteController::on_updButton_clicked()
{
    if (!inUpdate)
    {
        QString filename = QFileDialog::getOpenFileName(this, "Open File", "/home",
                                                        "", nullptr);


        firmware = new FileFirmware(filename);

        if (!firmware->isValid(FIRM_BrLt))
        {
            delete firmware;
            ui->updButton->setText("Not valid");
            return;
        }
        ui->updButton->setText("Valid");

        timeout_timer = new QTimer(this);
        connect(timeout_timer, SIGNAL(timeout()), this,
                SLOT(timer_timeout_handler()));
        maUpd.setTimer(timeout_timer);

        maUpd.setFirm(firmware);
        sendOverBt(maUpd.doUpdate(true));
        inUpdate = true;
        setUiState(false);
    }
}
