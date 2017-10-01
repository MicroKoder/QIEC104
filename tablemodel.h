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
    CTableModelItem(CIECSignal &signal, QString name = QString()){
        this->address = signal.address;

        this->quality = signal.quality;
        this->value = signal.value;
        this->timestamp = signal.timestamp;
        this->typeID = signal.typeID;

        this->name = name;
    }
    CTableModelItem(int address, QString name= QString()){
        this->address = address;
        this->name = name;
    }
    QString name;
};

class TableModel : public QAbstractTableModel
{
private:

public:
    QMap<int, CTableModelItem> *mData;

    TableModel();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    void updateSignal(CIECSignal signal);
    void redraw();
};

#endif // TABLEMODEL_H
