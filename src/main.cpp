// src/main.cpp
#include <QApplication>
#include <QTimer>
#include <QStandardPaths>
#include <QDir>

#include "file_contact_repository.hpp"
#include "main_window.hpp"

static QString contactsFilePath()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + "/contacts.txt";
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(true);

    FileContactRepository repo(contactsFilePath());
    MainWindow w(repo);

    w.resize(1100, 650);
    w.show();

    QTimer::singleShot(0, [&w]()
                       {
        w.raise();
        w.activateWindow(); });

    return app.exec();
}
