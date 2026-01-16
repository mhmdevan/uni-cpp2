TEMPLATE = app
CONFIG += c++17
QT += widgets sql concurrent

INCLUDEPATH += $$PWD/include
DEPENDPATH  += $$PWD/include
INCLUDEPATH += $$PWD/third_party

SOURCES += \
    src/main.cpp \
    src/contact.cpp \
    src/phone_number.cpp \
    src/validation.cpp \
    src/file_contact_repository.cpp \
    src/db_contact_repository.cpp \
    src/contact_table_model.cpp \
    src/multi_field_proxy_model.cpp \
    src/contact_dialog.cpp \
    src/main_window.cpp \


HEADERS += \
    include/contact.hpp \
    include/phone_number.hpp \
    include/validation.hpp \
    include/contact_repository.hpp \
    include/file_contact_repository.hpp \
    include/db_contact_repository.hpp \
    include/contact_table_model.hpp \
    include/multi_field_proxy_model.hpp \
    include/contact_dialog.hpp \
    include/main_window.hpp \
    include/db_config.hpp \
    include/db_config.hpp \
    dual_contact_repository.hpp
