# Phonebook (Qt Widgets) — Task 2

Небольшое настольное приложение «Телефонный справочник» на C++17 и Qt Widgets.

На этапе **Task 2** добавлен графический интерфейс и хранение данных в файле через **QFile** (формат `contacts.txt`).

## Что умеет

- отображение контактов в таблице;
- сортировка по колонкам;
- поиск по нескольким полям (строка поиска сверху);
- добавление / редактирование / удаление контакта;
- несколько телефонных номеров у одного контакта;
- автосохранение в `contacts.txt`.

Это на Qt 5.x (проверялось на Qt **5.14.2**)

## Сборка

Из корня проекта:

```bash
rm -rf build
mkdir -p build
cd build
/Users/User/Qt5.14.2/5.14.2/clang_64/bin/qmake ../phonebook.pro
make -j
```

## Запуск на macOS

Рекомендуемый запуск — через `open`:

```bash
cd build
arch -x86_64 open -n ./phonebook.app
```

Или можно запустить напрямую бинарник:

```bash
./phonebook.app/Contents/MacOS/phonebook
```

## Вместе

```bash
rm -rf build
mkdir -p build
cd build

export PATH="$(brew --prefix qt)/bin:$PATH"

qmake ../phonebook.pro
make -j"$(sysctl -n hw.ncpu)"

open -n ./phonebook.app

./phonebook.app/Contents/MacOS/phonebook
```

## Файл данных

Данные сохраняются в `contacts.txt` рядом с приложением/в рабочей директории запуска.
Формат — одна запись на строку, поля разделены `|`, телефоны — через `,`.
