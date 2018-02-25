#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <settingsdialog.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qsettings = new QSettings("Settings.ini",QSettings::IniFormat);
    //qsettings = new QSettings("ZayruAZ","IEC104 sim");
   //настройка драйвера для работы с МЭК
   // settings = new CSetting();
    driver = IEC104Driver::GetInstance();

    qsettings->beginGroup("driver");
    driver->SetSettings(new CSetting( qsettings->value("IP").toString(),
                                  qsettings->value("asdu").toInt(),
                                  qsettings->value("port").toInt()
                                        ));
    qsettings->endGroup();

    connect(driver,SIGNAL(Connected()),this,SLOT(OnConnected()));
    connect(driver,SIGNAL(Disconnected()),this,SLOT(OnDisconnected()));
    connect(driver,SIGNAL(Message(QString)),this,SLOT(LogReceived(QString)));
    connect(driver, SIGNAL(IECSignalReceived(CIECSignal*)),this,SLOT(IECReceived(CIECSignal*)));

    //создаем статус сообщение
    connectionStatusLabel= new QLabel();
    statusBar()->addWidget(connectionStatusLabel);

    //настройка таблицы сигналов контроля
    tabmodel = new TableModel();
    ui->MTable->setModel(tabmodel);

    tabmodel->setHeaderData(0,Qt::Horizontal,QVariant("IOA"));
    tabmodel->setHeaderData(1,Qt::Horizontal,QVariant("Название"));
    tabmodel->setHeaderData(2,Qt::Horizontal,QVariant("Значение"));
    tabmodel->setHeaderData(3,Qt::Horizontal,QVariant("Тип"));
    tabmodel->setHeaderData(4,Qt::Horizontal,QVariant("Качество"));
    tabmodel->setHeaderData(5,Qt::Horizontal,QVariant("Время"));

    emit tabmodel->headerDataChanged(Qt::Horizontal,0,5);

}

MainWindow::~MainWindow()
{
    delete ui;
}

///кнопка "соединение"
void MainWindow::OnConnectPressed(void)
{
    //IEC104Driver::GetInstance()->OpenConnection(settings->IP,settings->Port);
    ConnectionSettingsDialog *cdialog = new ConnectionSettingsDialog(qsettings);
    connect(cdialog, SIGNAL(accepted()),this,SLOT(OnConnectAck()));
    cdialog->exec();
}

void MainWindow::OnConnectAck(void)
{
    qsettings->beginGroup("driver");
    driver->SetSettings(new CSetting( qsettings->value("IP").toString(),
                                  qsettings->value("asdu").toInt(),
                                  qsettings->value("port").toInt()
                                        ));
    qsettings->endGroup();

    driver->OpenConnection();
}

///кнопка "разорвать соединение"
void MainWindow::OnDisconnectPressed(void)
{
    //IEC104Driver::GetInstance()->CloseConnection();
    driver->CloseConnection();

}

///вызов окна настроек
void MainWindow::OnSettingsPressed(void)
{
    ConnectionSettingsDialog *cdialog = new ConnectionSettingsDialog(qsettings);
    cdialog->exec();
}

///в случае успешного подключения драйвера
void MainWindow::OnConnected()
{
    connectionStatusLabel->setText("connected: "/*+settings->IP*/);
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
}

///при разрыве соединения драйвера
void MainWindow::OnDisconnected()
{
    connectionStatusLabel->setText("disconnected");
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);

    ui->log->append("Соединение разорвано");
}

///получено текстовое сообщение от драйвера
void MainWindow::LogReceived(QString text)
{
    ui->log->append(text);
}

///очистка окна логов
void MainWindow::OnClearLogPressed()
{
    ui->log->clear();
}

void MainWindow::MToolAdd()
{

}

void MainWindow::MToolRemove()
{

}

///от драйвера получен декодированный тег
void MainWindow::IECReceived(CIECSignal *tag)
{
    if (tabmodel->isSignalExist(tag))
        tabmodel->updateSignal(tag);

    else
    {
        tabmodel->appendSignal(tag);
       ui->MTable->setModel(0);
       ui->MTable->setModel(tabmodel);
    }
    tabmodel->redraw();
    ui->MTable->resizeRowsToContents();
}

void MainWindow::OnGIPressed()
{
    //driver->SendFullRequest(settings->asdu,20);
    qsettings->beginGroup("driver");
    driver->SendFullRequest(qsettings->value("asdu",QVariant(1)).toInt(),20);
    qsettings->endGroup();
}
