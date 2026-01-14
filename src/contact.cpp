#include "contact.hpp"

const QString &Contact::firstName() const
{
    return firstName_;
}

const QString &Contact::lastName() const
{
    return lastName_;
}

const QString &Contact::middleName() const
{
    return middleName_;
}

const QString &Contact::address() const
{
    return address_;
}

const QDate &Contact::birthDate() const
{
    return birthDate_;
}

const QString &Contact::email() const
{
    return email_;
}

const std::vector<PhoneNumber> &Contact::phoneNumbers() const
{
    return phoneNumbers_;
}

void Contact::setFirstName(QString value)
{
    firstName_ = std::move(value);
}

void Contact::setLastName(QString value)
{
    lastName_ = std::move(value);
}

void Contact::setMiddleName(QString value)
{
    middleName_ = std::move(value);
}

void Contact::setAddress(QString value)
{
    address_ = std::move(value);
}

void Contact::setBirthDate(QDate value)
{
    birthDate_ = std::move(value);
}

void Contact::setEmail(QString value)
{
    email_ = std::move(value);
}

void Contact::setPhoneNumbers(std::vector<PhoneNumber> values)
{
    phoneNumbers_ = std::move(values);
}
