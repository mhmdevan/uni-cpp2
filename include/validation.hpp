#pragma once

#include <QDate>
#include <QString>

QString trim(const QString &value);
QString normalizeEmail(const QString &value);

bool isValidName(const QString &value);
bool isValidEmail(const QString &value);
bool isValidPhoneNumber(const QString &value);
bool isValidBirthDate(const QDate &date);
