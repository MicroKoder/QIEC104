#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>
#include "ciecsignal.h"
#include <QItemSelectionModel>
#include <QStringListModel.h>

///
/// \brief Отображаемый элемент таблицы сигналов
///
/*class CTableModelItem{
public:
    CTableModelItem(){
    }
    CTableModelItem(CIECSignal *signal){

        pSignal = *signal;
*/

/*
        this->SetAddress(signal->GetAddress());

        this->quality = signal->quality;
        this->value = signal->value;
        this->timestamp = signal->timestamp;
        this->SetType(signal->GetType());

        this->name = name;*/
 //   }
    /*CTableModelItem(int address, QString name= QString()){
        this->SetAddress(address);
        this->name = name;
    }*/

  /*  CIECSignal pSignal;

};
*/
//таблица измерений (сигналы в направлении контроля)
class TableModel : public QAbstractTableModel
{
    CIECSignal itemToAdd;
public:
    //QMap<uint, CTableModelItem> *mData;
    QList<CIECSignal> mData;
    TableModel();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool insertRows(int position, int rows, const QModelIndex &index);

    bool removeRow(int row, const QModelIndex &parent=QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent=QModelIndex());
    bool removeRows(QItemSelectionModel *pSelection);

    //bool isSignalExist(CIECSignal*);
    void updateSignal(CIECSignal, bool autoCreate = true,bool isImported=false);
    //void appendSignal(CIECSignal*);
    //void appendSignal(CIECSignal*, QString);

    void redraw();

    bool bAllowAppend;      //разрешение на автоматическое добавление тэга в таблицу

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value,
                    int role = Qt::EditRole);
};

#endif // TABLEMODEL_H
