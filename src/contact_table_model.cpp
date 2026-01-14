#include "contact_table_model.hpp"

#include <QStringList>

ContactTableModel::ContactTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void ContactTableModel::setContacts(std::vector<Contact> *contacts)
{
    beginResetModel();
    contacts_ = contacts;
    endResetModel();
}

int ContactTableModel::rowCount(const QModelIndex &) const
{
    if (!contacts_)
        return 0;
    return static_cast<int>(contacts_->size());
}

int ContactTableModel::columnCount(const QModelIndex &) const
{
    return 7;
}

QVariant ContactTableModel::data(const QModelIndex &index, int role) const
{
    if (!contacts_ || !index.isValid())
        return {};

    if (role != Qt::DisplayRole)
        return {};

    const auto &c = contacts_->at(static_cast<std::size_t>(index.row()));

    switch (index.column())
    {
    case 0:
        return c.lastName();
    case 1:
        return c.firstName();
    case 2:
        return c.middleName();
    case 3:
        return c.birthDate().isValid() ? c.birthDate().toString("dd.MM.yyyy") : QString();
    case 4:
        return c.email();
    case 5:
        return c.address();
    case 6:
        return phonesToString(c);
    default:
        return {};
    }
}

QVariant ContactTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

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
            return "Дата рождения";
        case 4:
            return "Email";
        case 5:
            return "Адрес";
        case 6:
            return "Телефоны";
        default:
            return {};
        }
    }

    return section + 1;
}

Qt::ItemFlags ContactTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void ContactTableModel::addContact(Contact contact)
{
    if (!contacts_)
        return;

    const int row = static_cast<int>(contacts_->size());
    beginInsertRows(QModelIndex(), row, row);
    contacts_->push_back(std::move(contact));
    endInsertRows();
}

bool ContactTableModel::updateContact(int row, Contact contact)
{
    if (!contacts_ || row < 0 || row >= static_cast<int>(contacts_->size()))
        return false;

    (*contacts_)[static_cast<std::size_t>(row)] = std::move(contact);
    const QModelIndex left = index(row, 0);
    const QModelIndex right = index(row, columnCount() - 1);
    emit dataChanged(left, right);
    return true;
}

bool ContactTableModel::removeContact(int row)
{
    if (!contacts_ || row < 0 || row >= static_cast<int>(contacts_->size()))
        return false;

    beginRemoveRows(QModelIndex(), row, row);
    contacts_->erase(contacts_->begin() + row);
    endRemoveRows();
    return true;
}

const Contact *ContactTableModel::contactAt(int row) const
{
    if (!contacts_ || row < 0 || row >= static_cast<int>(contacts_->size()))
        return nullptr;
    return &(*contacts_)[static_cast<std::size_t>(row)];
}

QString ContactTableModel::phonesToString(const Contact &c) const
{
    QStringList parts;
    for (const auto &p : c.phoneNumbers())
    {
        parts << (PhoneNumber::typeToLabel(p.type()) + ": " + p.value());
    }
    return parts.join(", ");
}
