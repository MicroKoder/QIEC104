#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>
#include "ciecsignal.h"


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
    QString name;
};

class TableModel : public QAbstractTableModel
{
private:

public:
    QMap<uint, CTableModelItem> *mData;

    TableModel();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void updateSignal(CIECSignal *signal);
    void redraw();
};

#endif // TABLEMODEL_H
