#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#define FILL_TEST_TABLE

#include <QMainWindow>
//#include <csetting.h>
#include <iec104driver.h>
#include "tablemodel.h"
#include <QSettings>
#include <QLabel>
#include <addsignaldialog.h>
#include <connectiondialog.h>
#include "importdialog.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    //CSetting *settings;
    QSettings *qsettings=0;
    IEC104Driver *pDriver=0;
    TableModel *tabmodel=0;

    ~MainWindow();

    void AddMSignal(CIECSignal* tag, QString description=QString());
private:
    Ui::MainWindow *ui;
     QLabel *pConnectionStatusLabel=0;
     addSignalDialog *pAddSignalDialog=0;
     ConnectionSettingsDialog *pConnectionDialog=0;

public slots:
    void OnConnectPressed(void);
    void OnConnectAck(void);//
    void OnConnectionDialogFinished();

    void OnDisconnectPressed(void);
    void OnSettingsPressed(void);
    void OnConnected();
    void OnDisconnected();
    void LogReceived(QString);
    void OnClearLogPressed();

    void MToolAdd_Pressed();
    void MToolAdd_Accept();
    void MToolRemove_Pressed();
    void IECReceived(CIECSignal tag);

    void OnLoadBaseTriggered(bool); //меню - импорт базы
    void OnGIPressed(void); //general interrogation
    void OnCMDPressed(void); //открыть диалог с командами
};

#endif // MAINWINDOW_H
