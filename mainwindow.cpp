#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QSet>
#include <algorithm>
#include <QAction>
#include <QAbstractItemView>
#include <QTextDocument>
#include <QTextCursor>
#include <QApplication>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qsettings = new QSettings("Settings.ini",QSettings::IniFormat);
   //настройка драйвера для работы с МЭК
    pDriver = IEC104Driver::GetInstance();


    pDriver->SetSettings(qsettings);

    connect(pDriver,SIGNAL(Connected()),this,SLOT(OnConnected()));

    pConnectionStatusLabel= new QLabel();
    statusBar()->addWidget(pConnectionStatusLabel);

    pConnectionStatusLabel->setText("Disconnected");

    pConnectionStatusLabel->setStyleSheet("color: red;");


    connect(pDriver, &IEC104Driver::Connecting, [this](){
        pConnectionStatusLabel->setText("Connecting");
        pConnectionStatusLabel->setStyleSheet("color: orange;");
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
    });

    connect(pDriver,SIGNAL(Disconnected()),this,SLOT(OnDisconnected()));
    connect(pDriver,SIGNAL(Message(QString)),this,SLOT(LogReceived(QString)));
    connect(pDriver, SIGNAL(IECSignalReceived(CIECSignal)),this,SLOT(IECReceived(CIECSignal)));

    connect(ui->action_LoadBase, SIGNAL(triggered(bool)),this, SLOT(OnLoadBaseTriggered(bool)));
    connect(ui->action_SaveBase, SIGNAL(triggered(bool)),this,SLOT(OnSaveBaseTriggered(bool)));
    connect(ui->action_LoadFile, SIGNAL(triggered(bool)),this,SLOT(OnLoadFileTriggered(bool)));
    connect(ui->actionWatch,SIGNAL(triggered(bool)),this,SLOT(OnShowWatchTriggered(bool)));
    connect(ui->actionAbout, SIGNAL(triggered(bool)),this,SLOT(OnAboutTriggered(bool)));

    connect(ui->action_3, SIGNAL(toggled(bool)), this,SLOT(OnLogVisibleToggled(bool)));

    connect(ui->actionTS, SIGNAL(triggered(bool)),pDriver, SLOT(ClockSynch(void)));


    connect(ui->actionCMD,SIGNAL(triggered(bool)),this,SLOT(OnCMDPressed()));
    //создаем статус сообщение


    //настройка таблицы сигналов контроля
    tabmodel = new TableModel();

    proxyModel = new ProxyModel(this);
    proxyModel->setSourceModel(tabmodel);

    ui->MTable->setModel(proxyModel);
   // ui->MTable->setModel(tabmodel);

    connect(ui->lineEdit_nameFilter,SIGNAL(textChanged(QString)),proxyModel,SLOT(setFilter(QString)));

    // Headers come from TableModel::headerData (IOA, Description, Type, Value, Quality, Time)
    emit tabmodel->headerDataChanged(Qt::Horizontal,0,5);
    connect(tabmodel,SIGNAL(rowsInserted(QModelIndex,int,int)),ui->MTable,SLOT(rowsInserted(QModelIndex,int,int)));
    connect(tabmodel,SIGNAL(rowsRemoved(QModelIndex,int,int)),ui->MTable, SLOT(rowsAboutToBeRemoved(QModelIndex,int,int)));

    ui->MTable->verticalHeader()->setDefaultSectionSize(20);
    ui->MTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->MTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->MTable->setSortingEnabled(true);

    connect(ui->MTable, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(OnContextMenuRequested(QPoint)));

#ifdef FILL_TEST_TABLE
    for (int i=1; i<10; i++)
        tabmodel->updateSignal(new CIECSignal(i,30,"test"));
#endif

    // Delete key: single QAction only (QShortcut + QAction caused Ambiguous shortcut overload)
    removeAction = new QAction(tr("Delete"), this);
    removeAction->setShortcut(QKeySequence::Delete);
    removeAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    ui->MTable->addAction(removeAction);
    connect(removeAction, SIGNAL(triggered()), this, SLOT(MToolRemove_Pressed()));



     watch = new WatchDialog(pDriver,this);

     //commandList = new QList<CIECSignal>();
    cmdTableModel = new TableModel();
    cmdTableModel->isShortTable = true;

    qsettings->beginGroup("driver");
    bool go = qsettings->value("autoStart",false).toBool();
    qsettings->endGroup();
   if (go)
   {

       OnConnectionDialogFinished();
   }


}
void MainWindow::OnContextMenuRequested(QPoint pos)
{
    QMenu *contextMenu= new QMenu(this);
    contextMenu->setAttribute(Qt::WA_DeleteOnClose);

    QAction *addWatch=new QAction(tr("Add to watch"), contextMenu);
    connect(addWatch,SIGNAL(triggered(bool)), this, SLOT(OnAddWatch(bool)));

    QAction *read= new QAction(tr("Single request"), contextMenu);
    connect(read, SIGNAL(triggered(bool)), this, SLOT(OnRead(bool)));

    QAction *remove = new QAction(tr("Delete"), contextMenu);
    connect(remove, SIGNAL(triggered()), this, SLOT(MToolRemove_Pressed()));

    contextMenu->addAction(addWatch);
    contextMenu->addAction(read);
    contextMenu->addSeparator();
    contextMenu->addAction(remove);

    contextMenu->popup(ui->MTable->viewport()->mapToGlobal(pos));
}
void MainWindow::OnAddWatch(bool)
{
    if (!watch)
        return; //no watch window, exiting

     QItemSelectionModel *pSelection =  ui->MTable->selectionModel();
     QModelIndexList indexes= pSelection->selectedIndexes();
     QSet<int> doneRows;
     foreach(QModelIndex index, indexes)
     {
         QModelIndex sourceIndex = proxyModel->mapToSource(index);
         if (doneRows.contains(sourceIndex.row()))
             continue;
         doneRows.insert(sourceIndex.row());
         if (sourceIndex.row() < 0 || sourceIndex.row() >= tabmodel->mData.count())
             continue;
         CIECSignal s =tabmodel->mData[sourceIndex.row()];
         watch->AddWatch(s);
     }
}

void MainWindow::OnRead(bool)
{
    QItemSelectionModel *pSelection =  ui->MTable->selectionModel();
    QModelIndexList indexes= pSelection->selectedIndexes();
    QSet<int> doneRows;
    foreach(QModelIndex index, indexes)
    {
        QModelIndex sourceIndex = proxyModel->mapToSource(index);
        if (doneRows.contains(sourceIndex.row()))
            continue;
        doneRows.insert(sourceIndex.row());
        if (sourceIndex.row() < 0 || sourceIndex.row() >= tabmodel->mData.count())
            continue;
        CIECSignal s =tabmodel->mData[sourceIndex.row()];
        pDriver->ReadIOA(s.GetAddress());
    }
}
MainWindow::~MainWindow()
{
    if (logFile)
    {
        logFile->close();
        delete logFile;
        logFile = nullptr;
    }
    delete ui;
}

///кнопка "соединение"
void MainWindow::OnConnectPressed(void)
{
    if (pConnectionDialog)
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

    pDriver->SetSettings(qsettings);

}

///
/// \brief Окно "соединение" закрыто с результатом "ок" - устанавливаем соединение
///
void MainWindow::OnConnectionDialogFinished(/*int result*/)
{
    pDriver->SetSettings(qsettings);
    pDriver->OpenConnection();

    qsettings->beginGroup("driver");
    bool createLog = qsettings->value("log","false").toBool();
    qsettings->endGroup();

     if (createLog)
     {
         QString logFileName = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd_HH-mm-ss")) + QStringLiteral(".txt");

         logFile = new QFile(logFileName);

         qDebug() << "New log: " << logFileName;
         logFile->open(QIODevice::WriteOnly | QIODevice::Text);
     }
}

///кнопка "разорвать соединение"
void MainWindow::OnDisconnectPressed(void)
{
    pDriver->CloseConnection();

    if (logFile)
        logFile->close();
}

///вызов окна настроек
void MainWindow::OnSettingsPressed(void)
{
    ConnectionSettingsDialog *cdialog = new ConnectionSettingsDialog(qsettings);
    cdialog->setAttribute(Qt::WA_DeleteOnClose);
    cdialog->show();
}

///в случае успешного подключения драйвера
void MainWindow::OnConnected()
{
    pConnectionStatusLabel->setText("Connected");
    pConnectionStatusLabel->setStyleSheet("color: green;");

}

///при разрыве соединения драйвера
void MainWindow::OnDisconnected()
{
    pConnectionStatusLabel->setText("Disconnected");
    pConnectionStatusLabel->setStyleSheet("color: red;");
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);

    ui->log->append(tr("Connection closed"));
}

///получено текстовое сообщение от драйвера
void MainWindow::LogReceived(QString text)
{
    if (logFile)
    {
      QTextStream logStream(logFile);
      logStream << text << '\n';
    }
    ui->log->append(text);

    // Cap on-screen log to avoid UI freezes on long sessions
    const int maxBlocks = 2000;
    QTextDocument *doc = ui->log->document();
    if (doc && doc->blockCount() > maxBlocks)
    {
        QTextCursor cursor(doc);
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor,
                            doc->blockCount() - maxBlocks);
        cursor.removeSelectedText();
    }
}


///очистка окна логов
void MainWindow::OnClearLogPressed()
{
    ui->log->clear();
}

///добавление в список сигналов через форму add
void MainWindow::MToolAdd_Accept()
{


        tabmodel->updateSignal(pAddSignalDialog->tag,true);

    delete pAddSignalDialog;
}

void MainWindow::MToolAdd_Pressed()
{
    pAddSignalDialog = new addSignalDialog();
    connect(pAddSignalDialog,SIGNAL(accepted()),this,SLOT(MToolAdd_Accept()));
    pAddSignalDialog->exec();
}


void MainWindow::MToolRemove_Pressed()
{
    // Qt6: QAbstractItemView::state() is protected — detect in-cell editor via focus
    QWidget *fw = QApplication::focusWidget();
    if (fw && fw != ui->MTable && ui->MTable->isAncestorOf(fw))
        return;

    QItemSelectionModel *pSelection =  ui->MTable->selectionModel();
    if (!pSelection)
        return;

    // Prefer whole-row selection; fall back to any selected cells / current index
    QModelIndexList proxyIndexes = pSelection->selectedRows();
    if (proxyIndexes.isEmpty())
        proxyIndexes = pSelection->selectedIndexes();
    if (proxyIndexes.isEmpty() && pSelection->currentIndex().isValid())
        proxyIndexes.append(pSelection->currentIndex());

    QList<int> sourceRows;
    foreach (QModelIndex proxyIndex, proxyIndexes)
    {
        QModelIndex sourceIndex = proxyModel->mapToSource(proxyIndex);
        if (sourceIndex.isValid() && !sourceRows.contains(sourceIndex.row()))
            sourceRows.append(sourceIndex.row());
    }
    if (sourceRows.isEmpty())
        return;

    std::sort(sourceRows.begin(), sourceRows.end());
    for (int i = sourceRows.count() - 1; i >= 0; --i)
        tabmodel->removeRow(sourceRows[i]);
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
    if (pDriver)
    pDriver->SendFullRequest(20);
}
void MainWindow::OnSaveBaseTriggered(bool)
{
    QFileDialog *fileDialog = new QFileDialog();

    QString filename = fileDialog->getSaveFileName(this,tr("Save file"),QString(),QString("*.csv")); //fileDialog->getOpenFileName(this,"Открытие файла",QString(),QString("*.xls *.xlsx"));

    if (filename.length()>0)
    {
        QFile *file = new QFile(filename);
        file->open(QIODevice::WriteOnly);
        QTextStream stream(file);
        stream << "type,IOA,description, value\n";
        foreach(CIECSignal signal, tabmodel->mData)
        {
            stream << signal.GetType() << ',' << signal.GetAddress() << ',' << signal.description << ',' << signal.value.toString() << '\n';

        }

        foreach(CIECSignal signal, cmdTableModel->mData)
        {
            stream << signal.GetType() << ',' << signal.GetAddress() << ',' << signal.description << ',' << signal.value.toString() << '\n';

        }
        file->close();
        delete file;

    }
    delete fileDialog;
}

void MainWindow::loadBase(QString filename)
{

    QString descr;
    if (filename.length()>0)
    {
        QFile *file= new QFile(filename);
        if (!file->open(QIODevice::ReadOnly))
        {
            delete file;
            return;
        }
        QTextStream in(file);

        //commandList = new QList<CIECSignal>();
        in.readLine();  //skip headers
        while(!in.atEnd())
        {
          QString line = in.readLine();
          if (line.trimmed().isEmpty())
              continue;
          qDebug() << line;
          QStringList words= line.split(',');
          if (words.count() < 3)
              continue;
          uint type = words[0].toUInt();
          uint ioa = words[1].toUInt();
          uint key = (type<<24) + ioa;


          CIECSignal sig;

          sig.SetKey( key);
          sig.description = words[2];



          // get value from file

          if (words.count()>3)
          {
              QString strval = words[3];
              if (strval=="true")
                  sig.value = true;
              else if (strval=="false")
                  sig.value = false;
              else
              {
                  bool ok;
                  sig.value = strval.toInt(&ok);

                  if (!ok)
                      sig.value = strval.toFloat(&ok);
              }

          }

          if (sig.GetType()>=45)
              cmdTableModel->updateSignal(sig,true,true);
          else
              tabmodel->updateSignal(sig,true,true);

        }

        file->close();
        delete file;

    }
}

void MainWindow::OnLoadFileTriggered(bool)
{
  QFileDialog *fileDialog = new QFileDialog();


  QString filename = fileDialog->getOpenFileName(this,tr("Open file"),QString(),QString("*.csv"));

  MainWindow::loadBase(filename);

  delete fileDialog;
}

void MainWindow::autoLoad(char *argv)
{
    QString filename;
    char c=argv[0];
    for(int i=0; argv[i]!='\0';i++)
    {
        c=argv[i];
        filename+=c;
    }
  MainWindow::loadBase(filename);
}

void MainWindow::OnLoadBaseTriggered(bool val)
{
    Q_UNUSED(val);
    // Excel import was removed; reuse CSV loader
    OnLoadFileTriggered(false);
}

void MainWindow::OnCMDPressed()
{
    CmdDialog *d = new CmdDialog(pDriver, qsettings, cmdTableModel,this);

    d->setAttribute(Qt::WA_DeleteOnClose);
    d->show();
}

void MainWindow::OnLogVisibleToggled(bool a)
{
    ui->dockWidget->setVisible(a);
}

void MainWindow::OnAboutTriggered(bool)
{
    aboutDialog *dialog = new aboutDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void MainWindow::OnShowWatchTriggered(bool)
{
    watch->show();
}

void MainWindow::AddCommand(CIECSignal item)
{
   // commandList->append(item);
    cmdTableModel->mData.append(item);
}
