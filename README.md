# Phonebook (Qt Widgets) — Task 3

Небольшой «телефонный справочник» на C++17 и Qt Widgets.

Эта версия — **Task 3**: добавлено хранение в **PostgreSQL**, при этом **хранение в файле остаётся**. Идея простая: приложение **всегда должно стартовать**, даже если база недоступна.

## Как работает хранение

Используется три слоя:

- `FileContactRepository` — файл `contacts.txt`
- `DbContactRepository` — PostgreSQL (через Qt SQL / драйвер **QPSQL**)
- `DualContactRepository` — «умная» обёртка: **приоритет у БД**, но есть fallback на файл

Поведение:

- **Если БД online**
  - данные **грузятся из БД**
  - затем эти данные **синхронизируются в файл** (чтобы был актуальный оффлайн-резерв)
  - при сохранении: **сначала пишем в файл**, затем пытаемся сохранить в БД (если БД упала — данные всё равно останутся в файле)

- **Если БД offline**
  - данные **грузятся из файла**
  - все изменения **сохраняются в файл**
  - статус показывается в статус-баре (`DB: online/offline`)

## Конфиг БД (MVP)

Для MVP конфиг лежит прямо в коде: `db_config.hpp`

```cpp
struct DbConfig {
    QString host = "127.0.0.1";
    int port = 5432;
    QString name = "phonebook";
    QString user = "postgres";
    QString password = "postgres";
};
```

## Сборка (qmake)

Из корня проекта:

```bash
rm -rf build
mkdir -p build
cd build
qmake ../phonebook.pro
make -j"$(sysctl -n hw.ncpu)"
```

## Запуск (macOS)

Нормальный запуск приложения:

```bash
open -n ./phonebook.app
```

Либо напрямую бинарник:

```bash
./phonebook.app/Contents/MacOS/phonebook
```

## Файл данных

`contacts.txt` лежит в корне проекта (или рядом с рабочей директорией запуска) и используется как оффлайн-хранилище и резерв на случай проблем с БД.
