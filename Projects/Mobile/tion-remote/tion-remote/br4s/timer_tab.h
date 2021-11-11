#ifndef TIMER_TAB_H
#define TIMER_TAB_H

#include <QWidget>
#include "br4s_mainwindow.h"
#include "br4s_protocol.h"

namespace Ui {
class timer_tab;
}

class timer_tab : public QWidget
{
    Q_OBJECT

public:
    explicit timer_tab(br4s_mainwindow *p_br4s_rc, QWidget *parent = nullptr, int index = 0);
    ~timer_tab();

public slots:
    void updateParams();

private slots:
    void on_timeEdit_1_editingFinished();

    void on_monday_box_1_clicked();

    void on_enable_checkbox_1_clicked();

    void on_tuesday_box_1_clicked();

    void on_wednesday_box_1_clicked();

    void on_thursday_box_1_clicked();

    void on_friday_box_1_clicked();

    void on_saturday_box_1_clicked();

    void on_sunday_box_1_clicked();

    void on_inflow_1_clicked();

    void on_recirculation_1_clicked();

    void on_heating_1_clicked();

    void on_maintenance_1_clicked();

    void on_PowerCheckBox_1_clicked();

    void on_sound_ind_checkbox_1_clicked();

    void on_LED_in_checkbox_1_clicked();

    void on_speed_slider_1_sliderReleased();

    void on_temp_slider_1_sliderReleased();

    void on_speed_slider_1_actionTriggered(int action);

    void on_temp_slider_1_actionTriggered(int action);

private:
    Ui::timer_tab *ui;
    br4s_mainwindow *br4s_rc;
    int my_index;
};

#endif // TIMER_TAB_H
