#pragma once

#include <QLineEdit>
#include <QMainWindow>
#include <QTableView>
#include <vector>

#include "contact.hpp"
#include "contact_table_model.hpp"
#include "contact_repository.hpp"
#include "multi_field_proxy_model.hpp"

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(ContactRepository &repository, QWidget *parent = nullptr);

private:
    ContactRepository &repository_;
    std::vector<Contact> contacts_;

    ContactTableModel model_;
    MultiFieldProxyModel proxy_;

    QTableView *table_{nullptr};
    QLineEdit *search_{nullptr};

    void load();
    void save();

    int selectedSourceRow() const;

    void addContact();
    void editContact();
    void removeContact();
};
