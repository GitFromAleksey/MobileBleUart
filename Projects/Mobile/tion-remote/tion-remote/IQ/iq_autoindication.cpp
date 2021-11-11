#include "iq_autoindication.h"
#include "ui_iq_autoindication.h"

iq_autoindication::iq_autoindication(iq_mainwindow *p_iq_rc,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::iq_autoindication),
    iq_rc(p_iq_rc)
{
    ui->setupUi(this);

    connect(iq_rc, &iq_mainwindow::updParam,
            this, &iq_autoindication::updateParams);
}

iq_autoindication::~iq_autoindication()
{
    delete ui;
}

void iq_autoindication::updateParams()
{
    if(!ui->pm25_red_ind->hasFocus())
    ui->pm25_red_ind->setValue(iq_rc->getMaRc()->getPM25_red_ind());

    if(!ui->pm25_yellow_ind->hasFocus())
    ui->pm25_yellow_ind->setValue(iq_rc->getMaRc()->getPM25_yellow_ind());

    if(!ui->pm25_green_ind->hasFocus())
    ui->pm25_green_ind->setValue(iq_rc->getMaRc()->getPM25_green_ind());

    if(!ui->pm25_motor_off->hasFocus())
    ui->pm25_motor_off->setValue(iq_rc->getMaRc()->getPM25_motor_off());

    if(!ui->pm25_1_speed->hasFocus())
    ui->pm25_1_speed->setValue(iq_rc->getMaRc()->getPM25_1_speed());

    if(!ui->pm25_2_speed->hasFocus())
    ui->pm25_2_speed->setValue(iq_rc->getMaRc()->getPM25_2_speed());

    if(!ui->pm25_3_speed->hasFocus())
    ui->pm25_3_speed->setValue(iq_rc->getMaRc()->getPM25_3_speed());

    if(!ui->pm25_4_speed->hasFocus())
    ui->pm25_4_speed->setValue(iq_rc->getMaRc()->getPM25_4_speed());

    if(!ui->pm25_5_speed->hasFocus())
    ui->pm25_5_speed->setValue(iq_rc->getMaRc()->getPM25_5_speed());

    if(!ui->pm25_6_speed->hasFocus())
    ui->pm25_6_speed->setValue(iq_rc->getMaRc()->getPM25_6_speed());

    if(!ui->pm25_hesteresis_set->hasFocus())
    ui->pm25_hesteresis_set->setValue(iq_rc->getMaRc()->getPM25_hesteresis_set());

    if(!ui->pm25_measerument_time_set->hasFocus())
    ui->pm25_measerument_time_set->setValue(iq_rc->getMaRc()->getPM25_measerument_time_set());

    if(!ui->voc_red_ind->hasFocus())
    ui->voc_red_ind->setValue(iq_rc->getMaRc()->getVOC_red_ind());

    if(!ui->voc_yellow_ind->hasFocus())
    ui->voc_yellow_ind->setValue(iq_rc->getMaRc()->getVOC_yellow_ind());

    if(!ui->voc_green_ind->hasFocus())
    ui->voc_green_ind->setValue(iq_rc->getMaRc()->getVOC_green_ind());

    if(!ui->voc_motor_off->hasFocus())
    ui->voc_motor_off->setValue(iq_rc->getMaRc()->getVOC_motor_off());

    if(!ui->voc_1_speed->hasFocus())
    ui->voc_1_speed->setValue(iq_rc->getMaRc()->getVOC_1_speed());

    if(!ui->voc_2_speed->hasFocus())
    ui->voc_2_speed->setValue(iq_rc->getMaRc()->getVOC_2_speed());

    if(!ui->voc_3_speed->hasFocus())
    ui->voc_3_speed->setValue(iq_rc->getMaRc()->getVOC_3_speed());

    if(!ui->voc_4_speed->hasFocus())
    ui->voc_4_speed->setValue(iq_rc->getMaRc()->getVOC_4_speed());

    if(!ui->voc_5_speed->hasFocus())
    ui->voc_5_speed->setValue(iq_rc->getMaRc()->getVOC_5_speed());

    if(!ui->voc_6_speed->hasFocus())
    ui->voc_6_speed->setValue(iq_rc->getMaRc()->getVOC_6_speed());
}

void iq_autoindication::on_BacktoCtrl_clicked()
{
    this->hide();
    emit BacktoCtrl();
}

void iq_autoindication::on_pm25_red_ind_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setPM25_red_ind(ui->pm25_red_ind->value()));
    ui->pm25_red_ind->clearFocus();
}

void iq_autoindication::on_pm25_yellow_ind_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setPM25_yellow_ind(ui->pm25_yellow_ind->value()));
    ui->pm25_yellow_ind->clearFocus();
}

void iq_autoindication::on_pm25_green_ind_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setPM25_green_ind(ui->pm25_green_ind->value()));
    ui->pm25_green_ind->clearFocus();
}

void iq_autoindication::on_pm25_motor_off_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setPM25_motor_off(ui->pm25_motor_off->value()));
    ui->pm25_motor_off->clearFocus();
}

void iq_autoindication::on_pm25_1_speed_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setPM25_1_speed(ui->pm25_1_speed->value()));
    ui->pm25_1_speed->clearFocus();
}

void iq_autoindication::on_pm25_2_speed_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setPM25_2_speed(ui->pm25_2_speed->value()));
    ui->pm25_2_speed->clearFocus();
}

void iq_autoindication::on_pm25_3_speed_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setPM25_3_speed(ui->pm25_3_speed->value()));
    ui->pm25_3_speed->clearFocus();
}

void iq_autoindication::on_pm25_4_speed_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setPM25_4_speed(ui->pm25_4_speed->value()));
    ui->pm25_4_speed->clearFocus();
}

void iq_autoindication::on_pm25_5_speed_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setPM25_5_speed(ui->pm25_5_speed->value()));
    ui->pm25_5_speed->clearFocus();
}

void iq_autoindication::on_pm25_6_speed_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setPM25_6_speed(ui->pm25_6_speed->value()));
    ui->pm25_6_speed->clearFocus();
}

void iq_autoindication::on_pm25_hesteresis_set_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setPM25_hesteresis_set(ui->pm25_hesteresis_set->value()));
    ui->pm25_hesteresis_set->clearFocus();
}

void iq_autoindication::on_pm25_measerument_time_set_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setPM25_measerument_time_set(ui->pm25_measerument_time_set->value()));
    ui->pm25_measerument_time_set->clearFocus();
}

void iq_autoindication::on_voc_red_ind_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setVOC_red_ind(ui->voc_red_ind->value()));
    ui->voc_red_ind->clearFocus();
}

void iq_autoindication::on_voc_yellow_ind_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setVOC_yellow_ind(ui->voc_yellow_ind->value()));
    ui->voc_yellow_ind->clearFocus();
}

void iq_autoindication::on_voc_green_ind_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setVOC_green_ind(ui->voc_green_ind->value()));
    ui->voc_green_ind->clearFocus();
}

void iq_autoindication::on_voc_motor_off_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setVOC_motor_off(ui->voc_motor_off->value()));
    ui->voc_motor_off->clearFocus();
}

void iq_autoindication::on_voc_1_speed_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setVOC_1_speed(ui->voc_1_speed->value()));
    ui->voc_1_speed->clearFocus();
}

void iq_autoindication::on_voc_2_speed_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setVOC_2_speed(ui->voc_2_speed->value()));
    ui->voc_2_speed->clearFocus();
}

void iq_autoindication::on_voc_3_speed_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setVOC_3_speed(ui->voc_3_speed->value()));
    ui->voc_3_speed->clearFocus();
}

void iq_autoindication::on_voc_4_speed_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setVOC_4_speed(ui->voc_4_speed->value()));
    ui->voc_4_speed->clearFocus();
}

void iq_autoindication::on_voc_5_speed_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setVOC_5_speed(ui->voc_5_speed->value()));
    ui->voc_5_speed->clearFocus();
}

void iq_autoindication::on_voc_6_speed_editingFinished()
{
    iq_rc->sendOverBt(iq_rc->getMaRc()->setVOC_6_speed(ui->voc_6_speed->value()));
    ui->voc_6_speed->clearFocus();
}
