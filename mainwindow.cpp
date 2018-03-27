#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qsettings = new QSettings("Settings.ini",QSettings::IniFormat);
    //qsettings = new QSettings("ZayruAZ","IEC104 sim");
   //настройка драйвера для работы с МЭК
   // settings = new CSetting();
    pDriver = IEC104Driver::GetInstance();


    //driver->SetSettings(new CSetting( qsettings->value("IP").toString(),
   //                               qsettings->value("asdu").toInt(),
   //                               qsettings->value("port").toInt()
   //                                     ));
   // qsettings->endGroup();
    pDriver->SetSettings(qsettings);

    connect(pDriver,SIGNAL(Connected()),this,SLOT(OnConnected()));
    connect(pDriver,SIGNAL(Disconnected()),this,SLOT(OnDisconnected()));
    connect(pDriver,SIGNAL(Message(QString)),this,SLOT(LogReceived(QString)));
    connect(pDriver, SIGNAL(IECSignalReceived(CIECSignal*)),this,SLOT(IECReceived(CIECSignal*)));


    //создаем статус сообщение
    pConnectionStatusLabel= new QLabel();
    statusBar()->addWidget(pConnectionStatusLabel);

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
    pConnectionDialog = new ConnectionSettingsDialog(qsettings);
  //  connect(pConnectionDialog, SIGNAL(accepted()),this,SLOT(OnConnectAck()));
    connect(pConnectionDialog, SIGNAL(finished(int)),this, SLOT(OnConnectionDialogFinished(int)));
    pConnectionDialog->exec();
}
//
void MainWindow::OnConnectAck(void)
{
   /* qsettings->beginGroup("driver");
    driver->SetSettings(new CSetting( qsettings->value("IP").toString(),
                                  qsettings->value("asdu").toInt(),
                                  qsettings->value("port").toInt()
                                        ));
    qsettings->endGroup();
*/
    pDriver->SetSettings(qsettings);
    pDriver->OpenConnection();
}
void MainWindow::OnConnectionDialogFinished(int result)
{
    qDebug() << result;
    pDriver->SetSettings(qsettings);
    pDriver->OpenConnection();
    delete pConnectionDialog;
}

///кнопка "разорвать соединение"
void MainWindow::OnDisconnectPressed(void)
{
    //IEC104Driver::GetInstance()->CloseConnection();
    pDriver->CloseConnection();

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
    pConnectionStatusLabel->setText("connected: "/*+settings->IP*/);
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
}

///при разрыве соединения драйвера
void MainWindow::OnDisconnected()
{
    pConnectionStatusLabel->setText("disconnected");
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);

    ui->log->append("Соединение разорвано");
}

///получено текстовое сообщение от драйвера
void MainWindow::LogReceived(QString text)
{
    ui->log->append(text);
}

void MainWindow::AddMSignal(CIECSignal *tag)
{
    tabmodel->appendSignal(tag);
    ui->MTable->setModel(0);
    ui->MTable->setModel(tabmodel);
    tabmodel->redraw();
    ui->MTable->resizeRowsToContents();
}

///очистка окна логов
void MainWindow::OnClearLogPressed()
{
    ui->log->clear();
}

//accept adding new signal from AddSignalDialog
void MainWindow::MToolAdd_Accept()
{

    if ((pAddSignalDialog->tag != NULL)&&(!tabmodel->isSignalExist(pAddSignalDialog->tag)))
    {
        AddMSignal(pAddSignalDialog->tag);
    }
    delete pAddSignalDialog;
    qDebug()<< "accept";
}

void MainWindow::MToolAdd_Pressed()
{
    pAddSignalDialog = new addSignalDialog();
    connect(pAddSignalDialog,SIGNAL(accepted()),this,SLOT(MToolAdd_Accept()));
    pAddSignalDialog->exec();
}


void MainWindow::MToolRemove_Pressed()
{
    ui->log->append("remove pressed");
    QItemSelectionModel *pSelection =  ui->MTable->selectionModel();

    tabmodel->removeRows(pSelection);

    ui->MTable->setModel(0);
    ui->MTable->setModel(tabmodel);
    tabmodel->redraw();
    ui->MTable->resizeRowsToContents();
}

///от драйвера получен декодированный тег
void MainWindow::IECReceived(CIECSignal *tag)
{
    if (tabmodel->isSignalExist(tag))
        tabmodel->updateSignal(tag);

    else
    {
        //add new row, reload table
        AddMSignal(tag);
    }
    tabmodel->redraw();
    ui->MTable->resizeRowsToContents();
}

void MainWindow::OnGIPressed()
{
    //driver->SendFullRequest(settings->asdu,20);
    qsettings->beginGroup("driver");
    pDriver->SendFullRequest(qsettings->value("asdu",QVariant(1)).toInt(),20);
    qsettings->endGroup();
}
