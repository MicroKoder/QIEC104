#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QSortFilterProxyModel>
class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    ProxyModel(QObject* parent=0);
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
public slots:
    void setFilter(QString textFilter);
private:
    QString textFilter;
};

#endif // PROXYMODEL_H
