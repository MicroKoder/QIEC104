#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::SettingsDialog(CSetting *settings):
    QDialog(0),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->settings = settings;
    ui->comboBox_ip->setCurrentText(settings->IP);
    ui->comboBox_port->setCurrentText(QString::number(settings->Port));
    ui->edit_t0->setText(QString::number(settings->t0));
    ui->edit_t1->setText(QString::number(settings->t1));
    ui->edit_t2->setText(QString::number(settings->t2));
    ui->edit_t3->setText(QString::number(settings->t3));

   connect(ui->buttonBox,SIGNAL(accepted()), this, SLOT(Accepted()));
 //connect(ui->buttonBox,SIGNAL(rejected()), this, SLOT(reject());


}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::Accepted()
{
     settings->IP = ui->comboBox_ip->currentText();
     settings->Port = ui->comboBox_port->currentText().toUInt();
     settings->t0 = ui->edit_t0->text().toUInt();
     settings->t1 = ui->edit_t1->text().toUInt();
     settings->t2 = ui->edit_t2->text().toUInt();
     settings->t3 = ui->edit_t3->text().toUInt();
     this->close();
}
