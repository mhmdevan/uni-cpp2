#pragma once

#include <string>
#include <vector>

#include "phone_number.hpp"

class Contact
{
public:
    Contact() = default;

    const std::string &firstName() const;
    const std::string &lastName() const;
    const std::string &middleName() const;
    const std::string &address() const;
    const std::string &birthDate() const;
    const std::string &email() const;
    const std::vector<PhoneNumber> &phoneNumbers() const;

    void setFirstName(const std::string &value);
    void setLastName(const std::string &value);
    void setMiddleName(const std::string &value);
    void setAddress(const std::string &value);
    void setBirthDate(const std::string &value);
    void setEmail(const std::string &value);
    void setPhoneNumbers(const std::vector<PhoneNumber> &values);

private:
    std::string firstName_;
    std::string lastName_;
    std::string middleName_;
    std::string address_;
    std::string birthDate_;
    std::string email_;
    std::vector<PhoneNumber> phoneNumbers_;
};
