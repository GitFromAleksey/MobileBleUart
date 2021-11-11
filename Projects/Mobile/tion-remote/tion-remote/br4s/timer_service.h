#ifndef TIMER_SERVICE_H
#define TIMER_SERVICE_H

#include <QWidget>
#include "br4s_mainwindow.h"
#include "br4s_protocol.h"
#include "timer_tab.h"

namespace Ui {
class timer_service;
}

class timer_service : public QWidget
{
    Q_OBJECT

public:
    explicit timer_service(br4s_mainwindow *p_br4s_rc,QWidget *parent = nullptr);
    ~timer_service();

private slots:
    void on_back_to_ctrl_button_clicked();

signals:
    void BacktoCtrl();

private:
    Ui::timer_service *ui;
    br4s_mainwindow *br4s_rc;
    timer_tab *timers[NUM_OF_TIMERS];
};

#endif // TIMER_SERVICE_H
