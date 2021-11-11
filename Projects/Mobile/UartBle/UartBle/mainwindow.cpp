#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Ble/cbledevice.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    cBleDevice *ble = new cBleDevice();
    ble->StartDiscovereDevices();
    ble->SetScanResultsCallback(ui->textBrowser);
//    ui->textBrowser->setText("text");
}

