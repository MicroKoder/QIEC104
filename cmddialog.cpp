#include "cmddialog.h"
#include "ui_cmddialog.h"
#include <QDebug>
CmdDialog::CmdDialog(IEC104Driver *pDriver,QSettings *pSettings, QWidget *parent) :
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


}

CmdDialog::~CmdDialog()
{
    delete ui;
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
  if (pDriver!=0)
  {

        quint16 correctType = ui->checkBox_timestamp->isChecked() ? 13:0;

        switch(ui->comboBox_type->currentIndex())
        {
           case 0:

            pDriver->SendCommand(45 + correctType,ui->spinBox_ioa->value(),1);
            break;

        }
  }
}
