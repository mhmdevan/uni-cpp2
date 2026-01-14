#pragma once

#include <QString>

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
    PhoneNumber(PhoneType type, QString value);

    PhoneType type() const;
    const QString &value() const;

    void setType(PhoneType type);
    void setValue(QString value);

    static QString typeToString(PhoneType type);
    static PhoneType stringToType(const QString &value);

    static QString typeToLabel(PhoneType type);
    static PhoneType labelToType(const QString &label);

private:
    PhoneType type_{PhoneType::Home};
    QString value_;
};
