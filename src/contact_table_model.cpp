#include "contact_table_model.hpp"

#include <QString>

ContactTableModel::ContactTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void ContactTableModel::setContacts(const std::vector<Contact> &contacts)
{
    beginResetModel();
    contacts_ = contacts;
    endResetModel();
}

const std::vector<Contact> &ContactTableModel::contacts() const
{
    return contacts_;
}

int ContactTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(contacts_.size());
}

int ContactTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 6;
}

QVariant ContactTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const int row = index.row();
    const int col = index.column();

    if (row < 0 || static_cast<std::size_t>(row) >= contacts_.size())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    const Contact &c = contacts_[static_cast<std::size_t>(row)];

    switch (col)
    {
    case 0:
        return c.lastName();
    case 1:
        return c.firstName();
    case 2:
        return c.middleName();
    case 3:
        return c.email();
    case 4:
        return c.birthDate().isValid() ? c.birthDate().toString("dd.MM.yyyy") : QString();
    case 5:
        return phonePreview(c);
    default:
        return QVariant();
    }
}

QVariant ContactTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return "Фамилия";
        case 1:
            return "Имя";
        case 2:
            return "Отчество";
        case 3:
            return "Email";
        case 4:
            return "Дата рождения";
        case 5:
            return "Телефоны";
        default:
            return QVariant();
        }
    }

    return section + 1;
}

QString ContactTableModel::phonePreview(const Contact &c)
{
    const auto &phones = c.phoneNumbers();
    if (phones.empty())
        return QString();

    QStringList out;
    out.reserve(static_cast<int>(phones.size()));

    for (const auto &p : phones)
    {
        out.push_back(p.value());
    }

    return out.join(", ");
}
