#include "file_contact_repository.hpp"

#include <fstream>
#include <sstream>

namespace
{
    std::string escapeField(const std::string &value)
    {
        std::string result;
        for (char ch : value)
        {
            if (ch == '|' || ch == ',' || ch == '\\' || ch == '\n')
            {
                result.push_back('\\');
            }
            result.push_back(ch);
        }
        return result;
    }

    std::string unescapeField(const std::string &value)
    {
        std::string result;
        bool escape = false;
        for (char ch : value)
        {
            if (!escape && ch == '\\')
            {
                escape = true;
            }
            else
            {
                result.push_back(ch);
                escape = false;
            }
        }
        return result;
    }

    std::vector<std::string> split(const std::string &line, char delimiter)
    {
        std::vector<std::string> parts;
        std::string current;
        bool escape = false;
        for (char ch : line)
        {
            if (!escape && ch == '\\')
            {
                escape = true;
                continue;
            }
            if (!escape && ch == delimiter)
            {
                parts.push_back(current);
                current.clear();
            }
            else
            {
                if (escape)
                {
                    current.push_back('\\');
                    escape = false;
                }
                current.push_back(ch);
            }
        }
        parts.push_back(current);
        return parts;
    }

    std::string serializeContact(const Contact &contact)
    {
        std::ostringstream output;
        output << escapeField(contact.firstName()) << '|';
        output << escapeField(contact.lastName()) << '|';
        output << escapeField(contact.middleName()) << '|';
        output << escapeField(contact.address()) << '|';
        output << escapeField(contact.birthDate()) << '|';
        output << escapeField(contact.email()) << '|';
        const auto &phones = contact.phoneNumbers();
        for (std::size_t i = 0; i < phones.size(); ++i)
        {
            const auto &phone = phones[i];
            output << PhoneNumber::typeToString(phone.type()) << ':' << escapeField(phone.value());
            if (i + 1 < phones.size())
            {
                output << ',';
            }
        }
        return output.str();
    }

    Contact deserializeContact(const std::string &line)
    {
        auto fields = split(line, '|');
        Contact contact;
        if (fields.size() < 7)
        {
            return contact;
        }
        contact.setFirstName(unescapeField(fields[0]));
        contact.setLastName(unescapeField(fields[1]));
        contact.setMiddleName(unescapeField(fields[2]));
        contact.setAddress(unescapeField(fields[3]));
        contact.setBirthDate(unescapeField(fields[4]));
        contact.setEmail(unescapeField(fields[5]));
        std::vector<PhoneNumber> phones;
        auto phoneParts = split(fields[6], ',');
        for (const auto &part : phoneParts)
        {
            if (part.empty())
            {
                continue;
            }
            auto typeAndValue = split(part, ':');
            if (typeAndValue.size() != 2)
            {
                continue;
            }
            PhoneType type = PhoneNumber::stringToType(typeAndValue[0]);
            std::string value = unescapeField(typeAndValue[1]);
            phones.emplace_back(type, value);
        }
        contact.setPhoneNumbers(phones);
        return contact;
    }
}

FileContactRepository::FileContactRepository(std::string filePath)
    : filePath_(std::move(filePath))
{
}

std::vector<Contact> FileContactRepository::loadAll()
{
    std::vector<Contact> contacts;
    std::ifstream input(filePath_);
    if (!input.is_open())
    {
        return contacts;
    }
    std::string line;
    while (std::getline(input, line))
    {
        if (line.empty())
        {
            continue;
        }
        Contact contact = deserializeContact(line);
        if (!contact.firstName().empty() || !contact.lastName().empty())
        {
            contacts.push_back(contact);
        }
    }
    return contacts;
}

void FileContactRepository::saveAll(const std::vector<Contact> &contacts)
{
    std::ofstream output(filePath_, std::ios::trunc);
    for (const auto &contact : contacts)
    {
        output << serializeContact(contact) << '\n';
    }
}
