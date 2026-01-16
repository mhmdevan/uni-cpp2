#include "validation.hpp"

#include <QRegularExpression>

QString trim(const QString &value)
{
    return value.trimmed();
}

QString normalizeEmail(const QString &value)
{
    QString s = value;
    s.remove(QRegularExpression("\\s+"));
    return s;
}

bool isValidName(const QString &value)
{
    const QString s = trim(value);
    if (s.isEmpty())
        return false;

    if (!s.at(0).isLetter())
        return false;

    if (s.startsWith('-') || s.endsWith('-'))
        return false;

    for (const QChar ch : s)
    {
        if (ch.isLetterOrNumber())
            continue;
        if (ch == ' ' || ch == '-')
            continue;
        return false;
    }

    return true;
}

bool isValidEmail(const QString &value)
{
    const QString s = normalizeEmail(trim(value));
    if (s.isEmpty())
        return false;

    static const QRegularExpression re(
        R"(^[A-Za-z0-9._%+\-]+@[A-Za-z0-9.\-]+\.[A-Za-z]{2,}$)");
    return re.match(s).hasMatch();
}

bool isValidPhoneNumber(const QString &value)
{
    const QString s = normalizeEmail(trim(value));
    if (s.isEmpty())
        return false;

    static const QRegularExpression re("^(\\+7|8)(\\(\\d{3}\\)|\\d{3})\\d{3}(-?\\d{2}){2}$");
    return re.match(s).hasMatch();
}

bool isValidBirthDate(const QDate &date)
{
    if (!date.isValid())
        return false;
    return date < QDate::currentDate();
}
