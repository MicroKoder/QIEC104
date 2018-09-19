#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
#include "tablemodel.h"


///
/// \brief Элемент прочитанный из базы данных
///
class ImportItem
{
public:
    ///
    /// \brief ioa адрес сигнала
    ///
    int ioa = 0;
    ///
    /// \brief тип сигнала (код по стандарту МЭК)
    ///
    int kod = 0;
    ///
    /// \brief короткое описание
    ///
    QString descr = "";
    ImportItem()
    {}
    ///
    /// \brief ImportItem
    /// \param ioa
    /// \param kod
    /// \param descr
    ///
    ImportItem(int ioa, int kod, QString descr)
    {
        this->ioa = ioa;
        this->kod = kod;
        this->descr = descr;
    }
};

namespace Ui {
class ImportDialog;
}

class ImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportDialog();

    /// прицепить таблицу с сигналами
    void SetModel(TableModel*);
    ~ImportDialog();

private:
    Ui::ImportDialog *ui;
    TableModel *MeasuresTable;//указатель на таблицу с сигналами
    QList<ImportItem> *importedItems; // список импортированных данных
public slots:
    void On_OpenDialogPressed();
    void On_OkPressed();
  //  void FileDialogClosed();
  //  void FileDialogAccepted(QString);
};

#endif // IMPORTDIALOG_H
