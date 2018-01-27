#include "tablemodel.h"

TableModel::TableModel()
{
    mData = new QMap<uint, CTableModelItem>();
    bAllowAppend = true;
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
            //TODO: подправить для 33 типа мэк
                if (pSignal.GetType()==30)
                    result = QVariant(pSignal.value == 1 ? "true" : "false");
                else
                    result =  pSignal.value;


                break;  //значение
            case 3:
                result =  QVariant(pSignal.GetType());
                break;  //тип
            case 4:
                {
                QString quality_str;
                    if (pSignal.quality&1)
                        quality_str+="OV ";

                    if (pSignal.quality&8)
                        quality_str+="EI ";

                    if (pSignal.quality&16)
                        quality_str+="BL ";

                    if (pSignal.quality&32)
                        quality_str+="SB ";

                    if (pSignal.quality&64)
                        quality_str+="NT ";

                    if (pSignal.quality&128)
                        quality_str+="IV ";

                    result = QVariant(quality_str);
                }
                break;  //качество
            case 5:
                result = QVariant(pSignal.timestamp.GetTimeString());
                break;  //метка времени
        };
        return result;
    }
    return QVariant();
}

bool TableModel::isSignalExist(CIECSignal *pSignal)
{
    return mData->keys().contains(pSignal->GetKey());
}
void TableModel::updateSignal(CIECSignal *pSignal)
{
    if (isSignalExist(pSignal))
    {
        (*mData)[pSignal->GetKey()].quality = pSignal->quality;
        (*mData)[pSignal->GetKey()].value = pSignal->value;
        (*mData)[pSignal->GetKey()].timestamp = pSignal->timestamp;
    }
}

void TableModel::appendSignal(CIECSignal *pSignal)
{
    if (!isSignalExist(pSignal))
    {
        mData->insert(pSignal->GetKey(), CTableModelItem(pSignal));
        (*mData)[pSignal->GetKey()].quality = pSignal->quality;
        (*mData)[pSignal->GetKey()].value = pSignal->value;
        (*mData)[pSignal->GetKey()].timestamp = pSignal->timestamp;
    }
}

void TableModel::redraw(){
    emit dataChanged(this->index(0,0),this->index(mData->count()-1,4));
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("IOA");

            case 1:
                return tr("Описание");

            case 2:
                return tr("Значение");

            case 3:
                return tr("Тип");

            case 4:
                return tr("Качество");

            case 5:
                return tr("Время");

        default:
                return QVariant();
        }
    }
    return QVariant();
}

bool TableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    beginInsertRows(QModelIndex(),position, position + rows - 1);
        CIECSignal* temp = new CIECSignal(1,1);
        mData->insert(temp->GetKey(),CTableModelItem(temp,""));
    endInsertRows();
}

bool TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(),row,row+count-1);
    endRemoveRows();
    return true;
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
  /*      int row = index.row();

        QPair<QString, QString> p = listOfPairs.value(row);

        if (index.column() == 0)
            p.first = value.toString();
        else if (index.column() == 1)
            p.second = value.toString();
        else
            return false;

        listOfPairs.replace(row, p);
        emit(dataChanged(index, index));

        return true;*/
    }

    return false;
}
