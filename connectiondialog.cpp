#include "connectiondialog.h"
#include "ui_settingsdialog.h"
#include <QDebug>
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

    ui->AutoCreate_checkBox->setChecked(settings->autoCreateTags);

   connect(ui->buttonBox,SIGNAL(accepted()), this, SLOT(Accepted()));
 //connect(ui->buttonBox,SIGNAL(rejected()), this, SLOT(reject());


}

ConnectionSettingsDialog::ConnectionSettingsDialog(QSettings *_qset):
    QDialog(0),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    if (_qset != NULL)
    {
        qset = _qset;
        qset->beginGroup("driver");
        ui->comboBox_ip->setCurrentText(qset->value("ip","127.0.0.1").toString());
        ui->comboBox_port->setCurrentText(qset->value("port", "2404").toString());
        ui->edit_t0->setText(qset->value("t0", "10").toString());
        ui->edit_t1->setText(qset->value("t1", "15").toString());
        ui->edit_t2->setText(qset->value("t2", "10").toString());
        ui->edit_t3->setText(qset->value("t3", "5").toString());
        ui->edit_k->setText(qset->value("k", "12").toString());
        ui->edit_w->setText(qset->value("w", "8").toString());
        ui->edit_asdu->setText(qset->value("asdu", "1").toString());
        ui->AutoCreate_checkBox->setChecked(qset->value("autoCreate","false").toBool());
        qset->endGroup();
    }
    connect(ui->buttonBox,SIGNAL(accepted()), this, SLOT(AcceptSettings()));
}

ConnectionSettingsDialog::~ConnectionSettingsDialog()
{
    delete ui;
}

void ConnectionSettingsDialog::AcceptSettings()
{
    if (settings != NULL)
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
     settings->autoCreateTags = ui->AutoCreate_checkBox->isChecked();
    }

    if (qset != NULL)
    {
     qset->beginGroup("driver");
     qset->setValue("ip",   ui->comboBox_ip->currentText());
     qset->setValue("port", ui->comboBox_port->currentText());
     qset->setValue("t0",   ui->edit_t0->text());
     qset->setValue("t1",   ui->edit_t1->text());
     qset->setValue("t2",   ui->edit_t2->text());
     qset->setValue("t3",   ui->edit_t3->text());
     qset->setValue("k",    ui->edit_k->text());
     qset->setValue("w",    ui->edit_w->text());
     qset->setValue("asdu", ui->edit_asdu->text());
     qset->setValue("autoCreate", ui->AutoCreate_checkBox->isChecked());
     qset->endGroup();
    }
    qDebug() << "Connection Settings accepted";
    emit SettingsAccepted();
     this->close();
}
