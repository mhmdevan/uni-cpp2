#include "multi_field_proxy_model.hpp"

#include <QAbstractItemModel>

MultiFieldProxyModel::MultiFieldProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

bool MultiFieldProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    const auto rx = filterRegularExpression();
    if (rx.pattern().isEmpty())
        return true;

    const auto *m = sourceModel();
    if (!m)
        return true;

    const int cols = m->columnCount(sourceParent);
    for (int col = 0; col < cols; ++col)
    {
        const QModelIndex idx = m->index(sourceRow, col, sourceParent);
        const QString value = idx.data().toString();
        if (value.contains(rx))
            return true;
    }

    return false;
}
