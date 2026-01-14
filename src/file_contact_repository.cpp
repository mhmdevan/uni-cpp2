#include "file_contact_repository.hpp"

#include <QFile>
#include <QTextStream>

#include "phone_number.hpp"

namespace
{
    QString escapeField(const QString &value)
    {
        QString out;
        out.reserve(value.size());
        for (const QChar ch : value)
        {
            if (ch == '|' || ch == ',' || ch == '\\' || ch == '\n')
                out.append('\\');
            out.append(ch);
        }
        return out;
    }

    QString unescapeField(const QString &value)
    {
        QString out;
        out.reserve(value.size());
        bool esc = false;
        for (const QChar ch : value)
        {
            if (!esc && ch == '\\')
            {
                esc = true;
                continue;
            }
            out.append(ch);
            esc = false;
        }
        return out;
    }

    std::vector<QString> splitEscaped(const QString &line, QChar delimiter)
    {
        std::vector<QString> parts;
        QString cur;
        bool esc = false;

        for (const QChar ch : line)
        {
            if (!esc && ch == '\\')
            {
                esc = true;
                continue;
            }

            if (!esc && ch == delimiter)
            {
                parts.push_back(cur);
                cur.clear();
                continue;
            }

            cur.append(ch);
            esc = false;
        }

        parts.push_back(cur);
        return parts;
    }

    QString serializePhones(const std::vector<PhoneNumber> &phones)
    {
        QString out;
        for (std::size_t i = 0; i < phones.size(); ++i)
        {
            const auto &p = phones[i];
            out += PhoneNumber::typeToString(p.type());
            out += ':';
            out += escapeField(p.value());
            if (i + 1 < phones.size())
                out += ',';
        }
        return out;
    }

    std::vector<PhoneNumber> deserializePhones(const QString &value)
    {
        std::vector<PhoneNumber> phones;
        const auto items = splitEscaped(value, ',');

        for (const auto &item : items)
        {
            if (item.isEmpty())
                continue;

            const auto pair = splitEscaped(item, ':');
            if (pair.size() != 2)
                continue;

            const PhoneType type = PhoneNumber::stringToType(pair[0]);
            const QString number = unescapeField(pair[1]);
            phones.emplace_back(type, number);
        }

        return phones;
    }

    QString dateToString(const QDate &date)
    {
        if (!date.isValid())
            return QString();
        return date.toString("dd.MM.yyyy");
    }

    QDate dateFromString(const QString &value)
    {
        const QString s = value.trimmed();
        if (s.isEmpty())
            return QDate();
        return QDate::fromString(s, "dd.MM.yyyy");
    }

    QString serializeContact(const Contact &c)
    {
        QString out;
        out += escapeField(c.firstName());
        out += '|';
        out += escapeField(c.lastName());
        out += '|';
        out += escapeField(c.middleName());
        out += '|';
        out += escapeField(c.address());
        out += '|';
        out += escapeField(dateToString(c.birthDate()));
        out += '|';
        out += escapeField(c.email());
        out += '|';
        out += serializePhones(c.phoneNumbers());
        return out;
    }

    bool deserializeContact(const QString &line, Contact &outContact)
    {
        const auto fields = splitEscaped(line, '|');
        if (fields.size() < 7)
            return false;

        Contact c;
        c.setFirstName(unescapeField(fields[0]).trimmed());
        c.setLastName(unescapeField(fields[1]).trimmed());
        c.setMiddleName(unescapeField(fields[2]).trimmed());
        c.setAddress(unescapeField(fields[3]).trimmed());
        c.setBirthDate(dateFromString(unescapeField(fields[4])));
        c.setEmail(unescapeField(fields[5]).trimmed());
        c.setPhoneNumbers(deserializePhones(fields[6]));

        if (c.firstName().isEmpty() || c.lastName().isEmpty() || c.email().isEmpty() || c.phoneNumbers().empty())
            return false;

        outContact = std::move(c);
        return true;
    }
}

FileContactRepository::FileContactRepository(QString filePath)
    : filePath_(std::move(filePath))
{
}

std::vector<Contact> FileContactRepository::loadAll()
{
    std::vector<Contact> contacts;

    QFile file(filePath_);
    if (!file.exists())
        return contacts;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return contacts;

    QTextStream in(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif

    while (!in.atEnd())
    {
        const QString line = in.readLine();
        if (line.trimmed().isEmpty())
            continue;

        Contact c;
        if (deserializeContact(line, c))
            contacts.push_back(std::move(c));
    }

    return contacts;
}

void FileContactRepository::saveAll(const std::vector<Contact> &contacts)
{
    QFile file(filePath_);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif

    for (const auto &c : contacts)
        out << serializeContact(c) << "\n";
}
