#pragma once

#include <QAbstractTableModel>
#include <vector>

#include "contact.hpp"

class ContactTableModel : public QAbstractTableModel
{
public:
    explicit ContactTableModel(QObject *parent = nullptr);

    void setContacts(std::vector<Contact> *contacts);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void addContact(Contact contact);
    bool updateContact(int row, Contact contact);
    bool removeContact(int row);

    const Contact *contactAt(int row) const;

private:
    std::vector<Contact> *contacts_{nullptr};

    QString phonesToString(const Contact &c) const;
};
