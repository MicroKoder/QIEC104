#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <csetting.h>
#include <iec104driver.h>
#include "tablemodel.h"
#include <QLabel>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    CSetting *settings;
    IEC104Driver *driver;
    TableModel *tabmodel;

    ~MainWindow();

private:
    Ui::MainWindow *ui;
     QLabel *connectionStatusLabel;
public slots:
    void OnConnectPressed(void);
    void OnDisconnectPressed(void);
    void OnSettingsPressed(void);
    void OnConnected();
    void OnDisconnected();
    void LogReceived(QString);
    void OnClearLogPressed();

    void IECReceived(CIECSignal* tag);
};

#endif // MAINWINDOW_H
