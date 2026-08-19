#include "editcmddialog.h"
#include "ui_editcmddialog.h"
#include <QShortcut>
#include <QAction>
#include <QAbstractItemView>
#include <algorithm>

EditCMDdialog::EditCMDdialog(TableModel *cmdTable, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditCMDdialog)
{
    ui->setupUi(this);

    this->cmdTable = cmdTable;
    ui->tableView->setModel(cmdTable);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(ui->pushButton_add,SIGNAL(pressed()),this,SLOT(Append()));
    connect(ui->pushButton_remove,SIGNAL(pressed()),this,SLOT(Remove()));

    QAction *deleteAction = new QAction(tr("Delete"), ui->tableView);
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    ui->tableView->addAction(deleteAction);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(Remove()));

    QShortcut* shortcut = new QShortcut(QKeySequence(QKeySequence::Delete), ui->tableView);
    shortcut->setContext(Qt::WidgetWithChildrenShortcut);
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
    if (ui->tableView->state() == QAbstractItemView::EditingState)
        return;

    QItemSelectionModel *pSelection =  ui->tableView->selectionModel();
    if (!pSelection)
        return;

    QModelIndexList indexes = pSelection->selectedRows();
    if (indexes.isEmpty())
        indexes = pSelection->selectedIndexes();
    if (indexes.isEmpty() && pSelection->currentIndex().isValid())
        indexes.append(pSelection->currentIndex());

    QList<int> rows;
    foreach (QModelIndex index, indexes)
    {
        if (index.isValid() && !rows.contains(index.row()))
            rows.append(index.row());
    }
    std::sort(rows.begin(), rows.end());
    for (int i = rows.count() - 1; i >= 0; --i)
        cmdTable->removeRow(rows[i]);
}
