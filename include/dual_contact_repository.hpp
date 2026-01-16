#pragma once

#include <QString>
#include <vector>

#include "contact_repository.hpp"
#include "db_contact_repository.hpp"
#include "file_contact_repository.hpp"

class DualContactRepository final : public ContactRepository
{
public:
    DualContactRepository(DbContactRepository &db, FileContactRepository &file)
        : db_(db), file_(file) {}

    std::vector<Contact> loadAll() override
    {
        lastError_.clear();

        auto data = db_.loadAll();
        const QString dbErr = db_.lastError().trimmed();

        if (!dbErr.isEmpty())
        {
            lastError_ = "DB load failed: " + dbErr;
            auto fallback = file_.loadAll();
            const QString fileErr = file_.lastError().trimmed();
            if (!fileErr.isEmpty())
                lastError_ += " | File load failed: " + fileErr;
            return fallback;
        }

        file_.saveAll(data);
        const QString fileErr = file_.lastError().trimmed();
        if (!fileErr.isEmpty())
            lastError_ = "File sync after DB load failed: " + fileErr;

        return data;
    }

    void saveAll(const std::vector<Contact> &contacts) override
    {
        lastError_.clear();

        file_.saveAll(contacts);
        const QString fileErr = file_.lastError().trimmed();

        db_.saveAll(contacts);
        const QString dbErr = db_.lastError().trimmed();

        if (!fileErr.isEmpty() && !dbErr.isEmpty())
        {
            lastError_ = "File save failed: " + fileErr + " | DB save failed: " + dbErr;
            return;
        }

        if (!fileErr.isEmpty())
        {
            lastError_ = "File save failed: " + fileErr;
            return;
        }

        if (!dbErr.isEmpty())
        {
            lastError_ = "DB save failed: " + dbErr + " | saved to file";
            return;
        }
    }

    QString lastError() const override { return lastError_; }

private:
    DbContactRepository &db_;
    FileContactRepository &file_;
    QString lastError_;
};
