#ifndef CMDDIALOG_H
#define CMDDIALOG_H

#include <QDialog>
#include "iec104driver.h"
#include "tablemodel.h"
namespace Ui {
class CmdDialog;
}

class CmdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CmdDialog( IEC104Driver *pDriver,QSettings *pSettings, TableModel *cmdTable, QWidget *parent = 0);
    ~CmdDialog();

private:
    Ui::CmdDialog *ui;
    QSettings *pSett=nullptr;
    IEC104Driver *pDriver=nullptr;
   // QList<CIECSignal> *pCmdList=nullptr;
    void ShowWarning();
    QList<CIECSignal> filteredList;
    TableModel *cmdTable=nullptr;
    void FilterCommands(uint type);
public slots:
    void reject();
    void OnActivateCommand();
    void OnTypeChanged(int);
    void OnCommandSelected(int);
    void OnEditCommandList();

};

#endif // CMDDIALOG_H
