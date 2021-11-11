#include "mainwindow.h"
#include "ui_mainwindow.h"


//MainWindow *p_main_window;
Ui::MainWindow *p_ui;
void PrintNameOfDevice(const QString &text);
void ClearDeviceList();

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    p_main_window = this;
    p_ui = ui;
    m_ble.SetScanResultsCallback(PrintNameOfDevice);
    m_ble.SetClearDevListCallback(ClearDeviceList);

    connect(ui->listWidget, SIGNAL(itemPressed(QListWidgetItem*)),
            this, SLOT(ListWidgetDeviceSelected(QListWidgetItem*)));
}
// -----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}
// -----------------------------------------------------------------------------
void MainWindow::on_pushButton_clicked()
{
    m_ble.StartDiscovereDevices();
}
// -----------------------------------------------------------------------------
void MainWindow::ListWidgetDeviceSelected(QListWidgetItem* item)
{
    QString dev_info = m_ble.GetDevInfoByName(item->text());

    p_ui->textBrowser->setText(dev_info);
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PrintNameOfDevice(const QString &text)
{
    p_ui->listWidget->addItem(text);
//    p_ui->textBrowser->setText(text);
}
// -----------------------------------------------------------------------------
void ClearDeviceList()
{
    p_ui->listWidget->clear();
    p_ui->textBrowser->clear();
}
