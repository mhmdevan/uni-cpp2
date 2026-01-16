#pragma once

#include <QString>
#include <vector>

#include "contact.hpp"

class ContactRepository
{
public:
    virtual ~ContactRepository() = default;

    virtual std::vector<Contact> loadAll() = 0;
    virtual void saveAll(const std::vector<Contact> &contacts) = 0;

    virtual QString lastError() const { return QString(); }
};
