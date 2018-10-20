#include "tablemodel.h"

TableModel::TableModel()
{
    //mData = new QMap<uint, CTableModelItem>();
    mData = new QList<CTableModelItem>();
    bAllowAppend = true;
}

int TableModel::rowCount(const QModelIndex &parent=QModelIndex()) const{
    Q_UNUSED(parent);
        return mData->count();

}

int TableModel::columnCount(const QModelIndex &parent = QModelIndex()) const{
   Q_UNUSED(parent);
    return 6;
}

QVariant TableModel::data(const QModelIndex &index, int role) const{
    if ((role == Qt::DisplayRole)&&(mData->count()>index.row())){
    //    QString data = "row: "+QString::number(index.row())+" col: "+QString::number(index.column());
        CTableModelItem pSignalModel = mData->at(index.row());
        QVariant result = QVariant("");

        if (pSignalModel.pSignal==NULL) return result;

        switch (index.column()){
            case 0:
                result =  QVariant(pSignalModel.pSignal->GetAddress());
                break;  //адрес
            case 1:
                    result =  QVariant(pSignalModel.pSignal->description);
                break;  //название
            case 2:
            //TODO: подправить для 33 типа мэк
                /*if (pSignal.GetType()==30)
                    result = QVariant(pSignal.value == 1 ? "true" : "false");
                else
                    result =  pSignal.value;
        */
                switch (pSignalModel.pSignal->GetType())
                {
                case 1 : result = QVariant(pSignalModel.pSignal->value == 1 ? "ON" : "OFF");break;
                case 3 : if (pSignalModel.pSignal->value.toUInt() == 0)
                            result = QVariant("Intransit(0)");
                         else if (pSignalModel.pSignal->value.toUInt() == 1)
                            result = QVariant("OFF(1)");
                         else if (pSignalModel.pSignal->value.toUInt() == 2)
                            result = QVariant("ON(2)");
                         else result = QVariant("Invalid(3)");
                        break;

                case 30:result = QVariant(pSignalModel.pSignal->value == 1 ? "ON" : "OFF");break;
                case 31 : if (pSignalModel.pSignal->value == 0)
                            result = QVariant("Intransit(0)");
                         else if (pSignalModel.pSignal->value == 1)
                            result = QVariant("OFF(1)");
                         else if (pSignalModel.pSignal->value == 2)
                            result = QVariant("ON(2)");
                         else result = QVariant("Invalid(3)");
                        break;
                default:
                    result = pSignalModel.pSignal->value;

                }
                break;  //значение
            case 3:
                result =  QVariant(pSignalModel.pSignal->GetType());
                break;  //тип
            case 4:
                {
                QString quality_str;
                if (!pSignalModel.pSignal->bNeverUpdated)
                {
                    if (pSignalModel.pSignal->quality&1)
                        quality_str+="OV ";

                    if (pSignalModel.pSignal->quality&8)
                        quality_str+="EI ";

                    if (pSignalModel.pSignal->quality&16)
                        quality_str+="BL ";

                    if (pSignalModel.pSignal->quality&32)
                        quality_str+="SB ";

                    if (pSignalModel.pSignal->quality&64)
                        quality_str+="NT ";

                    if (pSignalModel.pSignal->quality&128)
                        quality_str+="IV ";

                    if (pSignalModel.pSignal->quality==0)
                        quality_str = "GOOD";
                }
                else
                {
                      quality_str+="NULL";
                }

                    result = QVariant(quality_str);
                }
                break;  //качество
            case 5:
                result = QVariant(pSignalModel.pSignal->timestamp.GetTimeString());
                break;  //метка времени
        };
        return result;
    }
    return QVariant();
}

///обновить существующий сигнал
void TableModel::updateSignal(CIECSignal *pSignal, bool autoCreate)
{
    CTableModelItem item;
    if (mData->count()>0)
        for(int i=0; i<mData->count(); i++)
        {
            item =   mData->at(i);
            if (item.pSignal->GetKey() == pSignal->GetKey())
            {
                delete (*mData)[i].pSignal;
                (*mData)[i].pSignal = pSignal;
                //delete (*mData)[i].pSignal->bNeverUpdated = false;
                emit dataChanged(QModelIndex(),QModelIndex(),{Qt::EditRole});
                return;
            }
        }

    if (!autoCreate)
        return;
    //если дошли до этой части кода значит сигнала небыло в таблице, добавляем
    //-добавление в пустой список
    if (mData->count()==0)
    {
        itemToAdd = pSignal;
        insertRows(0,1,QModelIndex());
        return;
    }

    //-поиск места для добавления
    int row = 0;
    do
    {

        item =   mData->at(row);

        if (pSignal->GetKey()< item.pSignal->GetKey())
        {

            break;
        }
        ++row;

    }while (row<mData->count());

    itemToAdd = pSignal;
    insertRows(row,1,QModelIndex());
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
   Q_UNUSED(index);
  beginInsertRows(QModelIndex(),position, position + rows - 1);
  for (int row = 0; row<rows; ++row)
  {
      if (itemToAdd != nullptr)
      {
        CTableModelItem item = CTableModelItem(itemToAdd);
        mData->insert(position,item);
      }
  }

  endInsertRows();
    itemToAdd =nullptr;
    return true;
}

bool TableModel::removeRow(int row, const QModelIndex &parent)
{
    beginRemoveRows(parent,row,row);


    mData->removeAt(row);


    endRemoveRows();
    return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent,row,row+count-1);
  //  QList<unsigned int> keys = mData->keys();

        for (int i= row; i< row+count;i++)
        {
            mData->removeAt(i);
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

    qSort(rowList);

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
        CTableModelItem p;
        if (mData->count()>index.row())
            p = (*mData)[index.row()];

        switch (index.column())
        {
            case 0:
                    p.pSignal->SetAddress(value.toInt());
            break;
            case 1:
                    p.pSignal->description = value.toString();
            break;
            case 3:
                    p.pSignal->SetType(value.toInt());
            break;
            break;

        }
        (*mData)[index.row()] = p;
        emit(dataChanged(index, index));

        return true;
    }
    return false;
}
