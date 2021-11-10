#ifndef IQ_MAINWINDOW_H
#define IQ_MAINWINDOW_H

#include "lib/tiondevicebaselogic.h"
#include "iq_protocol.h"
#include "lib/maupdater.h"

#include <QWidget>
#include <QBoxLayout>

namespace Ui {
class iq_mainwindow;
}

class iq_mainwindow : public TionDeviceBaseLogic
{
    Q_OBJECT

public:
    explicit iq_mainwindow(QWidget *parent = nullptr, QVBoxLayout* layout = 0);
    ~iq_mainwindow();

    virtual void updateDeviceParams(const QLowEnergyCharacteristic &c,
                                                     const QByteArray &value) override ;
    virtual IQ_Rc *getMaRc() override ;

signals:
    void discButton();
    void updParam();
    void go_to_service();
    void go_to_autoind();

private slots:
    void on_discButton_clicked();

    void on_powerOn_clicked();

    void on_ChildLock_clicked();

    void on_sleepTimerValue_editingFinished();

    void on_sleepTimerOn_clicked();

    void on_speed_slider_actionTriggered(int action);

    void on_srvBtn_clicked();

    void on_autoIndBtn_clicked();

    void on_updButton_clicked();

    virtual void sendRequest() override;

    void on_speed_slider_sliderReleased();



private:
    void setUiState(bool st);
    Ui::iq_mainwindow *ui;
    IQ_Rc IQ_rc;
    MAUpdater maUpd;
    QVBoxLayout* parent_layout;
};

#endif // IQ_MAINWINDOW_H
