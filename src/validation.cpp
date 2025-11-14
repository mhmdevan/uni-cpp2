#include "validation.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <ctime>
#include <regex>
#include <sstream>

bool isBlank(const std::string &value)
{
    return std::all_of(value.begin(), value.end(), [](unsigned char ch)
                       { return std::isspace(ch) != 0; });
}

std::string trim(const std::string &value)
{
    auto begin = value.begin();
    while (begin != value.end() && std::isspace(static_cast<unsigned char>(*begin)) != 0)
    {
        ++begin;
    }
    auto end = value.end();
    if (begin == end)
    {
        return std::string();
    }
    do
    {
        --end;
    } while (std::distance(begin, end) > 0 && std::isspace(static_cast<unsigned char>(*end)) != 0);
    return std::string(begin, end + 1);
}

bool isValidName(const std::string &value)
{
    std::string trimmed = trim(value);
    if (trimmed.empty())
    {
        return false;
    }
    std::regex pattern("^[A-Za-zА-Яа-яЁё][A-Za-zА-Яа-яЁё0-9 -]*[A-Za-zА-Яа-яЁё0-9]$");
    if (trimmed.size() == 1)
    {
        std::regex single("^[A-Za-zА-Яа-яЁё]$");
        return std::regex_match(trimmed, single);
    }
    if (trimmed.front() == '-' || trimmed.back() == '-')
    {
        return false;
    }
    return std::regex_match(trimmed, pattern);
}

bool isValidEmail(const std::string &value)
{
    std::string trimmed = trim(value);
    if (trimmed.empty())
    {
        return false;
    }
    std::regex pattern("^[A-Za-z0-9]+@[A-Za-z0-9]+(\\.[A-Za-z0-9]+)*$");
    return std::regex_match(trimmed, pattern);
}

bool isValidPhoneNumber(const std::string &value)
{
    std::string trimmed = trim(value);
    if (trimmed.empty())
    {
        return false;
    }
    std::regex pattern("^(\\+7|8)(\\(\\d{3}\\)|\\d{3})\\d{3}-?\\d{2}-?\\d{2}$");
    return std::regex_match(trimmed, pattern);
}

static bool parseBirthDate(const std::string &value, int &day, int &month, int &year)
{
    std::string trimmed = trim(value);
    if (trimmed.empty())
    {
        return false;
    }
    char sep1;
    char sep2;
    std::istringstream input(trimmed);
    if (!(input >> day >> sep1 >> month >> sep2 >> year))
    {
        return false;
    }
    if (sep1 != '.' || sep2 != '.')
    {
        return false;
    }
    if (year < 1900)
    {
        return false;
    }
    if (month < 1 || month > 12)
    {
        return false;
    }
    int daysInMonth;
    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
    {
        daysInMonth = 31;
    }
    else if (month == 4 || month == 6 || month == 9 || month == 11)
    {
        daysInMonth = 30;
    }
    else
    {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        daysInMonth = isLeap ? 29 : 28;
    }
    if (day < 1 || day > daysInMonth)
    {
        return false;
    }
    return true;
}

bool isValidBirthDate(const std::string &value)
{
    int day = 0;
    int month = 0;
    int year = 0;
    if (!parseBirthDate(value, day, month, year))
    {
        return false;
    }
    std::tm birth{};
    birth.tm_mday = day;
    birth.tm_mon = month - 1;
    birth.tm_year = year - 1900;
    birth.tm_hour = 0;
    birth.tm_min = 0;
    birth.tm_sec = 0;
    std::time_t birthTime = std::mktime(&birth);
    if (birthTime == -1)
    {
        return false;
    }
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm *nowLocal = std::localtime(&nowTime);
    if (!nowLocal)
    {
        return false;
    }
    std::tm today = *nowLocal;
    today.tm_hour = 0;
    today.tm_min = 0;
    today.tm_sec = 0;
    std::time_t todayTime = std::mktime(&today);
    if (todayTime == -1)
    {
        return false;
    }
    return birthTime < todayTime;
}
