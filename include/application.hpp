#pragma once

#include <vector>

#include "contact.hpp"
#include "file_contact_repository.hpp"

class ConsoleApplication
{
public:
    ConsoleApplication(FileContactRepository &repository);
    void run();

private:
    FileContactRepository &repository_;
    std::vector<Contact> contacts_;

    void load();
    void save();

    void showMenu();
    void handleChoice(int choice);

    void listContacts();
    void addContact();
    void editContact();
    void removeContact();
    void searchContacts();
    void sortContacts();

    Contact readContact();
    void printContact(const Contact &contact, std::size_t index);
};
