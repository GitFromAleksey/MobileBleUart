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

    connect(ui->listWidgetDevNames, SIGNAL(itemPressed(QListWidgetItem*)),
            this, SLOT(ListWidgetDeviceSelected(QListWidgetItem*)));

    m_BleDevTest = new cBleDev();
    connect(m_BleDevTest, &cBleDev::signalSendNewDeviceInfo,
            this, &MainWindow::slotPrintNewDeviceInfo);
    connect(m_BleDevTest, &cBleDev::signalLog,
            this, &MainWindow::slotLog);

    connect(m_BleDevTest, &cBleDev::signalReceiveBleData,
            this, &MainWindow::slotReceiveData);
}
// -----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}
// -----------------------------------------------------------------------------
void MainWindow::on_pushButton_clicked() // сканирование BLE устройств
{
//    m_ble.StartDiscovereDevices();
    ui->listWidgetDevNames->clear();
    ui->listWidget_2->clear();

    m_BleDevTest->StartFindDevices();
}
// -----------------------------------------------------------------------------
void MainWindow::on_pushButton_2_clicked() // отправка сообщения по BLE
{
//    m_ble.SendMessage("hello from mobile!\n");
//    ui->textBrowser_log->append("hello from mobile!");
    m_BleDevTest->Disconnect();
}

void MainWindow::ListWidgetDeviceSelected(QListWidgetItem* item)
{
    QString dev_info = m_ble.GetDevInfoByName(item->text());

    int row = ui->listWidgetDevNames->currentRow();

    m_cur_dev_name = item->text();
    m_cur_dev_addr = ui->listWidget_2->item(row)->text();
    p_ui->textBrowser->setText(m_cur_dev_name + "; " + m_cur_dev_addr);
}
// -----------------------------------------------------------------------------
void MainWindow::slotPrintNewDeviceInfo(const QString name, const QString address)
{
//    PrintNameOfDevice(name + ":" + address);
    p_ui->listWidgetDevNames->addItem(name);
    p_ui->listWidget_2->addItem(address);
}
// -----------------------------------------------------------------------------
void MainWindow::on_pushButtonConnect_clicked()
{
    m_BleDevTest->Connect(m_cur_dev_addr);
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void PrintNameOfDevice(const QString &text)
{
    p_ui->listWidgetDevNames->addItem(text);
//    p_ui->textBrowser->setText(text);
}
// -----------------------------------------------------------------------------
void ClearDeviceList()
{
    p_ui->listWidgetDevNames->clear();
    p_ui->textBrowser->clear();
}
// -----------------------------------------------------------------------------
void MainWindow::slotLog(const QString &text)
{
    ui->textBrowser_log->append(text);
}
void MainWindow::slotReceiveData(const QString &uuid, const QByteArray &data)
{
    ui->textBrowser_log->append("RX: uuid:" + uuid + "; data: " + QString(data));
}
// -----------------------------------------------------------------------------
void MainWindow::on_pushButtonConnect_2_clicked()
{
    QString s = ui->textEdit_send->toPlainText() + '\n';

    m_BleDevTest->TransmitBleData(s.toLocal8Bit());
}

// GET DESCRIBTION
void MainWindow::on_pushButtonConnect_3_clicked()
{
    QList<ServiceDescription> d = m_BleDevTest->GetConnectedDeviceDescribtion();

    int i = 0;
    ui->textBrowser_log->clear();
    for(auto service_it = d.begin(); service_it != d.end(); ++service_it)
    {
        ui->textBrowser_log->append(QString::number(i++) + ". Service");
        ui->textBrowser_log->append("  name:" + service_it->name);
        ui->textBrowser_log->append("  uuid:" + service_it->uuid);

        if(service_it->characteristics.size())
        {
            ui->textBrowser_log->append("   Characteristics:");
            for(auto char_it = service_it->characteristics.begin(); char_it != service_it->characteristics.end(); ++char_it)
            {
                ui->textBrowser_log->append("    uuid:" + char_it->uuid);
                for(auto prop_it = char_it->property_type.begin(); prop_it != char_it->property_type.end(); ++prop_it)
                {
                    ui->textBrowser_log->append("      property_type:" + *prop_it);
                }
            }
        }
    }

}

