#pragma once

#include <QString>

struct DbConfig
{
    QString host = "127.0.0.1";
    int port = 5432;
    QString name = "phonebook";
    QString user = "postgres";
    QString password = "postgres";

    bool isValid() const
    {
        return !host.trimmed().isEmpty() && port > 0 && !name.trimmed().isEmpty() && !user.trimmed().isEmpty();
    }
};
