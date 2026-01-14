#include "phone_number.hpp"

PhoneNumber::PhoneNumber(PhoneType type, QString value)
    : type_(type), value_(std::move(value))
{
}

PhoneType PhoneNumber::type() const
{
    return type_;
}

const QString &PhoneNumber::value() const
{
    return value_;
}

void PhoneNumber::setType(PhoneType type)
{
    type_ = type;
}

void PhoneNumber::setValue(QString value)
{
    value_ = std::move(value);
}

QString PhoneNumber::typeToString(PhoneType type)
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

PhoneType PhoneNumber::stringToType(const QString &value)
{
    if (value == "work")
        return PhoneType::Work;
    if (value == "service")
        return PhoneType::Service;
    return PhoneType::Home;
}

QString PhoneNumber::typeToLabel(PhoneType type)
{
    switch (type)
    {
    case PhoneType::Work:
        return "Рабочий";
    case PhoneType::Home:
        return "Домашний";
    case PhoneType::Service:
        return "Служебный";
    }
    return "Домашний";
}

PhoneType PhoneNumber::labelToType(const QString &label)
{
    if (label == "Рабочий")
        return PhoneType::Work;
    if (label == "Служебный")
        return PhoneType::Service;
    return PhoneType::Home;
}
