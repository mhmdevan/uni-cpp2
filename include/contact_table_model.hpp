#pragma once

#include <QAbstractTableModel>
#include <vector>

#include "contact.hpp"

class ContactTableModel final : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ContactTableModel(QObject *parent = nullptr);

    void setContacts(const std::vector<Contact> &contacts);
    const std::vector<Contact> &contacts() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    std::vector<Contact> contacts_;

    static QString phonePreview(const Contact &c);
};
