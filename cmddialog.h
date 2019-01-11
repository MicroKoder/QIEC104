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
    explicit CmdDialog( IEC104Driver *pDriver,QSettings *pSettings, QWidget *parent = 0);
    ~CmdDialog();

private:
    Ui::CmdDialog *ui;
    QSettings *pSett=0;
    IEC104Driver *pDriver=0;
    void ShowWarning();
public slots:
    void reject();
    void OnActivateCommand();
    void OnTypeChanged(int);
};

#endif // CMDDIALOG_H
