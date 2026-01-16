#pragma once

#include <QSqlDatabase>
#include <QString>
#include <vector>

#include "contact_repository.hpp"

class DbContactRepository final : public ContactRepository
{
public:
    DbContactRepository(QString host,
                        int port,
                        QString dbName,
                        QString user,
                        QString password);

    bool initialize();
    bool isAvailable() const;

    std::vector<Contact> loadAll() override;
    void saveAll(const std::vector<Contact> &contacts) override;

    QString lastError() const override;

    QString host() const;
    int port() const;
    QString dbName() const;
    QString user() const;
    QString password() const;

private:
    QString host_;
    int port_{5432};
    QString dbName_;
    QString user_;
    QString password_;

    QString connectionName_;
    QString lastError_;
    bool available_{false};

    QSqlDatabase db();
    bool open();
    bool ensureSchema();
};
