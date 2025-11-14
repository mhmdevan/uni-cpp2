#pragma once

#include <string>

enum class PhoneType
{
    Work,
    Home,
    Service
};

class PhoneNumber
{
public:
    PhoneNumber() = default;
    PhoneNumber(PhoneType type, std::string value);

    PhoneType type() const;
    const std::string &value() const;

    void setType(PhoneType type);
    void setValue(const std::string &value);

    static std::string typeToString(PhoneType type);
    static PhoneType stringToType(const std::string &value);

private:
    PhoneType type_{PhoneType::Home};
    std::string value_;
};
