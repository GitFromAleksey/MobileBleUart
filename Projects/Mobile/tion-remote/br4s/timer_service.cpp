#include "timer_service.h"
#include "ui_timer_service.h"

#define NUM_OF_TIMERS   6

timer_service::timer_service(br4s_mainwindow *p_br4s_rc, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::timer_service),
    br4s_rc(p_br4s_rc)
{
    ui->setupUi(this);


    for (int index = 0; index < NUM_OF_TIMERS ; index++) {
        timers[index] = new timer_tab(br4s_rc, this, index);
        connect(br4s_rc, &br4s_mainwindow::updParam,
                timers[index], &timer_tab::updateParams);
        ui->TimersStack->insertTab(index, timers[index], QString("Timer %1").arg(index +1));
    }
}

timer_service::~timer_service()
{
    for (int index = 0; index < NUM_OF_TIMERS ; index++) {
        ui->TimersStack->removeTab(index);
    }
    delete ui;
}



void timer_service::on_back_to_ctrl_button_clicked()
{
    this->hide();
    emit BacktoCtrl();
}


