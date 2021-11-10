#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QWidget>
#include "remotecontroller.h"

namespace Ui {
class Configurator;
}

class Configurator : public QWidget
{
    Q_OBJECT

public:
    explicit Configurator(RemoteController *rc, QWidget *parent = 0);
    ~Configurator();

private slots:
    void on_contButton_clicked();

    void on_gateTestButton_clicked();

    void on_noTestButton_clicked();

    void on_heatTestButton_clicked();

    void updateParams();
    void on_filterButton_clicked();

    void on_soundButton_clicked();

    void on_hardResetButton_clicked();

signals:
    void contButton();
private:
    Ui::Configurator *ui;
    RemoteController *rc;
};

#endif // CONFIGURATOR_H
