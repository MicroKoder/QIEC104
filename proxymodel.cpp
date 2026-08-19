#include "proxymodel.h"

ProxyModel::ProxyModel(QObject* parent):QSortFilterProxyModel(parent),textFilter(QString())
{

}

void ProxyModel::setFilter(QString textFilter)
{
    if (this->textFilter != textFilter)
    {
        this->textFilter = textFilter;
        invalidateFilter();
    }
}

bool ProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (textFilter.isEmpty())
        return true;

    // Match description or IOA
    QModelIndex descIndex = sourceModel()->index(source_row, 1, source_parent);
    QModelIndex ioaIndex = sourceModel()->index(source_row, 0, source_parent);

    if (sourceModel()->data(descIndex).toString().contains(textFilter, Qt::CaseInsensitive))
        return true;
    if (sourceModel()->data(ioaIndex).toString().contains(textFilter, Qt::CaseInsensitive))
        return true;
    return false;
}

QVariant ProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return sourceModel()->headerData(section, orientation,
                                        role);
}
