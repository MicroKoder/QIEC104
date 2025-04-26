/*
 * Copyright (C) 2025 Zayrullin Azat / zayruaz@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#ifndef EDITCMDDIALOG_H
#define EDITCMDDIALOG_H

#include <QDialog>
#include "tablemodel.h"
namespace Ui {
class EditCMDdialog;
}

class EditCMDdialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditCMDdialog(TableModel *cmdTable, QWidget *parent = 0);
    ~EditCMDdialog();

private:
    Ui::EditCMDdialog *ui;
    TableModel *cmdTable=nullptr;

public slots:
    void Append();
    void Remove();
};

#endif // EDITCMDDIALOG_H
