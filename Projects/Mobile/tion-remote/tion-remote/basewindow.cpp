#include "basewindow.h"
#include "ui_basewindow.h"
#include "brlt/remotecontroller.h"
#include "deviceselector.h"
#include "brlt/configurator.h"
#include "br4s/br4s_mainwindow.h"
#include "br4s/br4s_service.h"
#include "br4s/timer_service.h"
#include <QVBoxLayout>

BaseWindow::BaseWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BaseWindow)
{
    ui->setupUi(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    DeviceSelector* devsel = new DeviceSelector(this, layout);

    layout->addWidget(devsel);
    devsel->show();
}

BaseWindow::~BaseWindow()
{
    delete ui;
}
