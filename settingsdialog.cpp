#include "settingsdialog.h"
#include "ui_settingsdialog.h"

ConnectionSettingsDialog::ConnectionSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

ConnectionSettingsDialog::ConnectionSettingsDialog(CSetting *settings):
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
    ui->edit_k->setText(QString::number(settings->k));
    ui->edit_w->setText(QString::number(settings->w));
    ui->edit_asdu->setText(QString::number(settings->asdu));

   connect(ui->buttonBox,SIGNAL(accepted()), this, SLOT(Accepted()));
 //connect(ui->buttonBox,SIGNAL(rejected()), this, SLOT(reject());


}

ConnectionSettingsDialog::~ConnectionSettingsDialog()
{
    delete ui;
}

void ConnectionSettingsDialog::Accepted()
{
     settings->IP = ui->comboBox_ip->currentText();
     settings->Port = ui->comboBox_port->currentText().toUInt();
     settings->t0 = ui->edit_t0->text().toUInt();
     settings->t1 = ui->edit_t1->text().toUInt();
     settings->t2 = ui->edit_t2->text().toUInt();
     settings->t3 = ui->edit_t3->text().toUInt();
     settings->k = ui->edit_k->text().toUInt();
     settings->w = ui->edit_w->text().toUInt();
     settings->asdu = ui->edit_asdu->text().toUInt();
     this->close();
}
