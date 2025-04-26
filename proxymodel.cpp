/*
 * Copyright (C) 2025 Zayrullin Azat / zayruaz@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
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
    QModelIndex ind= sourceModel()->index(source_row,1,source_parent);

    if (sourceModel()->data(ind).toString().contains(textFilter))
        return true;
    return false;
}

QVariant ProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return sourceModel()->headerData(section, orientation,
                                        role);
}
