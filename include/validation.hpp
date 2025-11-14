#pragma once

#include <string>

bool isBlank(const std::string &value);
std::string trim(const std::string &value);

bool isValidName(const std::string &value);
bool isValidEmail(const std::string &value);
bool isValidPhoneNumber(const std::string &value);
bool isValidBirthDate(const std::string &value);
