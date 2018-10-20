#include "importdialog.h"
#include "ui_importdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include "ciecsignal.h"
#include <QAxObject>
#include <QDebug>
#include <QList>
ImportDialog::ImportDialog() :
    QDialog(0),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);

    connect(ui->pbOpenDialog, SIGNAL(pressed()),this,SLOT(On_OpenDialogPressed()));
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(On_OkPressed()));
    MeasuresTable = NULL;
}
void ImportDialog::SetModel(TableModel* table)
{
     this->MeasuresTable = dynamic_cast<TableModel*>(table);
}
ImportDialog::~ImportDialog()
{
    delete ui;
}

///
/// \brief Подтверждение команды импорта
///
void ImportDialog::On_OkPressed()
{
    if (MeasuresTable != NULL && importedItems->count()>0)
     {
        qDebug() << "adding to table...";
         ImportItem item;
         foreach(item, *importedItems)
             if (item.kod<45) //измерения
                MeasuresTable->updateSignal(new CIECSignal(item.ioa,item.kod, item.descr));
            //else //добавить код для таблицы команд
     }
   // MeasuresTable->redraw();
    qDebug() << "import complete";
    delete importedItems;
}
void ImportDialog::On_OpenDialogPressed()
{
    /*if (parent==NULL)
    {
        QMessageBox msgbox;
        msgbox.setText("parent не задан!");
        msgbox.show();
        return;
    }*/
    importedItems = new QList<ImportItem>();

    if (ui->selector_xls->isChecked())
    {
        QFileDialog *fileDialog = new QFileDialog();

        QString filename = fileDialog->getOpenFileName(this,"Открытие файла",QString(),QString("*.xls *.xlsx"));
        int iSkipRows = ui->skipSpinBox->value();
       // parent->IECReceived();

        delete fileDialog;

        if (filename =="") return;

        QAxObject* excel = new QAxObject("Excel.Application", 0);
        QAxObject* workbooks = excel->querySubObject("Workbooks");
        QAxObject* workbook = workbooks->querySubObject("Open(const QString&)", filename);

        qDebug() << "opening file: " << filename;
        QAxObject* sheets = workbook->querySubObject("Worksheets");
        QAxObject* sheet = sheets->querySubObject("Item(int)", 1);

        QAxObject* rows = sheet->querySubObject( "Rows" );
        int rowCount = rows->dynamicCall( "Count()" ).toInt(); //unfortunately, always returns 255, so you have to check somehow validity of cell values
        QAxObject* columns = sheet->querySubObject("Columns");
        int columnCount = columns->dynamicCall("Count()").toInt();
      qDebug() << rowCount << " rows in sheet";
        if (columnCount>10)
            columnCount=10;

        int iEmptyRowsCount = 0;
        int iMaxEmptyRows = 10; //максимальное количество пустых строк после которых прекращаем чтение
        int row = iSkipRows;   //номер строки в файле источнике
        int parsedRows=0;   //номер строки в итоговой таблице
        //for (int row = 0; row<10; row++)
        while (row<rowCount && iEmptyRowsCount<iMaxEmptyRows)
        {
            int ioa=0;
            int kode=0;
            QString descr;
            for (int col=0;col<3; col++)
            {
                QAxObject* cell;
                QVariant value;
                switch (col)
                {
                    case 0: cell = sheet->querySubObject("Cells(int,int)", row+1, ui->ioaSpinBox->value());
                            value = cell->property("Value");
                            if (value.canConvert<int>())
                                ioa = value.toInt();
                            break;
                    case 1: cell = sheet->querySubObject("Cells(int,int)", row+1, ui->codSpinBox->value());
                            value = cell->property("Value");
                            if (value.canConvert<int>())
                                kode = value.toInt();
                            break;
                    case 2: cell = sheet->querySubObject("Cells(int,int)", row+1, ui->descrSpinBox->value());
                            descr = QString (cell->property("Value").toString());
                            break;
               }

            }//col
            row++;
            //если ioa и код прочитались то сбрасываем счетчик ошибок и добавляем в таблицу
            if (ioa>0 && kode>0)
            {
                iEmptyRowsCount = 0;
                qDebug() << ioa<< " " << kode << " " << descr;
                QTableWidgetItem* item = new QTableWidgetItem(QString::number(ioa));


                    ui->tableWidget->setItem(parsedRows,0,item);

                    item = new QTableWidgetItem(QString::number(kode));
                    ui->tableWidget->setItem(parsedRows,1,item);

                    item = new QTableWidgetItem(descr);
                    ui->tableWidget->setItem(parsedRows,2,item);

                importedItems->append(ImportItem(ioa,kode,descr));

                parsedRows++;
            }else
            {
                iEmptyRowsCount++;
            }
        }//while
        qDebug() << "rows parsed " << parsedRows;
        QMessageBox msg;
        msg.setText("Прочитано сигналов: " + QString::number(parsedRows));
        msg.show();


    }//if xls


}


