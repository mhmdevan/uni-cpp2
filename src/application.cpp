#include "application.hpp"

#include <algorithm>
#include <iostream>

#include "validation.hpp"

ConsoleApplication::ConsoleApplication(FileContactRepository &repository)
    : repository_(repository)
{
}

void ConsoleApplication::run()
{
    load();
    bool running = true;
    while (running)
    {
        showMenu();
        std::string line;
        std::getline(std::cin, line);
        if (line.empty())
        {
            continue;
        }
        int choice = std::stoi(line);
        if (choice == 0)
        {
            running = false;
        }
        else
        {
            handleChoice(choice);
        }
    }
    save();
}

void ConsoleApplication::load()
{
    contacts_ = repository_.loadAll();
}

void ConsoleApplication::save()
{
    repository_.saveAll(contacts_);
}

void ConsoleApplication::showMenu()
{
    std::cout << "1. Показать все контакты" << std::endl;
    std::cout << "2. Добавить контакт" << std::endl;
    std::cout << "3. Изменить контакт" << std::endl;
    std::cout << "4. Удалить контакт" << std::endl;
    std::cout << "5. Поиск" << std::endl;
    std::cout << "6. Сортировка" << std::endl;
    std::cout << "0. Выход" << std::endl;
    std::cout << "> ";
}

void ConsoleApplication::handleChoice(int choice)
{
    switch (choice)
    {
    case 1:
        listContacts();
        break;
    case 2:
        addContact();
        break;
    case 3:
        editContact();
        break;
    case 4:
        removeContact();
        break;
    case 5:
        searchContacts();
        break;
    case 6:
        sortContacts();
        break;
    default:
        break;
    }
}

void ConsoleApplication::listContacts()
{
    if (contacts_.empty())
    {
        std::cout << "Список контактов пуст" << std::endl;
        return;
    }
    for (std::size_t i = 0; i < contacts_.size(); ++i)
    {
        printContact(contacts_[i], i);
    }
}

void ConsoleApplication::addContact()
{
    Contact contact = readContact();
    contacts_.push_back(contact);
    save();
}

void ConsoleApplication::editContact()
{
    if (contacts_.empty())
    {
        std::cout << "Нет контактов для изменения" << std::endl;
        return;
    }
    listContacts();
    std::cout << "Введите номер контакта для изменения: ";
    std::string line;
    std::getline(std::cin, line);
    if (line.empty())
    {
        return;
    }
    std::size_t index = static_cast<std::size_t>(std::stoul(line));
    if (index >= contacts_.size())
    {
        std::cout << "Неверный индекс" << std::endl;
        return;
    }
    Contact contact = readContact();
    contacts_[index] = contact;
    save();
}

void ConsoleApplication::removeContact()
{
    if (contacts_.empty())
    {
        std::cout << "Нет контактов для удаления" << std::endl;
        return;
    }
    listContacts();
    std::cout << "Введите номер контакта для удаления: ";
    std::string line;
    std::getline(std::cin, line);
    if (line.empty())
    {
        return;
    }
    std::size_t index = static_cast<std::size_t>(std::stoul(line));
    if (index >= contacts_.size())
    {
        std::cout << "Неверный индекс" << std::endl;
        return;
    }
    contacts_.erase(contacts_.begin() + static_cast<std::ptrdiff_t>(index));
    save();
}

void ConsoleApplication::searchContacts()
{
    if (contacts_.empty())
    {
        std::cout << "Список контактов пуст" << std::endl;
        return;
    }
    std::cout << "Введите строку поиска (имя, фамилия или email): ";
    std::string query;
    std::getline(std::cin, query);
    query = trim(query);
    if (query.empty())
    {
        return;
    }
    std::vector<std::size_t> results;
    for (std::size_t i = 0; i < contacts_.size(); ++i)
    {
        const auto &c = contacts_[i];
        if (c.firstName().find(query) != std::string::npos || c.lastName().find(query) != std::string::npos || c.email().find(query) != std::string::npos)
        {
            results.push_back(i);
        }
    }
    if (results.empty())
    {
        std::cout << "Ничего не найдено" << std::endl;
        return;
    }
    for (auto index : results)
    {
        printContact(contacts_[index], index);
    }
}

void ConsoleApplication::sortContacts()
{
    if (contacts_.empty())
    {
        return;
    }
    std::cout << "Сортировать по:" << std::endl;
    std::cout << "1. Фамилия" << std::endl;
    std::cout << "2. Имя" << std::endl;
    std::cout << "3. Email" << std::endl;
    std::cout << "> ";
    std::string line;
    std::getline(std::cin, line);
    if (line.empty())
    {
        return;
    }
    int field = std::stoi(line);
    switch (field)
    {
    case 1:
        std::sort(contacts_.begin(), contacts_.end(), [](const Contact &a, const Contact &b)
                  { return a.lastName() < b.lastName(); });
        break;
    case 2:
        std::sort(contacts_.begin(), contacts_.end(), [](const Contact &a, const Contact &b)
                  { return a.firstName() < b.firstName(); });
        break;
    case 3:
        std::sort(contacts_.begin(), contacts_.end(), [](const Contact &a, const Contact &b)
                  { return a.email() < b.email(); });
        break;
    default:
        break;
    }
    save();
}

Contact ConsoleApplication::readContact()
{
    Contact contact;
    std::string value;

    while (true)
    {
        std::cout << "Имя: ";
        std::getline(std::cin, value);
        value = trim(value);
        if (isValidName(value))
        {
            contact.setFirstName(value);
            break;
        }
        std::cout << "Некорректное имя" << std::endl;
    }

    while (true)
    {
        std::cout << "Фамилия: ";
        std::getline(std::cin, value);
        value = trim(value);
        if (isValidName(value))
        {
            contact.setLastName(value);
            break;
        }
        std::cout << "Некорректная фамилия" << std::endl;
    }

    std::cout << "Отчество (можно оставить пустым): ";
    std::getline(std::cin, value);
    value = trim(value);
    if (!value.empty() && !isValidName(value))
    {
        std::cout << "Отчество будет сохранено пустым" << std::endl;
        value.clear();
    }
    contact.setMiddleName(value);

    std::cout << "Адрес: ";
    std::getline(std::cin, value);
    value = trim(value);
    contact.setAddress(value);

    while (true)
    {
        std::cout << "Дата рождения (дд.мм.гггг): ";
        std::getline(std::cin, value);
        value = trim(value);
        if (isValidBirthDate(value))
        {
            contact.setBirthDate(value);
            break;
        }
        std::cout << "Некорректная дата" << std::endl;
    }

    while (true)
    {
        std::cout << "Email: ";
        std::getline(std::cin, value);
        value = trim(value);
        if (isValidEmail(value))
        {
            contact.setEmail(value);
            break;
        }
        std::cout << "Некорректный email" << std::endl;
    }

    std::vector<PhoneNumber> phones;
    while (true)
    {
        std::cout << "Номер телефона: ";
        std::getline(std::cin, value);
        value = trim(value);
        if (!isValidPhoneNumber(value))
        {
            std::cout << "Некорректный номер" << std::endl;
            continue;
        }
        std::cout << "Тип номера (1 - рабочий, 2 - домашний, 3 - служебный): ";
        std::string typeLine;
        std::getline(std::cin, typeLine);
        int typeChoice = 1;
        if (!typeLine.empty())
        {
            typeChoice = std::stoi(typeLine);
        }
        PhoneType type = PhoneType::Work;
        if (typeChoice == 2)
        {
            type = PhoneType::Home;
        }
        else if (typeChoice == 3)
        {
            type = PhoneType::Service;
        }
        phones.emplace_back(type, value);
        std::cout << "Добавить ещё один номер? (y/n): ";
        std::string more;
        std::getline(std::cin, more);
        if (!more.empty() && (more[0] == 'y' || more[0] == 'Y'))
        {
            continue;
        }
        break;
    }
    contact.setPhoneNumbers(phones);

    return contact;
}

void ConsoleApplication::printContact(const Contact &contact, std::size_t index)
{
    std::cout << index << ": " << contact.lastName() << " " << contact.firstName();
    if (!contact.middleName().empty())
    {
        std::cout << " " << contact.middleName();
    }
    std::cout << std::endl;
    std::cout << "   Адрес: " << contact.address() << std::endl;
    std::cout << "   Дата рождения: " << contact.birthDate() << std::endl;
    std::cout << "   Email: " << contact.email() << std::endl;
    std::cout << "   Телефоны: ";
    const auto &phones = contact.phoneNumbers();
    for (std::size_t i = 0; i < phones.size(); ++i)
    {
        const auto &phone = phones[i];
        std::cout << PhoneNumber::typeToString(phone.type()) << "=" << phone.value();
        if (i + 1 < phones.size())
        {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}
