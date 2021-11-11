#include "timer_tab.h"
#include "ui_timer_tab.h"

timer_tab::timer_tab(br4s_mainwindow *p_br4s_rc, QWidget *parent, int index) :
    QWidget(parent),
    ui(new Ui::timer_tab),
    br4s_rc(p_br4s_rc),
    my_index(index)
{
    ui->setupUi(this);
}

timer_tab::~timer_tab()
{
    delete ui;
}


void timer_tab::updateParams()
{
    ui->enable_checkbox_1->setChecked(br4s_rc->getMaRc()->getTimerEnable(my_index));
    ui->PowerCheckBox_1->setChecked(br4s_rc->getMaRc()->getTimerPowerState(my_index));
    ui->sound_ind_checkbox_1->setChecked(br4s_rc->getMaRc()->getTimerSndIndState(my_index));
    ui->LED_in_checkbox_1->setChecked(br4s_rc->getMaRc()->getTimerLEDIndState(my_index));

    if (!ui->timeEdit_1->hasFocus())
        ui->timeEdit_1->setTime(br4s_rc->getMaRc()->getTimerTime(my_index));

    ui->monday_box_1->setChecked(br4s_rc->getMaRc()->getTimerMonday(my_index));
    ui->tuesday_box_1->setChecked(br4s_rc->getMaRc()->getTimerTuesday(my_index));
    ui->wednesday_box_1->setChecked(br4s_rc->getMaRc()->getTimerWednesday(my_index));
    ui->thursday_box_1->setChecked(br4s_rc->getMaRc()->getTimerThursday(my_index));
    ui->friday_box_1->setChecked(br4s_rc->getMaRc()->getTimerFriday(my_index));
    ui->saturday_box_1->setChecked(br4s_rc->getMaRc()->getTimerSaturday(my_index));
    ui->sunday_box_1->setChecked(br4s_rc->getMaRc()->getTimerSunday(my_index));

    if(!ui->speed_slider_1->isSliderDown())
        ui->speed_slider_1->setSliderPosition(br4s_rc->getMaRc()->getTimerTargetSpeed(my_index));
    if(!ui->temp_slider_1->isSliderDown())
        ui->temp_slider_1->setSliderPosition(br4s_rc->getMaRc()->getTimerTargetTemp(my_index));

    QString label = QString("Speed: %1").arg(br4s_rc->getMaRc()->getTimerTargetSpeed(my_index));
    ui->speedLabel_1->setText(label);
    label = QString("Temperature: %1").arg(br4s_rc->getMaRc()->getTimerTargetTemp(my_index));
    ui->temperatureLabel_1->setText(label);

    if (br4s_rc->getMaRc()->getTimerSubstate(my_index) == 0)
    {
        ui->inflow_1->setChecked(true);
        ui->recirculation_1->setChecked(false);
    }
    else if (br4s_rc->getMaRc()->getTimerSubstate(my_index) == 1)
    {
        ui->inflow_1->setChecked(false);
        ui->recirculation_1->setChecked(true);
    }
    ui->speed_slider_1->setEnabled(br4s_rc->getMaRc()->getTimerPowerState(0));

    if (br4s_rc->getMaRc()->getTimerHeaterMode(my_index) == 1)
    {
        ui->heating_1->setChecked(false);
        ui->maintenance_1->setChecked(true);
    }
    else if (br4s_rc->getMaRc()->getTimerHeaterMode(my_index) == 0)
    {
        ui->heating_1->setChecked(true);
        ui->maintenance_1->setChecked(false);
    }
    ui->temp_slider_1->setEnabled(br4s_rc->getMaRc()->getTimerPowerState(my_index) &
                                   !br4s_rc->getMaRc()->getTimerSubstate(my_index));
}

void timer_tab::on_enable_checkbox_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerEnable(
                            ui->enable_checkbox_1->isChecked(), 0));
}

void timer_tab::on_timeEdit_1_editingFinished()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerTime(
                            ui->timeEdit_1->time(), my_index));
    ui->timeEdit_1->clearFocus();
}

void timer_tab::on_monday_box_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerMonday(
                        ui->monday_box_1->isChecked(), my_index));
}

void timer_tab::on_tuesday_box_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerTuesday(
                        ui->tuesday_box_1->isChecked(), my_index));
}

void timer_tab::on_wednesday_box_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerWednesday(
                        ui->wednesday_box_1->isChecked(), my_index));
}

void timer_tab::on_thursday_box_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerThursday(
                        ui->thursday_box_1->isChecked(), my_index));
}

void timer_tab::on_friday_box_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerFriday(
                        ui->friday_box_1->isChecked(), my_index));
}

void timer_tab::on_saturday_box_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerSaturday(
                        ui->saturday_box_1->isChecked(), my_index));
}

void timer_tab::on_sunday_box_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerSunday(
                        ui->sunday_box_1->isChecked(), my_index));
}

void timer_tab::on_inflow_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerSubstate(0, my_index));
}

void timer_tab::on_recirculation_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerSubstate(1, my_index));
}

void timer_tab::on_heating_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerHeaterMode(0, my_index));
}

void timer_tab::on_maintenance_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerHeaterMode(1, my_index));
}

void timer_tab::on_PowerCheckBox_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerPowerState(
                            ui->PowerCheckBox_1->isChecked(), my_index));
}

void timer_tab::on_sound_ind_checkbox_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerSndIndState(
                            ui->sound_ind_checkbox_1->isChecked(), my_index));
}

void timer_tab::on_LED_in_checkbox_1_clicked()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerLEDIndState(
                            ui->LED_in_checkbox_1->isChecked(), my_index));
}

void timer_tab::on_speed_slider_1_sliderReleased()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerTargetSpeed(
                 static_cast<uint8_t>( ui->speed_slider_1->sliderPosition()), my_index));
}

void timer_tab::on_temp_slider_1_sliderReleased()
{
    br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerTargetTemp(
                 static_cast<int8_t>( ui->temp_slider_1->sliderPosition()), my_index));
}

void timer_tab::on_speed_slider_1_actionTriggered(int action)
{
    if (action == QAbstractSlider::SliderPageStepAdd ||
        action == QAbstractSlider::SliderPageStepSub ||
        action == QAbstractSlider::SliderSingleStepSub ||
        action == QAbstractSlider::SliderSingleStepAdd)
    {
        br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerTargetSpeed(
                     static_cast<uint8_t>( ui->speed_slider_1->sliderPosition()), my_index));
    }
}

void timer_tab::on_temp_slider_1_actionTriggered(int action)
{
    if (action == QAbstractSlider::SliderPageStepAdd ||
        action == QAbstractSlider::SliderPageStepSub ||
        action == QAbstractSlider::SliderSingleStepSub ||
        action == QAbstractSlider::SliderSingleStepAdd)
    {
        br4s_rc->sendOverBt(br4s_rc->getMaRc()->setTimerTargetTemp(
                     static_cast<int8_t>( ui->temp_slider_1->sliderPosition()), my_index));
    }
}

