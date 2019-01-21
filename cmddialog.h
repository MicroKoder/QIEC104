#ifndef CMDDIALOG_H
#define CMDDIALOG_H

#include <QDialog>
#include "iec104driver.h"
namespace Ui {
class CmdDialog;
}

class CmdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CmdDialog( IEC104Driver *pDriver,QSettings *pSettings, QList<CIECSignal> *cmdList, QWidget *parent = 0);
    ~CmdDialog();

private:
    Ui::CmdDialog *ui;
    QSettings *pSett=nullptr;
    IEC104Driver *pDriver=nullptr;
    QList<CIECSignal> *pCmdList=nullptr;
    void ShowWarning();
    QList<CIECSignal> filteredList;

    void FilterCommands(int type);
public slots:
    void reject();
    void OnActivateCommand();
    void OnTypeChanged(int);
    void OnCommandSelected(int);

};

#endif // CMDDIALOG_H
