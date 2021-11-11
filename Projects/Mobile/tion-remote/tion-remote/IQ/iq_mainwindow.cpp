#include "iq_mainwindow.h"
#include "iq_service.h"
#include "iq_autoindication.h"

#include <QFileDialog>
#include "ui_iq_mainwindow.h"

iq_mainwindow::iq_mainwindow(QWidget *parent, QVBoxLayout* layout) :
    TionDeviceBaseLogic(parent),
    ui(new Ui::iq_mainwindow),
    parent_layout(layout)
{
    ui->setupUi(this);

    iq_service* iq_serv = new iq_service(this, this);
    iq_autoindication* iq_aind = new iq_autoindication(this, this);

    connect(this, SIGNAL(go_to_service()), iq_serv, SLOT(show()));
    connect(iq_serv, SIGNAL(BacktoCtrl()), this, SLOT(show()));
    connect(this, SIGNAL(discButton()), iq_serv, SLOT(hide()));

    connect(this, SIGNAL(go_to_autoind()), iq_aind, SLOT(show()));
    connect(iq_aind, SIGNAL(BacktoCtrl()), this, SLOT(show()));
    connect(this, SIGNAL(discButton()), iq_aind, SLOT(hide()));

    parent_layout->addWidget(iq_serv);
    parent_layout->addWidget(iq_aind);

    iq_serv->hide();
    iq_aind->hide();

    QString msg = "Connecting to " + devName + "...";
    ui->status->setText(msg);
    setUiState(false);
}

void iq_mainwindow::updateDeviceParams(const QLowEnergyCharacteristic &c,
                                const QByteArray &value)
{
    if(!inConnect)
    {
        ui->status->setText(devName);
        setUiState(true);
        inConnect = true;
        connect(control, &QLowEnergyController::disconnected,
                   this, &iq_mainwindow::on_discButton_clicked);
    }

    // ignore any other characteristic change -> shouldn't really happen though
    if (c.uuid() != QBluetoothUuid(maServiceCharRxUuid))
        return;

    //qDebug() << QTime::currentTime().second()
    //         << " " << QTime::currentTime().msec();

    quint8 *data = (quint8*)value.constData();//reinterpret_cast<const quint8 *>(value.constData());

    IQ_Rc::RawData rw;
    rw.p = data;
    rw.ln = value.size();

    if(inUpdate)
    {
        maUpd.mainDataCallback(rw);

        IQ_Rc::RawDataList temp = maUpd.doUpdate();
        if(!temp.empty())
            sendOverBt(temp);

        ui->updButton->setText(QString("Upd progress: %1 %").arg(maUpd.progress()));

        if(maUpd.progress() == MAUpdater::UPDATE_DONE)
        {
            delete firmware;
            firmware = nullptr;

            on_discButton_clicked();
        }

        if(maUpd.progress() == MAUpdater::UPDATE_DONE_TIMEOUT)
        {
            delete firmware;
            firmware = nullptr;
            inUpdate = false;
            ui->updButton->setText(QString("TIMEOUT!"));
        }

        return;
    }

    ui->updButton->setText(QString("Update Firmware"));
    IQ_rc.mainDataCallback(rw);

    ui->powerOn->setChecked(IQ_rc.getPowerState());
    ui->ChildLock->setChecked(IQ_rc.getChildLock());

    if(!ui->speed_slider->isSliderDown())
        ui->speed_slider->setSliderPosition(IQ_rc.getFanSpeed());
    QString label = QString("Speed: %1").arg(IQ_rc.getFanSpeed());
    ui->speedLabel->setText(label);

    ui->sleepTimerOn->setChecked(IQ_rc.getActiveTimer());
    if (!ui->sleepTimerValue->hasFocus())
       ui->sleepTimerValue->setValue(IQ_rc.getSleepTimerValue());

    ui->firmLabel->setText(QString("Current Fw ver : %1").arg(IQ_rc.firmVer(), 0, 16));
    ui->speedLimitLabel->setText(QString("Speed limit : %1").arg(IQ_rc.getSpeedLimit()));

    ui->PM25->setText(QString("PM 2.5: %1").arg(IQ_rc.getPM25()));
    ui->VOC->setText(QString("VOC: %1").arg(IQ_rc.getVOC()));
    ui->CCM_label->setText(QString("CCM: %1").arg(IQ_rc.getFilterCCM()));


    emit updParam();
}

void iq_mainwindow::setUiState(bool st)
{

}

IQ_Rc* iq_mainwindow::getMaRc()
{
     return &IQ_rc;
}

void iq_mainwindow::sendRequest()
{
    if(!inUpdate)
        ui->updButton->setText("Update Firmware");

    if(foundMaService)
    {
        sendOverBt(IQ_rc.paramRequest());
        sendOverBt(IQ_rc.DevParamRequest());
        sendOverBt(IQ_rc.AutosetIndRequest());
    }
}

iq_mainwindow::~iq_mainwindow()
{
    delete ui;
}

void iq_mainwindow::on_discButton_clicked()
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
}

void iq_mainwindow::on_powerOn_clicked()
{
    sendOverBt(IQ_rc.setPowerState(ui->powerOn->isChecked()));
}

void iq_mainwindow::on_ChildLock_clicked()
{
    sendOverBt(IQ_rc.setChildLock(ui->ChildLock->isChecked()));
}

void iq_mainwindow::on_sleepTimerValue_editingFinished()
{
    sendOverBt(IQ_rc.setSleepTimerValue(ui->sleepTimerValue->value()));

    ui->sleepTimerValue->clearFocus();
}

void iq_mainwindow::on_sleepTimerOn_clicked()
{
    sendOverBt(IQ_rc.setActiveTimer(ui->sleepTimerOn->isChecked()));
}

void iq_mainwindow::on_speed_slider_actionTriggered(int action)
{
    if (action == QAbstractSlider::SliderPageStepAdd ||
        action == QAbstractSlider::SliderPageStepSub ||
        action == QAbstractSlider::SliderSingleStepSub ||
        action == QAbstractSlider::SliderSingleStepAdd)
    {
        sendOverBt(IQ_rc.setFanSpeed(ui->speed_slider->sliderPosition()));
    }
}


void iq_mainwindow::on_srvBtn_clicked()
{
    this->hide();
    emit go_to_service();
}

void iq_mainwindow::on_autoIndBtn_clicked()
{
    this->hide();
    emit go_to_autoind();
}

void iq_mainwindow::on_updButton_clicked()
{
    if (!inUpdate)
    {
        QString filename = QFileDialog::getOpenFileName(this, "Open File", "/home",
                                                        "", nullptr);


        firmware = new FileFirmware(filename);

        if (!firmware->isValid(FIRM_IQ))
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

void iq_mainwindow::on_speed_slider_sliderReleased()
{
     sendOverBt(IQ_rc.setFanSpeed(ui->speed_slider->sliderPosition()));
}
