#include "configurator.h"
#include "ui_configurator.h"

Configurator::Configurator(RemoteController* remc, QWidget *parent):
    QWidget(parent),
    ui(new Ui::Configurator),
    rc(remc)
{
    ui->setupUi(this);

    connect(rc, &RemoteController::updParam,
            this, &Configurator::updateParams);
}

Configurator::~Configurator()
{
    delete ui;
}


void Configurator::updateParams()
{
    auto secToDay = [] (int sec) -> int{return sec / 86400;};

    ui->noTestButton->setChecked(rc->getMaRc()->noTest());
    ui->gateTestButton ->setChecked(rc->getMaRc()->gateTest());
    ui->heatTestButton->setChecked(rc->getMaRc()->heaterTest());

    int secs = rc->getMaRc()->filterCounter();
    int days = secToDay(secs);

    QString label = QString("Filter counter: %1 sec, %2 days").arg(secs).arg(days);
    ui->filterLabel->setText(label);

    secs = rc->getMaRc()->elecCounter(), days = secToDay(secs);
    label = QString("Electronics counter: %1 sec, %2 days").arg(secs).arg(days);
    ui->elCountLabel->setText(label);

    secs = rc->getMaRc()->motorCounter(), days = secToDay(secs);
    label = QString("Motor counter: %1 sec, %2 days").arg(secs).arg(days);
    ui->motCountLabel->setText(label);

    QString errLabel = "Current Errors: ";
    QString errCntLabel = "Previous Errors: ";

    const int errNUmb = 11;
    for (int i = 0; i != errNUmb; ++i)
    {
        if(rc->getMaRc()->error(i))
            errLabel.append(QString("EC%1, ").arg(i + 1));

        int ec;
        if((ec = rc->getMaRc()->errorCnt(i)))
            errCntLabel.append(QString("EC%1 = %2, ").arg(i + 1).arg(ec));
    }

    ui->curErrLabel->setText(errLabel);
    ui->prevErrLabel->setText(errCntLabel);


    ui->afLabel->setText(QString("Airflow counter: %1 m3").
                         arg((float)rc->getMaRc()->airFlowCnt() * 15.0 / 3600.0));
}

void Configurator::on_gateTestButton_clicked()
{
    if(ui->gateTestButton->isChecked())
        rc->sendOverBt(rc->getMaRc()->setGateTest());
}

void Configurator::on_noTestButton_clicked()
{
    if(ui->noTestButton->isChecked())
        rc->sendOverBt(rc->getMaRc()->setNoTest());
}

void Configurator::on_heatTestButton_clicked()
{
    if(ui->heatTestButton->isChecked())
        rc->sendOverBt(rc->getMaRc()->setHeaterTest());
}


void Configurator::on_contButton_clicked()
{
    this->hide();
    emit contButton();
}

void Configurator::on_filterButton_clicked()
{
    rc->sendOverBt(rc->getMaRc()->resetFilter());
}

void Configurator::on_soundButton_clicked()
{
    rc->sendOverBt(rc->getMaRc()->switchSound());
}

void Configurator::on_hardResetButton_clicked()
{
    rc->sendOverBt(rc->getMaRc()->resetHard());
}
