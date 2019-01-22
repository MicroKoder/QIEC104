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
}

void CmdDialog::OnTypeChanged(int index)
{
    Q_UNUSED(index);
    switch(ui->comboBox_type->currentIndex())
    {
    case 0:
            ui->groupBox_setcommand->show();
            ui->groupBox_setpoint->hide();
            ui->comboBox_cmdValue->clear();

            ui->comboBox_cmdValue->addItem("0:Выкл");
            ui->comboBox_cmdValue->addItem("1:Вкл");

            FilterCommands(45);
        break;
    case 1:
        ui->groupBox_setcommand->show();
        ui->groupBox_setpoint->hide();
        ui->comboBox_cmdValue->clear();

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

        ui->comboBox_cmdValue->addItem("0:Не разрешено");
        ui->comboBox_cmdValue->addItem("1:Шаг вниз");
        ui->comboBox_cmdValue->addItem("2:Шаг вверх");
        ui->comboBox_cmdValue->addItem("3:Не разрешено");

        FilterCommands(47);

        break;
    case 3:
        ui->groupBox_setcommand->hide();
        ui->groupBox_setpoint->show();
        FilterCommands(48);
        break;
    case 4:
        ui->groupBox_setcommand->hide();
        ui->groupBox_setpoint->show();
        FilterCommands(49);
        break;
    case 5:
        ui->groupBox_setcommand->hide();
        ui->groupBox_setpoint->show();
        FilterCommands(50);
        break;
    case 6:
        ui->groupBox_setcommand->hide();
        ui->groupBox_setpoint->show();
        FilterCommands(51);
        break;
    }

}
void CmdDialog::FilterCommands(int type)
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

void CmdDialog::OnActivateCommand()
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



        switch(ui->comboBox_type->currentIndex())
        {
           case 0:
            CO=ui->comboBox_cmdValue->currentIndex() + (ui->comboBox_SCType->currentIndex()<<2);

            pDriver->SendCommand(45 + correctType ,ui->spinBox_ioa->value(),CO);
            break;
           case 1:
            CO=ui->comboBox_cmdValue->currentIndex() + (ui->comboBox_SCType->currentIndex()<<2);

            pDriver->SendCommand(46 + correctType ,ui->spinBox_ioa->value(),CO);
            break;

          case 2:
            CO=ui->comboBox_cmdValue->currentIndex() + (ui->comboBox_SCType->currentIndex()<<2);

           pDriver->SendCommand(47 + correctType ,ui->spinBox_ioa->value(),CO);
         break;
           case 3:
            uvalue = ui->lineEdit_value->text().toUInt(&ok);
            if (ok)
                pDriver->SetPoint(48+ correctType, ui->spinBox_ioa->value(),QVariant(uvalue));
            else
                ShowWarning();

            break;

           case 4:
            ivalue = ui->lineEdit_value->text().toInt(&ok);
            if (ok)
                pDriver->SetPoint(49+correctType, ui->spinBox_ioa->value(),QVariant(ivalue));
            else
                ShowWarning();
            break;
           case 5:
            fValue = ui->lineEdit_value->text().toFloat(&ok);
            if (ok)
                pDriver->SetPoint(50+correctType, ui->spinBox_ioa->value(),QVariant(fValue));
            else
                ShowWarning();

            break;
           case 6:
            dvalue = ui->lineEdit_value->text().toUInt(&ok);
            if (ok)
                pDriver->SetPoint(51+correctType, ui->spinBox_ioa->value(),QVariant(dvalue));
            else
                ShowWarning();
            break;
        }
  }

}
void CmdDialog::ShowWarning()
{
    QMessageBox::warning(this,"Ошибка","Недопустимое значение уставки");
}
