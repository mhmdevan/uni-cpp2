#pragma once

#include <QCheckBox>
#include <QDateEdit>
#include <QDialog>
#include <QLineEdit>
#include <QTableWidget>

#include "contact.hpp"

class ContactDialog : public QDialog
{
public:
    explicit ContactDialog(QWidget *parent = nullptr);

    void setContact(const Contact &contact);
    Contact contact() const;

private:
    QLineEdit *firstNameEdit_{nullptr};
    QLineEdit *lastNameEdit_{nullptr};
    QLineEdit *middleNameEdit_{nullptr};
    QLineEdit *addressEdit_{nullptr};
    QLineEdit *emailEdit_{nullptr};

    QCheckBox *birthEnabled_{nullptr};
    QDateEdit *birthEdit_{nullptr};

    QTableWidget *phonesTable_{nullptr};

    Contact contact_;

    void addPhoneRow(PhoneType type, const QString &value);
    std::vector<PhoneNumber> readPhones() const;

    bool validateAndBuild();
    void onAddPhone();
    void onRemovePhone();
};
