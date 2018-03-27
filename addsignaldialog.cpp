#include "addsignaldialog.h"
#include "ui_addsignaldialog.h"
#include <Qdebug>
#include <QIntValidator>

addSignalDialog::addSignalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addSignalDialog)
{
    ui->setupUi(this);
    ui->edit_IOA->setValidator(new QIntValidator(0,16777216));
}

addSignalDialog::~addSignalDialog()
{
    delete ui;
}

void addSignalDialog::accept()
{
    int type;
    switch (ui->comboBox->currentIndex())
    {
        case 0: type = 1; break;
    case 1: type = 3; break;
    case 2: type = 5; break;
    case 3: type = 7; break;
    case 4: type = 9; break;
    case 5: type = 11; break;
    case 6: type = 13; break;
    case 7: type = 15; break;
    case 8: type = 20; break;
    case 9: type = 21; break;
    case 10: type = 30; break;
    case 11: type = 31; break;
    case 12: type = 32; break;
    case 13: type = 33; break;
    case 14: type = 34; break;
    case 15: type = 35; break;
    case 16: type = 36; break;
    case 17: type = 37; break;
    case 18: type = 38; break;
    case 19: type = 40; break;

    }
    int IOA ;
    if (ui->edit_IOA->text().length()>0)
    {
        IOA = ui->edit_IOA->text().toInt();
        this->tag = new CIECSignal(IOA,type);
         qDebug() << "selected type "<< type << "; IOA= " << IOA;
         this->accepted();
    }
    else
    {
        qDebug() << "IOA not defined";

    }

}
