#ifndef REMOTECONTROLLER_H
#define REMOTECONTROLLER_H

#include <QWidget>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QBluetoothAddress>
#include "maprotocol.h"
#include "lib/firmware.h"
#include "lib/tiondevicebaselogic.h"
#include <QTimer>
#include <QVBoxLayout>

namespace Ui {
class RemoteController;
}

class RemoteController : public TionDeviceBaseLogic
{
    Q_OBJECT

public:
    explicit RemoteController(QWidget *parent = 0, QVBoxLayout* layout = 0);
    ~RemoteController();
    virtual void updateDeviceParams(const QLowEnergyCharacteristic &c,
                                    const QByteArray &value) override;
    virtual MaRc *getMaRc() override;


signals:
    void optButton();
    void discButton();
    void updParam();

private slots:
    void on_disconnectButton_clicked();

    virtual void sendRequest() override;

    void on_powerSwitcher_clicked();

    void on_heaterSwitcher_clicked();

    void on_speedSlider_sliderReleased();

    void on_tempSlider_sliderReleased();

    void on_optButton_clicked();
    void on_updButton_clicked();

    void timer_timeout_handler();

private:
    void setUiState(bool st);
    Ui::RemoteController* ui;
    MaRc maRc;
    MAUpdater maUpd;
    QVBoxLayout* parent_layout;
};

#endif // REMOTECONTROLLER_H
