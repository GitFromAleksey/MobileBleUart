#include "iq_service.h"
#include "ui_iq_service.h"
#include <QTimeZone>

iq_service::iq_service(iq_mainwindow *p_iq_rc, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::iq_service),
  iq_rc(p_iq_rc)
{
    ui->setupUi(this);

    connect(iq_rc, &iq_mainwindow::updParam,
            this, &iq_service::updateParams);
}

iq_service::~iq_service()
{
    delete ui;
}

void iq_service::updateParams()
{
    auto secToDay = [] (int sec) -> int{return sec / 86400;};

    ui->SoundOn->setChecked(iq_rc->getMaRc()->getSoundInd());
    switch(iq_rc->getMaRc()->getLEDInd())
    {
        case 0:
        ui->LED_night->setChecked(true);
        break;
        case 1:
        ui->LED_off->setChecked(true);
        break;
        case 2:
        ui->LED_50->setChecked(true);
        break;
        case 3:
        ui->LED_100->setChecked(true);
        break;
    }

    ui->MAcontrol->setChecked(iq_rc->getMaRc()->getMagicAir());
    ui->MAcontrol_auto->setChecked(iq_rc->getMaRc()->getMAAuto());

    int secs = iq_rc->getMaRc()->getFilterCounter();
    int days = secToDay(secs);

    QString label = QString("Filter counter: %1 sec, %2 days").arg(secs).arg(days);
    ui->FilterTime->setText(label);

    secs = iq_rc->getMaRc()->getHWCounter(), days = secToDay(secs);
    label = QString("Electronics counter: %1 sec, %2 days").arg(secs).arg(days);
    ui->ElectroTime->setText(label);

    secs = iq_rc->getMaRc()->getFanCounter(), days = secToDay(secs);
    label = QString("Motor counter: %1 sec, %2 days").arg(secs).arg(days);
    ui->MotorCounter->setText(label);

    secs = iq_rc->getMaRc()->getABLayerCounter(), days = secToDay(secs);
    ui->ABLCounter->setText(QString("Antibacterial layer counter:  %1 sec, %2 days").arg(secs).arg(days));

    QString errLabel = "Current Errors: ";

    const int errNUmb = 11;
    //const int warnNUmb = 6;
    for (uint8_t i = 0; i != errNUmb; ++i)
    {
        if(iq_rc->getMaRc()->getError(i))
            errLabel.append(QString("EC%1, ").arg(i + 1));
    }

    ui->CurrErrLabel->setText(errLabel);


}

void iq_service::on_BacktoCtrl_clicked()
{
    this->hide();
    emit BacktoCtrl();
}

void iq_service::on_FilterResetBut_clicked()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->resetFilter());
}

void iq_service::on_SoundOn_clicked()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setSoundInd(
                            ui->SoundOn->isChecked()));
}

void iq_service::on_MAcontrol_clicked()
{
     iq_rc->sendOverBt(iq_rc->getMaRc()->setMagicAir(
                             ui->MAcontrol->isChecked()));
}

void iq_service::on_MAcontrol_auto_clicked()
{
     iq_rc->sendOverBt(iq_rc->getMaRc()->setMAAuto(
                             ui->MAcontrol_auto->isChecked()));
}

void iq_service::on_SetResetBut_clicked()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->HardReset());
}

void iq_service::on_LED_night_clicked()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setLEDInd(0));
}

void iq_service::on_LED_off_clicked()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setLEDInd(1));
}

void iq_service::on_LED_50_clicked()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setLEDInd(2));
}

void iq_service::on_LED_100_clicked()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setLEDInd(3));
}

void iq_service::on_ErrResetBtn_clicked()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->resetErrCnt());
}
