#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private slots:

    void on_pushButton_clicked();
    void ListWidgetDeviceSelected(QListWidgetItem* item);

    void on_pushButton_2_clicked();

private:
    cBleDevice m_ble;
    cBleDev *m_BleDevTest;
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
