#include "tablemodel.h"

TableModel::TableModel()
{
    //mData = new QMap<uint, CTableModelItem>();
    bAllowAppend = true;
}

int TableModel::rowCount(const QModelIndex &parent=QModelIndex()) const{
    Q_UNUSED(parent);
        return mData.count();

}

int TableModel::columnCount(const QModelIndex &parent = QModelIndex()) const{
   Q_UNUSED(parent);
    if (isShortTable)
        return 3;

    return 6;
}

QVariant TableModel::data(const QModelIndex &index, int role) const{
    int row = index.row();
    if ((role == Qt::DisplayRole)&&(mData.count()>row)&& row>=0){

    //    QString data = "row: "+QString::number(index.row())+" col: "+QString::number(index.column());
        CIECSignal item = mData[row];
        QVariant result = QVariant("");

      //  if (pSignalModel->pSignal==NULL) return result;

        switch (index.column()){
            case 0:
                result =  QVariant(item.GetAddress());
                break;  //адрес
            case 1:

                    result =  QVariant(item.description);
                break;  //название
            case 2:
                result =  QVariant(item.GetType());
                break;  //тип

            case 3:
            //TODO: подправить для 33 типа мэк
                /*if (pSignal.GetType()==30)
                    result = QVariant(pSignal.value == 1 ? "true" : "false");
                else
                    result =  pSignal.value;
        */
                switch (item.GetType())
                {
                case 1 : result = QVariant(item.value == 1 ? "ON" : "OFF");break;
                case 3 : if (item.value.toUInt() == 0)
                            result = QVariant("Intransit(0)");
                         else if (item.value.toUInt() == 1)
                            result = QVariant("OFF(1)");
                         else if (item.value.toUInt() == 2)
                            result = QVariant("ON(2)");
                         else result = QVariant("Invalid(3)");
                        break;

                case 30:result = QVariant(item.value == 1 ? "ON" : "OFF");break;
                case 31 : if (item.value == 0)
                            result = QVariant("Intransit(0)");
                         else if (item.value == 1)
                            result = QVariant("OFF(1)");
                         else if (item.value == 2)
                            result = QVariant("ON(2)");
                         else result = QVariant("Invalid(3)");
                        break;
                default:
                    result = item.value;

                }
                break;  //значение
            case 4:
                {
                QString quality_str;
                uchar qualityByte = item.quality;
                if (!item.bNeverUpdated)
                {
                    if (qualityByte&1)
                        quality_str+="OV ";

                    if (qualityByte&8)
                        quality_str+="EI ";

                    if (qualityByte&16)
                        quality_str+="BL ";

                    if (qualityByte&32)
                        quality_str+="SB ";

                    if (qualityByte&64)
                        quality_str+="NT ";

                    if (qualityByte&128)
                        quality_str+="IV ";

                    if (qualityByte==0)
                        quality_str = "GOOD";
                }
                else
                {
                      quality_str+="";
                }

                    result = QVariant(quality_str);
                }
                break;  //качество
            case 5:
                result = QVariant(item.timestamp.GetTimeString());
                break;  //метка времени
        };
        return result;
    }
    return QVariant();
}

///обновить существующий сигнал
void TableModel::updateSignal(CIECSignal pSignal, bool autoCreate, bool isImported)
{
    CIECSignal* item;
    if (mData.count()>0)
        for(int i=0; i<mData.count(); i++)
        {
            item =   &mData[i];
            if (item->GetKey() == pSignal.GetKey())
            {
                if (isImported)
                {
                    item->description = pSignal.description;
                }else
                {
                    item->value = pSignal.value;
                    item->quality = pSignal.quality;
                    item->timestamp = pSignal.timestamp;

                    item->bNeverUpdated = false;
                }


                //delete (*mData)[i].pSignal->bNeverUpdated = false;
               // emit dataChanged(index(i,0),index(i,6),{Qt::EditRole, Qt::EditRole, Qt::EditRole, Qt::EditRole, Qt::EditRole, Qt::EditRole});
                setData(index(i,3), QVariant(pSignal.value));
                setData(index(i,4), QVariant(pSignal.quality));
                setData(index(i,5), QVariant(pSignal.timestamp.GetTimeString()));
                return;
            }
        }

    if (!autoCreate)
        return;
    //если дошли до этой части кода значит сигнала небыло в таблице, добавляем
    //-добавление в пустой список
    if (mData.count()==0)
    {
        itemToAdd = pSignal;
        insertRows(0,1,QModelIndex());
        return;
    }

    //-поиск места для добавления
    int row = 0;
    do
    {

        item =   &mData[row];

        if (pSignal.GetKey()< item->GetKey())
        {

            break;
        }
        ++row;

    }while (row<mData.count());

    itemToAdd = pSignal;
    insertRows(row,1,QModelIndex());
}


void TableModel::redraw(){
    emit dataChanged(this->index(0,0),this->index(mData.count()-1,4));
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
                return tr("Тип");

            case 3:
                return tr("Значение");

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
   Q_UNUSED(index);
  beginInsertRows(QModelIndex(),position, position + rows - 1);
  for (int row = 0; row<rows; ++row)
  {
      //if (itemToAdd != nullptr)
      //{
       // CIECSignal item = new CTableModelItem(itemToAdd);
        mData.insert(position,itemToAdd);
      //}
  }

  endInsertRows();
   // itemToAdd =nullptr;
    return true;
}

bool TableModel::removeRow(int row, const QModelIndex &parent)
{
    beginRemoveRows(parent,row,row);


    mData.removeAt(row);


    endRemoveRows();
    return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent,row,row+count-1);
  //  QList<unsigned int> keys = mData->keys();

        for (int i= row; i< row+count;i++)
        {
            mData.removeAt(i);
        }


    endRemoveRows();
    return true;
}

// Compare two variants.
bool ModelIndexLessThan(const QModelIndex &v1, const QModelIndex &v2)
{
    return v1.row() < v2.row();
}

bool TableModel::removeRows(QItemSelectionModel *pSelection)
{
    //get list of selected rows and sort it
    QModelIndexList selectedRowsList = pSelection->selectedIndexes();

    QList<int> rowList;

    foreach (QModelIndex index, selectedRowsList)
        rowList.append(index.row());

    //qSort(rowList);
    std::sort(rowList.begin(),rowList.end());

    for (int i=rowList.count()-1; i>=0; i--)
        removeRow(rowList[i]);

    return true;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    //только столбец 1 (наименования) доступен для редактирования
    if (!index.isValid() || index.column()!=1)
        return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole ) {

        /*CTableModelItem pSignalModel = mData->values().at(index.row());
        pSignalModel.pSignal->description = value.toString();
*/
        CIECSignal *p;
        if (mData.count()>index.row())
            p = &mData[index.row()];

        switch (index.column())
        {
            case 0:
                    p->SetAddress(value.toInt());
                    break;

            case 1:
                    p->description = value.toString();
                    break;

            case 2:
                    p->SetType(value.toInt());
                    break;



        }
        emit(dataChanged(index, index));

        return true;
    }
    return false;
}
