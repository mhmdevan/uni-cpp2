#pragma once

#include <QDate>
#include <QString>
#include <vector>

#include "phone_number.hpp"

class Contact
{
public:
    const QString &firstName() const;
    const QString &lastName() const;
    const QString &middleName() const;
    const QString &address() const;
    const QDate &birthDate() const;
    const QString &email() const;
    const std::vector<PhoneNumber> &phoneNumbers() const;

    void setFirstName(QString value);
    void setLastName(QString value);
    void setMiddleName(QString value);
    void setAddress(QString value);
    void setBirthDate(QDate value);
    void setEmail(QString value);
    void setPhoneNumbers(std::vector<PhoneNumber> values);

private:
    QString firstName_;
    QString lastName_;
    QString middleName_;
    QString address_;
    QDate birthDate_;
    QString email_;
    std::vector<PhoneNumber> phoneNumbers_;
};
