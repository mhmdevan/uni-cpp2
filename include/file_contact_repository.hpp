#pragma once

#include <string>

#include "contact_repository.hpp"

class FileContactRepository : public ContactRepository
{
public:
    explicit FileContactRepository(std::string filePath);

    std::vector<Contact> loadAll() override;
    void saveAll(const std::vector<Contact> &contacts) override;

private:
    std::string filePath_;
};
