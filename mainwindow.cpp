#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QShortcut>
#include <QDebug>
#include <QDateTime>

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
    connect(pDriver, SIGNAL(IECSignalReceived(CIECSignal)),this,SLOT(IECReceived(CIECSignal)));

    connect(ui->action_LoadBase, SIGNAL(triggered(bool)),this, SLOT(OnLoadBaseTriggered(bool)));
    connect(ui->action_SaveBase, SIGNAL(triggered(bool)),this,SLOT(OnSaveBaseTriggered(bool)));
    connect(ui->action_LoadFile, SIGNAL(triggered(bool)),this,SLOT(OnLoadFileTriggered(bool)));

    connect(ui->actionAbout, SIGNAL(triggered(bool)),this,SLOT(OnAboutTriggered(bool)));


    connect(ui->actionCMD,SIGNAL(triggered(bool)),this,SLOT(OnCMDPressed()));
    //создаем статус сообщение
    pConnectionStatusLabel= new QLabel();
    statusBar()->addWidget(pConnectionStatusLabel);

    //настройка таблицы сигналов контроля
    tabmodel = new TableModel();

    proxyModel = new ProxyModel(this);
    proxyModel->setSourceModel(tabmodel);

    ui->MTable->setModel(proxyModel);
   // ui->MTable->setModel(tabmodel);

    connect(ui->lineEdit_nameFilter,SIGNAL(textChanged(QString)),proxyModel,SLOT(setFilter(QString)));
    tabmodel->setHeaderData(0,Qt::Horizontal,QVariant("IOA"));
    tabmodel->setHeaderData(1,Qt::Horizontal,QVariant("Название"));
    tabmodel->setHeaderData(2,Qt::Horizontal,QVariant("Значение"));
    tabmodel->setHeaderData(3,Qt::Horizontal,QVariant("Тип"));
    tabmodel->setHeaderData(4,Qt::Horizontal,QVariant("Качество"));
    tabmodel->setHeaderData(5,Qt::Horizontal,QVariant("Время"));

    emit tabmodel->headerDataChanged(Qt::Horizontal,0,5);
    connect(tabmodel,SIGNAL(rowsInserted(QModelIndex,int,int)),ui->MTable,SLOT(rowsInserted(QModelIndex,int,int)));
    connect(tabmodel,SIGNAL(rowsRemoved(QModelIndex,int,int)),ui->MTable, SLOT(rowsAboutToBeRemoved(QModelIndex,int,int)));

    ui->MTable->verticalHeader()->setDefaultSectionSize(20);

#ifdef FILL_TEST_TABLE
    for (int i=1; i<10; i++)
        tabmodel->updateSignal(new CIECSignal(i,30,"test"));
#endif

    QShortcut* shortcut = new QShortcut(QKeySequence(QKeySequence::Delete), ui->MTable);
    connect(shortcut, SIGNAL(activated()), ui->MTool_remove, SLOT(click()));


    QString logFileName(QDateTime::currentDateTime().toString() + ".txt");
    logFileName= logFileName.replace(':','_');

    logFile = new QFile(logFileName);

    qDebug() << "New log: " << logFileName;
    logFile->open(QIODevice::ReadWrite | QIODevice::Text);



}

MainWindow::~MainWindow()
{
    delete ui;
}

///кнопка "соединение"
void MainWindow::OnConnectPressed(void)
{
    if (pConnectionDialog!=0)
    {
        delete pConnectionDialog;
        pConnectionDialog = 0;
    }

        pConnectionDialog = new ConnectionSettingsDialog(qsettings);
        connect(pConnectionDialog, SIGNAL(SettingsAccepted()), this, SLOT(OnConnectionDialogFinished()));

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

///
/// \brief Окно "соединение" закрыто с результатом "ок" - устанавливаем соединение
///
void MainWindow::OnConnectionDialogFinished(/*int result*/)
{
    qDebug() << "OnConnectionDialogFinished";
    pDriver->SetSettings(qsettings);
    pDriver->OpenConnection();
   //delete pConnectionDialog;
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
     QTextStream logStream(logFile);
    logStream << text << '\n';
    ui->log->append(text);
   //qDebug() << text;
}


///очистка окна логов
void MainWindow::OnClearLogPressed()
{
    ui->log->clear();
}

///добавление в список сигналов через форму add
void MainWindow::MToolAdd_Accept()
{

    //if (pAddSignalDialog->tag != NULL)
    //{
       // AddMSignal(pAddSignalDialog->tag,pAddSignalDialog->description);


        tabmodel->updateSignal(pAddSignalDialog->tag,true);

      //  ui->MTable->setModel(0);
       // ui->MTable->setModel(tabmodel);
      //  tabmodel->redraw();
      //  ui->MTable->resizeRowsToContents();


    //}
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

  //  ui->MTable->setModel(0);
   // ui->MTable->setModel(tabmodel);
  //  tabmodel->redraw();
  //  ui->MTable->resizeRowsToContents();
}

///добавление нового сигнала в список

///от драйвера получен декодированный тег
void MainWindow::IECReceived(CIECSignal tag)
{



        bool autoCreate;
        qsettings->beginGroup("driver");
            autoCreate = qsettings->value("autoCreate",QVariant(false)).toBool();
        qsettings->endGroup();

         tabmodel->updateSignal(tag, autoCreate);

}

void MainWindow::OnGIPressed()
{
    if (pDriver!=0)
    pDriver->SendFullRequest(20);
}
void MainWindow::OnSaveBaseTriggered(bool)
{
    qDebug() << "save base";
    QFileDialog *fileDialog = new QFileDialog();

    QString filename = fileDialog->getSaveFileName(this,"Сохранение",QString(),QString("*.dat")); //fileDialog->getOpenFileName(this,"Открытие файла",QString(),QString("*.xls *.xlsx"));
    qDebug() << filename;
    if (filename.length()>0)
    {
        QFile *file = new QFile(filename);
        file->open(QIODevice::ReadWrite);
        QTextStream stream(file);
        foreach(CIECSignal signal, tabmodel->mData)
        {
            stream << signal.GetKey() << '\\' << signal.description << '\n';

        }
        file->close();
        delete file;

    }
    delete fileDialog;
}

void MainWindow::OnLoadFileTriggered(bool)
{
  QFileDialog *fileDialog = new QFileDialog();


  QString filename = fileDialog->getOpenFileName(this,"Открытие",QString(),QString("*.dat"));
  QString key;
  QString descr;
  if (filename.length()>0)
  {
      QFile *file= new QFile(filename);
      file->open(QIODevice::ReadOnly);
      QTextStream in(file);
      while(!in.atEnd())
      {
        QString line = in.readLine();
        qDebug() << line;
        QStringList words= line.split('\\');
        key = words[0];

        CIECSignal sig;
        sig.SetKey( key.toUInt());
        sig.description = words[1];
        tabmodel->updateSignal(sig,true,true);
      }

      file->close();
      delete file;

  }

  delete fileDialog;
}

void MainWindow::OnLoadBaseTriggered(bool val)
{
    Q_UNUSED(val);
  //  QMessageBox::warning(this,"load base","load base");
  // if (val)
  // {
       ImportDialog *id = new ImportDialog(qsettings);
       id->SetModel(this->tabmodel);
       id->exec();
  // }
}

void MainWindow::OnCMDPressed()
{
    if (pDialog!=0)
        delete pDialog;

    pDialog = new CmdDialog(pDriver, qsettings,this);
pDialog->show();
//pDialog->exec();
}

void MainWindow::OnAboutTriggered(bool)
{
    aboutDialog *dialog = new aboutDialog();
    dialog->show();
    //delete dialog;
}
