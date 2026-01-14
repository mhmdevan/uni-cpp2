QT += widgets
TEMPLATE = app
TARGET = phonebook
CONFIG += c++17

INCLUDEPATH += $$PWD/include

SOURCES += \
    $$PWD/src/main.cpp \
    $$PWD/src/contact.cpp \
    $$PWD/src/phone_number.cpp \
    $$PWD/src/file_contact_repository.cpp \
    $$PWD/src/validation.cpp \
    $$PWD/src/contact_table_model.cpp \
    $$PWD/src/multi_field_proxy_model.cpp \
    $$PWD/src/contact_dialog.cpp \
    $$PWD/src/main_window.cpp

HEADERS += \
    $$PWD/include/contact.hpp \
    $$PWD/include/phone_number.hpp \
    $$PWD/include/contact_repository.hpp \
    $$PWD/include/file_contact_repository.hpp \
    $$PWD/include/validation.hpp \
    $$PWD/include/contact_table_model.hpp \
    $$PWD/include/multi_field_proxy_model.hpp \
    $$PWD/include/contact_dialog.hpp \
    $$PWD/include/main_window.hpp
