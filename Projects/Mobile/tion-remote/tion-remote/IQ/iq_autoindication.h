#ifndef IQ_AUTOINDICATION_H
#define IQ_AUTOINDICATION_H

#include <QWidget>
#include "iq_mainwindow.h"
namespace Ui {
class iq_autoindication;
}

class iq_autoindication : public QWidget
{
    Q_OBJECT

public:
    explicit iq_autoindication(iq_mainwindow *p_iq_rc,QWidget *parent = nullptr);
    ~iq_autoindication();

private slots:
    void updateParams();

    void on_BacktoCtrl_clicked();

    void on_pm25_red_ind_editingFinished();

    void on_pm25_yellow_ind_editingFinished();

    void on_pm25_green_ind_editingFinished();

    void on_pm25_motor_off_editingFinished();

    void on_pm25_1_speed_editingFinished();

    void on_pm25_2_speed_editingFinished();

    void on_pm25_3_speed_editingFinished();

    void on_pm25_4_speed_editingFinished();

    void on_pm25_5_speed_editingFinished();

    void on_pm25_6_speed_editingFinished();

    void on_pm25_hesteresis_set_editingFinished();

    void on_pm25_measerument_time_set_editingFinished();

    void on_voc_red_ind_editingFinished();

    void on_voc_yellow_ind_editingFinished();

    void on_voc_green_ind_editingFinished();

    void on_voc_motor_off_editingFinished();

    void on_voc_1_speed_editingFinished();

    void on_voc_2_speed_editingFinished();

    void on_voc_3_speed_editingFinished();

    void on_voc_4_speed_editingFinished();

    void on_voc_5_speed_editingFinished();

    void on_voc_6_speed_editingFinished();

signals:
    void BacktoCtrl();

private:
    Ui::iq_autoindication *ui;
    iq_mainwindow *iq_rc;
};

#endif // IQ_AUTOINDICATION_H
