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

    m_BleDevTest = new cBleDev();
    m_BleDevTest->StartFindDevices();
}
// -----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}
// -----------------------------------------------------------------------------
void MainWindow::on_pushButton_clicked() // сканирование BLE устройств
{
    m_ble.StartDiscovereDevices();
}
// -----------------------------------------------------------------------------
void MainWindow::on_pushButton_2_clicked() // отправка сообщения по BLE
{
    m_ble.SendMessage("hello from mobile!\n");
//    ui->textBrowser_log->append("hello from mobile!");
}

void MainWindow::ListWidgetDeviceSelected(QListWidgetItem* item)
{
    QString dev_info = m_ble.GetDevInfoByName(item->text());

    m_ble.SetCurrentDeviceByName(item->text());
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

//void DeviceSelector::on_unpairButton_clicked()
//{
//    QList<QListWidgetItem *> items = ui->devicesList->findItems("IQ", Qt::MatchContains);
//    QString text, adress;
//    QBluetoothAddress devAddr;

//    for (int i = 0; i < items.size(); ++i)
//    {
//        text = items.at(i)->text();
//        adress = text.left(text.indexOf(" "));
//        devAddr = QBluetoothAddress(adress);
//        localDevice->requestPairing(devAddr, QBluetoothLocalDevice::Unpaired);
//    }
//}



