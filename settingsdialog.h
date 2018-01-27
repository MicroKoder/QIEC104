#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <csetting.h>
#include <qstring>
namespace Ui {
class SettingsDialog;
}

class ConnectionSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionSettingsDialog(QWidget *parent = 0);
    ConnectionSettingsDialog(CSetting *settings);
    ~ConnectionSettingsDialog();
public slots:
    void Accepted();
private:
    Ui::SettingsDialog *ui;
    CSetting *settings;
};

#endif // SETTINGSDIALOG_H
