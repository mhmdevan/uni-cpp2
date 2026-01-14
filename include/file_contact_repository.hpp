#pragma once

#include <QString>

#include "contact_repository.hpp"

class FileContactRepository : public ContactRepository
{
public:
    explicit FileContactRepository(QString filePath);

    std::vector<Contact> loadAll() override;
    void saveAll(const std::vector<Contact> &contacts) override;

private:
    QString filePath_;
};
