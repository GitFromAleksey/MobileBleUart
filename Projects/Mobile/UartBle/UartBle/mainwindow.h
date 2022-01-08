#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <qlistwidget.h>
#include "Ble/cbledevice_old.h"
#include "Ble/cbledev.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

    void on_pushButton_clicked();
    void ListWidgetDeviceSelected(QListWidgetItem* item);

    void on_pushButton_2_clicked();

    void slotPrintNewDeviceInfo(const QString name, const QString address);
    void on_pushButtonConnect_clicked();

    void slotLog(const QString &text);
    void slotReceiveData(const QString &uuid, const QByteArray &data);
private slots:
    void on_pushButtonConnect_2_clicked();

    void on_pushButtonConnect_3_clicked();

private:
    cBleDevice m_ble;
    cBleDev *m_BleDevTest;
    Ui::MainWindow *ui;
    QString m_cur_dev_addr;
    QString m_cur_dev_name;

};
#endif // MAINWINDOW_H
