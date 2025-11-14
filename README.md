# Телефонный справочник (Task 0)

## Описание

Проект: настольное приложение «Телефонный справочник» на C++ с использованием Qt.  
Дальше будут версии с хранением в файле и в PostgreSQL, а также GUI на Qt.

Текущий этап — **Task 0**: только архитектура и структура проекта, без реализации логики.

---

## Структура проекта

```text
project-root/
├── README.md
├── Makefile
├── include/
│   ├── contact.hpp
│   ├── phone_number.hpp
│   ├── contact_repository.hpp
│   ├── file_contact_repository.hpp
│   ├── db_contact_repository.hpp
│   └── validation.hpp
├── src/
│   ├── main.cpp
│   ├── contact.cpp
│   ├── phone_number.cpp
│   ├── file_contact_repository.cpp
│   ├── db_contact_repository.cpp
│   └── validation.cpp
└── build/
```

---

## Основные классы

- `Contact` — один контакт (имя, фамилия, отчество, адрес, дата рождения, email, номера телефонов).
- `PhoneNumber` — номер телефона + тип (рабочий, домашний, служебный).
- `ContactRepository` — интерфейс хранилища контактов.
- `FileContactRepository` — реализация хранилища в файле.
- `DbContactRepository` — реализация хранилища в PostgreSQL.
- Модуль `validation` — проверка корректности полей через регулярные выражения.

---

## Принципы

Питался написать с учётом принципов **SOLID**, **DRY**, **KISS**, **YAGNI**:  

---
