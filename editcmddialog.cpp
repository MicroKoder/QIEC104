/*
 * Copyright (C) 2025 Zayrullin Azat / zayruaz@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#include "editcmddialog.h"
#include "ui_editcmddialog.h"
#include <QShortcut>
EditCMDdialog::EditCMDdialog(TableModel *cmdTable, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditCMDdialog)
{
    ui->setupUi(this);

    this->cmdTable = cmdTable;
    ui->tableView->setModel(cmdTable);

    connect(ui->pushButton_add,SIGNAL(pressed()),this,SLOT(Append()));
    connect(ui->pushButton_remove,SIGNAL(pressed()),this,SLOT(Remove()));

    QShortcut* shortcut = new QShortcut(QKeySequence(QKeySequence::Delete), ui->tableView);
    connect(shortcut, SIGNAL(activated()), this, SLOT(Remove()));

    ui->comboBox_type->addItem(tr("45 Single point command"));
    ui->comboBox_type->addItem(tr("46 Double point command"));
    ui->comboBox_type->addItem(tr("47 Regulating step command"));
    ui->comboBox_type->addItem(tr("48 Setpoint command, normalized value"));
    ui->comboBox_type->addItem(tr("49 Setpoint command, scaled value"));
    ui->comboBox_type->addItem(tr("50 Setpoint command, short floating point value"));
    ui->comboBox_type->addItem(tr("51 Bit string 32 bit"));
}

EditCMDdialog::~EditCMDdialog()
{
    delete ui;
}

void EditCMDdialog::Append()
{
    uint16_t ioa = ui->lineEdit_IOA->text().toUInt();
    uint type = ui->comboBox_type->currentIndex()+45;
    QString descr = ui->lineEdit_desc->text();

    cmdTable->updateSignal(CIECSignal(ioa,type,descr),true,true);
}

void EditCMDdialog::Remove()
{
    QItemSelectionModel *pSelection =  ui->tableView->selectionModel();

    cmdTable->removeRows(pSelection);
}
