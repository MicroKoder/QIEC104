#include "watchdialog.h"
#include "ui_watchdialog.h"
#include "QDebug"
#include <QMenu>
WatchDialog::WatchDialog(IEC104Driver* pDriver, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WatchDialog)
{
    ui->setupUi(this);
    this->pDriver = pDriver;

    connect(pDriver,SIGNAL(IECSignalReceived(CIECSignal)),this,SLOT(OnSignalReceived(CIECSignal)));

   // ui->tableWidget->setCellWidget(1,1,new QPushButton());
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("IOA"));
    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem(tr("Description")));
    ui->tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem(tr("Type")));
    ui->tableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem(tr("Value")));
    ui->tableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem(tr("Quality")));
    ui->tableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem(tr("Time tag")));

    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(ContextMenuRequested(QPoint)));
}
void WatchDialog::ContextMenuRequested(QPoint pos)
{
    QMenu *contextMenu= new QMenu(this);
    QAction *remove=new QAction(tr("Delete"),this);
    connect(remove,SIGNAL(triggered(bool)), this, SLOT(OnRowRemove()));
    contextMenu->addAction(remove);
    contextMenu->popup(mapToParent(pos)+QPoint(10,10));
}
void WatchDialog::OnRowRemove()
{
    auto selection = ui->tableWidget->selectedItems();
    std::sort(selection.begin(),selection.end(),
              [] (const QTableWidgetItem *it1,const QTableWidgetItem *it2) -> bool { return it1->row()<it2->row();} );

    for (auto it=selection.end()-1; it>=selection.begin();it--)
        watchItems.removeAt((*it)->row());

    RefreshTable();
}

WatchDialog::~WatchDialog()
{
    delete ui;
}

void WatchDialog::OnSignalReceived(CIECSignal signal)
{
    for (int i=0; i<watchItems.count(); i++)
    {
        if (watchItems[i].GetKey()==signal.GetKey())
        {
            watchItems[i].value = signal.value;
            watchItems[i].quality = signal.quality;
            watchItems[i].timestamp = signal.timestamp;
            RefreshTable();

            return;
        }

    }
}

void WatchDialog::RefreshTable()
{
    ui->tableWidget->setRowCount(watchItems.count());
    if (watchItems.count()==0) return;

    for (int i=0; i<watchItems.count(); i++)
    {
        CIECSignal item = watchItems.at(i);
        ui->tableWidget->setItem(i,0, new QTableWidgetItem(QString::number(item.GetAddress())));
        ui->tableWidget->setItem(i,1, new QTableWidgetItem(item.description));
        ui->tableWidget->setItem(i,2, new QTableWidgetItem(QString::number(item.GetType())));
        ui->tableWidget->setItem(i,3, new QTableWidgetItem(item.value.toString()));
        //ui->tableWidget->setItem(i,4, new QTableWidgetItem(item.quality));

        QString quality_str;
        if (item.bNeverUpdated)
        {
            quality_str = "";
        }
        else
        {
            if (item.quality&1)
                quality_str+="OV ";

            if (item.quality&8)
                quality_str+="EI ";

            if (item.quality&16)
                quality_str+="BL ";

            if (item.quality&32)
                quality_str+="SB ";

            if (item.quality&64)
                quality_str+="NT ";

            if (item.quality&128)
                quality_str+="IV ";

            if (item.quality==0)
                quality_str = "GOOD";
        }
        ui->tableWidget->setItem(i,4, new QTableWidgetItem(quality_str));
        ui->tableWidget->setItem(i,5, new QTableWidgetItem(item.timestamp.GetTimeString()));
    }
}
void WatchDialog::AddWatch(CIECSignal signal)
{
    //проверяем есть ли уже такой в списке
    foreach(CIECSignal item, watchItems)
    {
        if (item.GetKey()==signal.GetKey())
            return;
    }
    //else

     watchItems.append(signal);


    RefreshTable();
    qDebug() << "watch items: " << watchItems.count();


}
