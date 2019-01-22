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

    ui->comboBox_type->addItem("45 Одноэлементная команда");
    ui->comboBox_type->addItem("46 Двухэлементная команда");
    ui->comboBox_type->addItem("47 Команда пошагового регулирования");
    ui->comboBox_type->addItem("48 Команда уставки нормализованное значение");
    ui->comboBox_type->addItem("49 Команда уставки масштабированное значение");
    ui->comboBox_type->addItem("50 Команда уставки значение с плавающей точкой");
    ui->comboBox_type->addItem("51 Команда уставки строка 32 бит");
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
