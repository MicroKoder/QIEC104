#include "tablemodel.h"

TableModel::TableModel()
{
    mData = new QMap<uint, CTableModelItem>();
}

int TableModel::rowCount(const QModelIndex &parent) const{
    return mData->count();
   // return 5;
}

int TableModel::columnCount(const QModelIndex &parent) const{
   return 6;
}

QVariant TableModel::data(const QModelIndex &index, int role) const{
    if ((role == Qt::DisplayRole)&&(mData->count()>index.row())){
    //    QString data = "row: "+QString::number(index.row())+" col: "+QString::number(index.column());
        CTableModelItem pSignal = mData->values().at(index.row());
        QVariant result;
        switch (index.column()){
            case 0:
                result =  QVariant(pSignal.GetAddress());
                break;  //адрес
            case 1:
                result =  QVariant(pSignal.name);
                break;  //название
            case 2:
                if (pSignal.GetType()==30)
                    result = QVariant(pSignal.value == 1 ? "true" : "false");
                else
                    result =  QVariant(pSignal.value);

                break;  //значение
            case 3:
                result =  QVariant(pSignal.GetType());
                break;  //тип
            case 4:
                result = QVariant(uint(pSignal.quality));
                break;  //качество
            case 5:
                result = QVariant(pSignal.timestamp.GetTimeString());
                break;  //метка времени
        };
        return QVariant(result);
    }
    return QVariant();
}

void TableModel::updateSignal(CIECSignal *pSignal){
    if (!mData->keys().contains(pSignal->GetKey()))
    {
        mData->insert(pSignal->GetKey(), CTableModelItem(pSignal));
    }

    (*mData)[pSignal->GetKey()].quality = pSignal->quality;
    (*mData)[pSignal->GetKey()].value = pSignal->value;
    (*mData)[pSignal->GetKey()].timestamp = pSignal->timestamp;
    //(*mData)[pSignal->GetKey()] = (*pSignal);
}
void TableModel::redraw(){
    emit dataChanged(this->index(0,0),this->index(mData->count()-1,4));
}
