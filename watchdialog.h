#ifndef WATCHDIALOG_H
#define WATCHDIALOG_H

#include <QDialog>
#include "iec104driver.h"
namespace Ui {
class WatchDialog;
}

class WatchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WatchDialog(IEC104Driver* pDriver, QWidget *parent = 0);
    ~WatchDialog();
public slots:
    void OnSignalReceived(CIECSignal signal);
    void AddWatch(CIECSignal signal);
    void ContextMenuRequested(QPoint);
    void OnRowRemove();
private:
    Ui::WatchDialog *ui;
    IEC104Driver *pDriver=0;
    QList<CIECSignal> watchItems;
    void RefreshTable();


};

#endif // WATCHDIALOG_H
