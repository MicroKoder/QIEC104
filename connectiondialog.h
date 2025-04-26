/*
 * Copyright (C) 2025 Zayrullin Azat / zayruaz@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <csetting.h>
#include <QString>
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
signals:
   void SettingsAccepted();

public slots:
    void AcceptSettings();
private:
    Ui::SettingsDialog *ui;
    CSetting *settings=nullptr;
    QSettings *qset=nullptr;
};

#endif // SETTINGSDIALOG_H
