#include "phone_number.hpp"

PhoneNumber::PhoneNumber(PhoneType type, std::string value)
    : type_(type), value_(std::move(value))
{
}

PhoneType PhoneNumber::type() const
{
    return type_;
}

const std::string &PhoneNumber::value() const
{
    return value_;
}

void PhoneNumber::setType(PhoneType type)
{
    type_ = type;
}

void PhoneNumber::setValue(const std::string &value)
{
    value_ = value;
}

std::string PhoneNumber::typeToString(PhoneType type)
{
    switch (type)
    {
    case PhoneType::Work:
        return "work";
    case PhoneType::Home:
        return "home";
    case PhoneType::Service:
        return "service";
    }
    return "home";
}

PhoneType PhoneNumber::stringToType(const std::string &value)
{
    if (value == "work")
    {
        return PhoneType::Work;
    }
    if (value == "service")
    {
        return PhoneType::Service;
    }
    return PhoneType::Home;
}
