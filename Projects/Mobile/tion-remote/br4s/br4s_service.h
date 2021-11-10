#ifndef BR4S_SERVICE_H
#define BR4S_SERVICE_H

#include <QWidget>
#include "br4s_mainwindow.h"
#include "br4s_protocol.h"

namespace Ui {
class br4s_service;
}

class br4s_service : public QWidget
{
    Q_OBJECT

public:
    explicit br4s_service(br4s_mainwindow *p_br4s_rc,QWidget *parent = nullptr);
    ~br4s_service();

private slots:
    void on_BacktoCtrl_clicked();

    void on_FilterResetBut_clicked();

    void on_SoundOn_clicked();

    void on_LEDOn_clicked();

    void on_MAcontrol_clicked();

    void on_MAcontrol_auto_clicked();

    void on_TestOff_clicked();

    void on_SetResetBut_clicked();

    void updateParams();

    void on_DefTest_clicked();

    void on_HeaterTest_clicked();

signals:
    void BacktoCtrl();

private:
    Ui::br4s_service *ui;
    br4s_mainwindow *br4s_rc;
};

#endif // BR4S_SERVICE_H
