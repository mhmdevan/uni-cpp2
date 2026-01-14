#pragma once

#include <QSortFilterProxyModel>

class MultiFieldProxyModel : public QSortFilterProxyModel
{
public:
    explicit MultiFieldProxyModel(QObject *parent = nullptr);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
};
