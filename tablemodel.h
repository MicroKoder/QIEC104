#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>
#include "ciecsignal.h"
#include <QItemSelectionModel>

class CTableModelItem: public CIECSignal{
public:
    CTableModelItem(){

    }
    CTableModelItem(CIECSignal *signal, QString name = QString()){
        this->SetAddress(signal->GetAddress());

        this->quality = signal->quality;
        this->value = signal->value;
        this->timestamp = signal->timestamp;
        this->SetType(signal->GetType());

        this->name = name;
    }
    /*CTableModelItem(int address, QString name= QString()){
        this->SetAddress(address);
        this->name = name;
    }*/
    ///
    /// \brief name
    /// наименование
    QString name;
};

class TableModel : public QAbstractTableModel
{
public:
    QMap<uint, CTableModelItem> *mData;

    TableModel();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool insertRows(int position, int rows, const QModelIndex &index);

    bool removeRow(int row, const QModelIndex &parent=QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent=QModelIndex());
    bool removeRows(QItemSelectionModel *pSelection);
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    bool isSignalExist(CIECSignal *pSignal);
    void updateSignal(CIECSignal *pSignal);
    void appendSignal(CIECSignal *pSignal);

    void redraw();

    bool bAllowAppend;      //разрешение на автоматическое добавление тэга в таблицу

};

#endif // TABLEMODEL_H
