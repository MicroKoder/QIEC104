#include "tablemodel.h"

TableModel::TableModel()
{
    mData = new QMap<int, CTableModelItem>();
}

int TableModel::rowCount(const QModelIndex &parent) const{
    return mData->count();
   // return 5;
}

int TableModel::columnCount(const QModelIndex &parent) const{
   return 5;
}

QVariant TableModel::data(const QModelIndex &index, int role) const{
    if ((role == Qt::DisplayRole)&&(mData->count()>index.row())){
    //    QString data = "row: "+QString::number(index.row())+" col: "+QString::number(index.column());
        CTableModelItem pSignal = mData->values().at(index.row());
        QVariant result;
        switch (index.column()){
            case 0:
                result =  QVariant(pSignal.address);
                break;  //адрес
            case 1:
                result =  QVariant(pSignal.name);
                break;  //адресbreak;  //название
            case 2:
                result =  QVariant(pSignal.value);
                break;  //значение
            case 3:
                result = QVariant(uint(pSignal.quality));
                break;  //качество
            case 4:
                result = QVariant(pSignal.timestamp.GetTimeString());
                break;  //метка времени
        };
        return QVariant(result);
    }
    return QVariant();
}

void TableModel::updateSignal(CIECSignal signal){
    if (mData->keys().contains(signal.address)){
        (*mData)[signal.address].quality = signal.quality;
        (*mData)[signal.address].value = signal.value;
        (*mData)[signal.address].timestamp = signal.timestamp;
    }
}
void TableModel::redraw(){
    emit dataChanged(this->index(0,0),this->index(mData->count()-1,4));
}
