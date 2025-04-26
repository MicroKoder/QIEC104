/*
 * Copyright (C) 2025 Zayrullin Azat / zayruaz@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#include "aboutdialog.h"
#include "ui_aboutdialog.h"
aboutDialog::aboutDialog():
QDialog(0),
ui(new Ui::aboutDialog)
{
    ui->setupUi(this);
}
