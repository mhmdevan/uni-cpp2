#include "contact.hpp"

const std::string &Contact::firstName() const
{
    return firstName_;
}

const std::string &Contact::lastName() const
{
    return lastName_;
}

const std::string &Contact::middleName() const
{
    return middleName_;
}

const std::string &Contact::address() const
{
    return address_;
}

const std::string &Contact::birthDate() const
{
    return birthDate_;
}

const std::string &Contact::email() const
{
    return email_;
}

const std::vector<PhoneNumber> &Contact::phoneNumbers() const
{
    return phoneNumbers_;
}

void Contact::setFirstName(const std::string &value)
{
    firstName_ = value;
}

void Contact::setLastName(const std::string &value)
{
    lastName_ = value;
}

void Contact::setMiddleName(const std::string &value)
{
    middleName_ = value;
}

void Contact::setAddress(const std::string &value)
{
    address_ = value;
}

void Contact::setBirthDate(const std::string &value)
{
    birthDate_ = value;
}

void Contact::setEmail(const std::string &value)
{
    email_ = value;
}

void Contact::setPhoneNumbers(const std::vector<PhoneNumber> &values)
{
    phoneNumbers_ = values;
}
