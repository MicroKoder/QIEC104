#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <csetting.h>
#include <qstring>
namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    SettingsDialog(CSetting *settings);
    ~SettingsDialog();
public slots:
    void Accepted();
private:
    Ui::SettingsDialog *ui;
    CSetting *settings;
};

#endif // SETTINGSDIALOG_H
