/*
 * Copyright (C) 2025 Zayrullin Azat / zayruaz@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#include "cmddialog.h"
#include "ui_cmddialog.h"
#include <QDebug>
#include <QMessageBox>
#include "editcmddialog.h"

CmdDialog::CmdDialog(IEC104Driver *pDriver,QSettings *pSettings,TableModel *cmdTable, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CmdDialog)
{
    ui->setupUi(this);
    this->pDriver = pDriver;
    if (pSettings !=0)
    {
        pSett= pSettings;
        pSettings->beginGroup("command");
        ui->comboBox_type->setCurrentIndex(pSettings->value("type",0).toInt());
        ui->checkBox_timestamp->setChecked(pSettings->value("UseTimestamp",false).toBool());
        ui->spinBox_ioa->setValue(pSettings->value("IOA",0).toInt());
        ui->lineEdit_value->setText(pSettings->value("value",QString("")).toString());
        pSettings->endGroup();
    }
    connect(ui->pushButton,SIGNAL(pressed()),this,SLOT(OnActivateCommand()));
    connect(ui->pushButton_deactivate,SIGNAL(pressed()),this,SLOT(OnDeactivateCommand()));
    connect(ui->comboBox_type, SIGNAL(currentIndexChanged(int)),this,SLOT(OnTypeChanged(int)));
    connect(ui->comboBox_selectCMD, SIGNAL(currentIndexChanged(int)),this, SLOT(OnCommandSelected(int)));

    this->cmdTable = cmdTable;

    OnTypeChanged(ui->comboBox_type->currentIndex());
    OnCommandSelected(ui->comboBox_selectCMD->currentIndex());

    connect(ui->pushButton_listCMD,SIGNAL(pressed()),this,SLOT(OnEditCommandList()));

}

CmdDialog::~CmdDialog()
{
    delete ui;
}

void CmdDialog::OnEditCommandList()
{
    EditCMDdialog dialog(cmdTable,this);
    dialog.exec();
    OnTypeChanged(ui->comboBox_type->currentIndex());
}

void CmdDialog::OnTypeChanged(int index)
{
    Q_UNUSED(index);
    switch(ui->comboBox_type->currentIndex())
    {
    case 0:
            ui->groupBox_setpoint->hide();
            ui->comboBox_cmdValue->clear();
            ui->pushButton_deactivate->show();

            ui->comboBox_cmdValue->hide();

            //ui->comboBox_cmdValue->addItem("0:Выкл");
            //ui->comboBox_cmdValue->addItem("1:Вкл");

            FilterCommands(45);
        break;
    case 1:
        ui->comboBox_cmdValue->show();
        ui->groupBox_setcommand->show();
        ui->groupBox_setpoint->hide();
        ui->comboBox_cmdValue->clear();

        ui->pushButton_deactivate->hide();


        ui->comboBox_cmdValue->addItem("0:Не разрешено");
        ui->comboBox_cmdValue->addItem("1:Выкл");
        ui->comboBox_cmdValue->addItem("2:Вкл");
        ui->comboBox_cmdValue->addItem("3:Не разрешено");

        FilterCommands(46);

        break;
    case 2:
        ui->groupBox_setcommand->show();
        ui->groupBox_setpoint->hide();
        ui->comboBox_cmdValue->clear();
        ui->pushButton_deactivate->hide();
        ui->comboBox_cmdValue->show();

        ui->comboBox_cmdValue->addItem("0:Не разрешено");
        ui->comboBox_cmdValue->addItem("1:Шаг вниз");
        ui->comboBox_cmdValue->addItem("2:Шаг вверх");
        ui->comboBox_cmdValue->addItem("3:Не разрешено");

        FilterCommands(47);

        break;
    case 3:
        ui->groupBox_setcommand->hide();
        ui->groupBox_setpoint->show();
        ui->pushButton_deactivate->hide();
        FilterCommands(48);
        break;
    case 4:
        ui->groupBox_setcommand->hide();
        ui->groupBox_setpoint->show();
        ui->pushButton_deactivate->hide();
        FilterCommands(49);
        break;
    case 5:
        ui->groupBox_setcommand->hide();
        ui->groupBox_setpoint->show();
        ui->pushButton_deactivate->hide();
        FilterCommands(50);
        break;
    case 6:
        ui->groupBox_setcommand->hide();
        ui->groupBox_setpoint->show();
        ui->pushButton_deactivate->hide();
        FilterCommands(51);
        break;
    }

}
void CmdDialog::FilterCommands(uint type)
{
    filteredList.clear();
    ui->comboBox_selectCMD->clear();
    if (cmdTable)
        foreach(CIECSignal item, cmdTable->mData)
        {
            if (item.GetType()==type)
            {
            filteredList.append(item);
            ui->comboBox_selectCMD->addItem(QString::number(item.GetAddress())+"  " +item.description);
            }
        }

}
void CmdDialog::OnCommandSelected(int index)
{
    if (filteredList.count()==0) return;

    ui->spinBox_ioa->setValue((int)filteredList[index].GetAddress());

}

void CmdDialog::reject()
{
    if (pSett!=0)
    {
        pSett->beginGroup("command");

        pSett->setValue("type",QVariant(ui->comboBox_type->currentIndex()));
        pSett->setValue("UseTimestamp", QVariant(ui->checkBox_timestamp->isChecked()));
        pSett->setValue("IOA",QVariant(ui->spinBox_ioa->value()));
        pSett->setValue("value",QVariant(ui->lineEdit_value->text()));

        pSett->endGroup();
    }

    QDialog::reject();
}
enum{
    CMD_SINGLE,
    CMD_DOUBLE,
    CMD_STEPREG,
    CMD_SETPOINT_NORMAL,
    CMD_SETPOINT_SCALED,
    CMD_SETPOINT_FLOAT,
    CMD_SETPOINT_DWORD
}E_CMD_TYPES;
void CmdDialog::SendCommand( bool isActivate)
{
    bool ok=0;
    quint8 CO; //Command Object
    quint16 uvalue;
    int ivalue;
    float fValue;
    quint32 dvalue;
  if (pDriver!=0)
  {

        quint16 correctType = ui->checkBox_timestamp->isChecked() ? 13:0;


        CO = ui->comboBox_SCType->currentIndex()<<2;
        CO += ui->comboBox_SE->currentIndex() <<7;
        switch(ui->comboBox_type->currentIndex())
        {
           case CMD_SINGLE:
            CO +=(isActivate? 1:0);

            pDriver->SendCommand(45 + correctType ,ui->spinBox_ioa->value(),CO);
            break;
           case CMD_DOUBLE:
            CO +=(ui->comboBox_cmdValue->currentIndex());

            pDriver->SendCommand(46 + correctType ,ui->spinBox_ioa->value(),CO);
            break;

          case CMD_STEPREG:
            CO +=(ui->comboBox_cmdValue->currentIndex());

           pDriver->SendCommand(47 + correctType ,ui->spinBox_ioa->value(),CO);
         break;
           case CMD_SETPOINT_NORMAL:
            fValue = ui->lineEdit_value->text().toFloat(&ok);
            uvalue = 0;
            if (fValue>0)
            {
                uvalue = quint16(fValue * 32767);
            }
            if (fValue<0)
            {
                uvalue = quint16(fValue * 32768 + 65536);
            }
            if (fValue>1.0f || fValue<-1.0f)
            {
                ok=false;
            }

            if (ok)
                pDriver->SetPoint(48+ correctType, ui->spinBox_ioa->value(),QVariant(uvalue));
            else
                ShowWarning();

            break;

           case CMD_SETPOINT_SCALED:
            ivalue = ui->lineEdit_value->text().toInt(&ok);
            if (ok)
                pDriver->SetPoint(49+correctType, ui->spinBox_ioa->value(),QVariant(ivalue));
            else
                ShowWarning();
            break;
           case CMD_SETPOINT_FLOAT:
            fValue = ui->lineEdit_value->text().toFloat(&ok);
            if (ok)
                pDriver->SetPoint(50+correctType, ui->spinBox_ioa->value(),QVariant(fValue));
            else
                ShowWarning();

            break;
           case CMD_SETPOINT_DWORD:
            dvalue = ui->lineEdit_value->text().toUInt(&ok);
            if (ok)
                pDriver->SetPoint(51+correctType, ui->spinBox_ioa->value(),QVariant(dvalue));
            else
                ShowWarning();
            break;
        }
  }
}

void CmdDialog::OnDeactivateCommand()
{
    SendCommand(false);
}


void CmdDialog::OnActivateCommand()
{
    SendCommand(true);
}

void CmdDialog::ShowWarning()
{
    QMessageBox::warning(this,tr("Error"),tr("Wrong value"));
}
