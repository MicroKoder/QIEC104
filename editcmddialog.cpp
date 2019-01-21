#include "editcmddialog.h"
#include "ui_editcmddialog.h"

EditCMDdialog::EditCMDdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditCMDdialog)
{
    ui->setupUi(this);
}

EditCMDdialog::~EditCMDdialog()
{
    delete ui;
}
