#include "br4s_mainwindow.h"
#include "br4s_service.h"
#include "timer_service.h"
#include "ui_br4s_mainwindow.h"
#include <QDebug>
#include <QtEndian>
#include <QTimer>
#include <QTime>
#include <QFileDialog>

br4s_mainwindow::br4s_mainwindow(QWidget *parent, QVBoxLayout* layout):
    TionDeviceBaseLogic(parent),
    ui(new Ui::br4s_mainwindow),
    parent_layout(layout)
{
    ui->setupUi(this);
    /*connect(control, &QLowEnergyController::disconnected,
           this, &br4s_mainwindow::on_DiscButton_clicked);*/

    timer_service* br4s_tim_serv = new timer_service(this, this);
    br4s_service* br4s_serv = new br4s_service(this, this);

    connect(this, SIGNAL(br4s_timButton()), br4s_tim_serv, SLOT(show()));
    connect(br4s_tim_serv, SIGNAL(BacktoCtrl()), this, SLOT(show()));

    connect(this, SIGNAL(br4s_optButton()), br4s_serv, SLOT(show()));
    connect(br4s_serv, SIGNAL(BacktoCtrl()), this, SLOT(show()));
    connect(this, SIGNAL(discButton()), br4s_serv, SLOT(hide()));

    parent_layout->addWidget(br4s_tim_serv);
    parent_layout->addWidget(br4s_serv);

    br4s_tim_serv->hide();
    br4s_serv->hide();

    QString msg = "Connecting to " + devName + "...";
    ui->status->setText(msg);
    setUiState(false);
}

br4s_mainwindow::~br4s_mainwindow()
{
    if(firmware)
        delete firmware;
    delete ui;
}



void br4s_mainwindow::setUiState(bool st)
{
    ui->PowerCheckBox->setEnabled(st);
    ui->speed_slider->setEnabled(st);
    ui->temp_slider->setEnabled(st);
    ui->br4s_optButton->setEnabled(st);
    //ui->disconnectButton->setEnabled(st);
}



void br4s_mainwindow::updateDeviceParams(const QLowEnergyCharacteristic &c,
                                          const QByteArray &value)
{   
    if(!inConnect)
    {
        ui->status->setText(devName);
        setUiState(true);
        inConnect = true;
        connect(control, &QLowEnergyController::disconnected,
                   this, &br4s_mainwindow::on_DiscButton_clicked);
    }

    // ignore any other characteristic change -> shouldn't really happen though
    if (c.uuid() != QBluetoothUuid(maServiceCharRxUuid))
        return;

    //qDebug() << QTime::currentTime().second()
    //         << " " << QTime::currentTime().msec();

    quint8 *data = (quint8*)value.constData();//reinterpret_cast<const quint8 *>(value.constData());

    BR4S_Rc::RawData rw;
    rw.p = data;
    rw.ln = value.size();

    if(inUpdate)
    {
        maUpd.mainDataCallback(rw);

        BR4S_Rc::RawDataList temp = maUpd.doUpdate();
        if(!temp.empty())
            sendOverBt(temp);

        ui->updButton->setText(QString("Upd progress: %1 %").arg(maUpd.progress()));

        if(maUpd.progress() == MAUpdater::UPDATE_DONE)
        {
            delete firmware;
            firmware = nullptr;

            on_DiscButton_clicked();
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
    BR4S_Rc.mainDataCallback(rw);

    ui->PowerCheckBox->setChecked(BR4S_Rc.powerState());
    ui->inflow->setEnabled(BR4S_Rc.powerState());
    ui->recirculation->setEnabled(BR4S_Rc.powerState());
    ui->heating->setEnabled(BR4S_Rc.powerState());
    ui->maintenance->setEnabled(BR4S_Rc.powerState());

    if(!ui->speed_slider->isSliderDown())
        ui->speed_slider->setSliderPosition(BR4S_Rc.speed());

    if(!ui->temp_slider->isSliderDown())
        ui->temp_slider->setSliderPosition((BR4S_Rc.temp()));

    QString label = QString("Speed: %1").arg(BR4S_Rc.speed());
    ui->speedLabel->setText(label);
    label = QString("Temperature: %1").arg(BR4S_Rc.temp());
    ui->temperatureLabel->setText(label);

    if (BR4S_Rc.substate() == 0)
    {
        ui->inflow->setChecked(true);
        ui->recirculation->setChecked(false);
        ui->HeaterBox->setEnabled(true);
    }
    else if (BR4S_Rc.substate() == 1)
    {
        ui->inflow->setChecked(false);
        ui->recirculation->setChecked(true);
        ui->HeaterBox->setEnabled(false);
    }

    ui->speed_slider->setEnabled(BR4S_Rc.powerState());

    if (BR4S_Rc.heaterMode() == 0)
    {
        ui->heating->setChecked(true);
        ui->maintenance->setChecked(false);
    }
    else if (BR4S_Rc.heaterMode() == 1)
    {
        ui->heating->setChecked(false);
        ui->maintenance->setChecked(true);
    }

    ui->temp_slider->setEnabled(BR4S_Rc.powerState() &
                                !BR4S_Rc.substate());

    label = QString("T in : %1 c").arg(BR4S_Rc.tempIn());
    ui->tin->setText(label);
    label = QString("T out : %1 c").arg(BR4S_Rc.tempOut());
    ui->tout->setText(label);
    label = QString("T pwr : %1 c").arg(BR4S_Rc.tempPwrb());
    ui->tpwr->setText(label);
    label = QString("T ctrl : %1 c").arg(BR4S_Rc.tempCtrlb());
    ui->tctrl->setText(label);
    label = QString("Heater power : %1 %").arg(BR4S_Rc.heaterVar());
    ui->heat_pwr->setText(label);

    int heatPres = BR4S_Rc.heaterPresent();
    if (heatPres == 0)
        label = QString("Heater presen: None");
    else if (heatPres == 1)
        label = QString("Heater present: 1kW");
    else if (heatPres == 2)
        label = QString("Heater present: 1.4 kW");
    else
        label = QString("Heater present: NaN");


    ui->heat_pres->setText(label);

    ui->firmLabel->setText(QString("Current Fw ver : %1").arg(BR4S_Rc.firmVer(), 0, 16));
    ui->speedLimitLabel->setText(QString("Speed limit : %1").arg(BR4S_Rc.speedLimit()));

    if (!ui->dateTimeEdit->hasFocus())
    {
        QDateTime b_datetime = BR4S_Rc.getDateTime();
        b_datetime.setTimeSpec(Qt::UTC);
        ui->dateTimeEdit->setDateTime(b_datetime);
    }

    if (BR4S_Rc.getTurboState().is_active )
    {
        ui->turboBox->setTitle(QString("Time: %1").arg(BR4S_Rc.getTurboState().turbo_time));
    }
    else
    {
        ui->turboBox->setTitle(QString("Time."));
    }

    if (BR4S_Rc.getTurboState().err_code)
    {
        ui->turboBox->setStyleSheet("QGroupBox {color: red;}");
        ui->turboBox->setTitle(QString("Turbo is disabled because holodno nahuy. Click TURBO to reset error."));
    }
    else
    {
        ui->turboBox->setStyleSheet("QGroupBox {color: black;}");
    }

    emit updParam();
}



void br4s_mainwindow::sendRequest()
{
    // soryan
    if(!inUpdate)
        ui->updButton->setText("Update Firmware");

    if(foundMaService)
    {
        sendOverBt(BR4S_Rc.paramRequest());
        sendOverBt(BR4S_Rc.devRequest());
        sendOverBt(BR4S_Rc.ErrCntRequest());
        sendOverBt(BR4S_Rc.CurrTestRequest());
        sendOverBt(BR4S_Rc.timeRequest());
        sendOverBt(BR4S_Rc.TurboReq());
        for (uint8_t i = 0; i < NUM_OF_TIMERS; i++)
        {
            sendOverBt(BR4S_Rc.timerSettingsRequest(i));
        }
    }

    if (BR4S_Rc.getTurboState().is_active)
    {
        if (!QString::compare(ui->turbo_btn->styleSheet(), "QPushButton {background-color: #A3C1DA; color: green;}"))
            ui->turbo_btn->setStyleSheet("QPushButton {background-color: #A3C1DA; color: black;}");
        else
            ui->turbo_btn->setStyleSheet("QPushButton {background-color: #A3C1DA; color: green;}");
    }
    else
    {
        ui->turbo_btn->setStyleSheet("QPushButton {background-color: #A3A3A3; color: black;}");
    }
}


void br4s_mainwindow::on_speed_slider_sliderReleased()
{
    sendOverBt(BR4S_Rc.setSpeed(ui->speed_slider->sliderPosition()));
}

void br4s_mainwindow::on_temp_slider_sliderReleased()
{
    sendOverBt(BR4S_Rc.setTemp(ui->temp_slider->sliderPosition()));
}


void br4s_mainwindow::on_br4s_optButton_clicked()
{
    this->hide();
    emit br4s_optButton();
}

BR4S_Rc *br4s_mainwindow::getMaRc()
{
    return &BR4S_Rc;
}

void br4s_mainwindow::on_updButton_clicked()
{
    if (!inUpdate)
    {
        QString filename = QFileDialog::getOpenFileName(this, "Open File", "/home",
                                                        "", nullptr);


        firmware = new FileFirmware(filename);

        if (!firmware->isValid(FIRM_Rb4S))
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

void br4s_mainwindow::on_PowerCheckBox_clicked()
{
    sendOverBt(BR4S_Rc.setPowerState(ui->PowerCheckBox->isChecked()));
}

void br4s_mainwindow::on_inflow_clicked()
{
    sendOverBt(BR4S_Rc.setSubstate(0));
}

void br4s_mainwindow::on_recirculation_clicked()
{
    sendOverBt(BR4S_Rc.setSubstate(1));
}

void br4s_mainwindow::on_DiscButton_clicked()
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

void br4s_mainwindow::on_heating_clicked()
{
    sendOverBt(BR4S_Rc.setHeaterMode(0));
}

void br4s_mainwindow::on_maintenance_clicked()
{
    sendOverBt(BR4S_Rc.setHeaterMode(1));
}

void br4s_mainwindow::timer_timeout_handler()
{
    maUpd.timeout_handler();
}

void br4s_mainwindow::on_TimButton_clicked()
{
    this->hide();
    emit br4s_timButton();
}

void br4s_mainwindow::on_dateTimeEdit_editingFinished()
{
    QDateTime b_datetime = BR4S_Rc.getDateTime();
    b_datetime.setTimeSpec(Qt::LocalTime);
    int64_t offset = b_datetime.offsetFromUtc();

    sendOverBt(BR4S_Rc.setTime((ui->dateTimeEdit->dateTime()).toSecsSinceEpoch(), offset));

    ui->dateTimeEdit->clearFocus();
}

void br4s_mainwindow::on_speed_slider_actionTriggered(int action)
{
    if (action == QAbstractSlider::SliderPageStepAdd ||
        action == QAbstractSlider::SliderPageStepSub ||
        action == QAbstractSlider::SliderSingleStepSub ||
        action == QAbstractSlider::SliderSingleStepAdd)
    {
        sendOverBt(BR4S_Rc.setSpeed(ui->speed_slider->sliderPosition()));
    }
}

void br4s_mainwindow::on_temp_slider_actionTriggered(int action)
{
    if (action == QAbstractSlider::SliderPageStepAdd ||
        action == QAbstractSlider::SliderPageStepSub ||
        action == QAbstractSlider::SliderSingleStepSub ||
        action == QAbstractSlider::SliderSingleStepAdd)
    {
        sendOverBt(BR4S_Rc.setTemp(ui->temp_slider->sliderPosition()));
    }
}


void br4s_mainwindow::on_turbo_btn_clicked()
{
    uint16_t time = 0;

    if (!BR4S_Rc.getTurboState().err_code)
    {
        if (ui->rdbtn_5_min->isChecked())
            time = 5 * 60;
        else if (ui->rdbtn_10_min->isChecked())
            time = 10 * 60;
        else if (ui->rdbtn_15_min->isChecked())
            time = 15 * 60;
    }
    else
    {
        time = 0;
    }

    sendOverBt(BR4S_Rc.StartTurbo(time));
}


void br4s_mainwindow::on_TurboTimeSpinBox_editingFinished()
{
    sendOverBt(BR4S_Rc.StartTurbo(ui->TurboTimeSpinBox->text().toUInt()));
    ui->TurboTimeSpinBox->clearFocus();
}
