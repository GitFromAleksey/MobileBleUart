#ifndef BR4S_MAINWINDOW_H
#define BR4S_MAINWINDOW_H



#include "lib/tiondevicebaselogic.h"
#include "br4s_protocol.h"
#include "lib/maupdater.h"

#include <QWidget>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QBluetoothAddress>
#include <QTimer>
#include <QBoxLayout>

namespace Ui {
class br4s_mainwindow;
}

class br4s_mainwindow : public TionDeviceBaseLogic
{
    Q_OBJECT

public:
    explicit br4s_mainwindow(QWidget *parent = 0, QVBoxLayout* layout = 0);
    ~br4s_mainwindow();
    virtual void updateDeviceParams(const QLowEnergyCharacteristic &c,
                                    const QByteArray &value) override ;
    virtual BR4S_Rc *getMaRc() override ;


signals:
    void br4s_optButton();
    void br4s_timButton();
    void discButton();
    void updParam();

private slots:
    void on_PowerCheckBox_clicked();

    void on_speed_slider_sliderReleased();

    void on_temp_slider_sliderReleased();

    void on_br4s_optButton_clicked();
    void on_updButton_clicked();

    void on_inflow_clicked();

    void on_recirculation_clicked();

    void on_DiscButton_clicked();

    void on_heating_clicked();

    void on_maintenance_clicked();

    void timer_timeout_handler();

    void on_TimButton_clicked();

    void on_dateTimeEdit_editingFinished();

    void on_speed_slider_actionTriggered(int action);

    void on_temp_slider_actionTriggered(int action);

    void on_turbo_btn_clicked();

    void on_TurboTimeSpinBox_editingFinished();

    virtual void sendRequest() override;
private:
    void setUiState(bool st);
    Ui::br4s_mainwindow* ui;
    BR4S_Rc BR4S_Rc;
    MAUpdater maUpd;
    QVBoxLayout* parent_layout;
};

#endif // BR4S_MAINWINDOW_H
