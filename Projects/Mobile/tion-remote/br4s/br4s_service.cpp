#include "br4s_service.h"
#include "ui_br4s_service.h"
#include <QTimeZone>

br4s_service::br4s_service(br4s_mainwindow *p_br4s_rc, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::br4s_service),
  br4s_rc(p_br4s_rc)
{
    ui->setupUi(this);

    connect(br4s_rc, &br4s_mainwindow::updParam,
            this, &br4s_service::updateParams);
}

br4s_service::~br4s_service()
{
    delete ui;
}

void br4s_service::updateParams()
{
    auto secToDay = [] (int sec) -> int{return sec / 86400;};

    ui->TestOff->setChecked(br4s_rc->getMaRc()->noTest());
    ui->DefTest ->setChecked(br4s_rc->getMaRc()->defaultTest());

    ui->SoundOn->setChecked(br4s_rc->getMaRc()->SoundState());
    ui->LEDOn->setChecked(br4s_rc->getMaRc()->LEDstate());
    ui->MAcontrol->setChecked(br4s_rc->getMaRc()->maControl());
    ui->MAcontrol_auto->setChecked(br4s_rc->getMaRc()->maAutoControl());

    int secs = br4s_rc->getMaRc()->filterCounter();
    int days = secToDay(secs);

    QString label = QString("Filter counter: %1 sec, %2 days").arg(secs).arg(days);
    ui->FilterTime->setText(label);

    secs = br4s_rc->getMaRc()->elecCounter(), days = secToDay(secs);
    label = QString("Electronics counter: %1 sec, %2 days").arg(secs).arg(days);
    ui->ElectroTime->setText(label);

    secs = br4s_rc->getMaRc()->motorCounter(), days = secToDay(secs);
    label = QString("Motor counter: %1 sec, %2 days").arg(secs).arg(days);
    ui->MotorCounter->setText(label);

    QString errLabel = "Current Errors: ";
    QString errCntLabel = "Previous Errors: ";

    const int errNUmb = 11;
    const int warnNUmb = 6;
    for (uint8_t i = 0; i != errNUmb; ++i)
    {
        if(br4s_rc->getMaRc()->error(i))
            errLabel.append(QString("EC%1, ").arg(i + 1));

        if (i < warnNUmb)
        {
            if(br4s_rc->getMaRc()->error(i+24))
                errLabel.append(QString("WS%1, ").arg(i + 1));
        }

        uint8_t ec;
        if((ec = br4s_rc->getMaRc()->errorCnt(i)))
            errCntLabel.append(QString("EC%1 = %2, ").arg(i + 1).arg(ec));
    }



    ui->CurrErrLabel->setText(errLabel);
    ui->PrevErrLabel->setText(errCntLabel);


    ui->AirflowCounter->setText(QString("Airflow counter: %1 m3").
                         arg((float)br4s_rc->getMaRc()->airFlowCnt() * 15.0 / 3600.0));
}

void br4s_service::on_BacktoCtrl_clicked()
{
    this->hide();
    emit BacktoCtrl();
}

void br4s_service::on_FilterResetBut_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->resetFilter());
}

void br4s_service::on_SoundOn_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->switchSound(
                            ui->SoundOn->isChecked()));
}

void br4s_service::on_LEDOn_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->switchLED(
                            ui->LEDOn->isChecked()));
}

void br4s_service::on_MAcontrol_clicked()
{
     br4s_rc->sendOverBt(br4s_rc->getMaRc()->switchMA(
                             ui->MAcontrol->isChecked()));
}

void br4s_service::on_MAcontrol_auto_clicked()
{
     br4s_rc->sendOverBt(br4s_rc->getMaRc()->switchMA_auto(
                             ui->MAcontrol_auto->isChecked()));
}

void br4s_service::on_TestOff_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setNoTest());
}

void br4s_service::on_DefTest_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setDefaultTest());
}


void br4s_service::on_SetResetBut_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->resetHard());
}



void br4s_service::on_HeaterTest_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setHeaterTest());
}
