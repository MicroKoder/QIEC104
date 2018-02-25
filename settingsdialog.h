#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <csetting.h>
#include <qstring>
#include <qsettings.h>
namespace Ui {
class SettingsDialog;
}

class ConnectionSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionSettingsDialog(QWidget *parent = 0);
    ConnectionSettingsDialog(CSetting *settings);
    ConnectionSettingsDialog(QSettings *settings);
    ~ConnectionSettingsDialog();
public slots:
    void Accepted();
private:
    Ui::SettingsDialog *ui;
    CSetting *settings=NULL;
    QSettings *qset=NULL;
};

#endif // SETTINGSDIALOG_H
