#ifndef IQ_SERVICE_H
#define IQ_SERVICE_H

#include <QWidget>
#include "iq_mainwindow.h"
#include "iq_protocol.h"

namespace Ui {
class iq_service;
}

class iq_service : public QWidget
{
    Q_OBJECT

public:
    explicit iq_service(iq_mainwindow *p_iq_rc = nullptr, QWidget *parent = nullptr);
    ~iq_service();

private slots:
    void on_BacktoCtrl_clicked();

    void on_FilterResetBut_clicked();

    void on_SoundOn_clicked();

    void on_MAcontrol_clicked();

    void on_MAcontrol_auto_clicked();

    void on_SetResetBut_clicked();

    void updateParams();

    void on_LED_night_clicked();

    void on_LED_off_clicked();

    void on_LED_50_clicked();

    void on_LED_100_clicked();

    void on_ErrResetBtn_clicked();

signals:
    void BacktoCtrl();

private:
    Ui::iq_service *ui;
    iq_mainwindow *iq_rc;
};

#endif // IQ_SERVICE_H
