#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QtSql/QSqlDatabase>

#include "db_config.hpp"
#include "db_contact_repository.hpp"
#include "dual_contact_repository.hpp"
#include "file_contact_repository.hpp"
#include "main_window.hpp"

static QString findProjectRoot()
{
    QDir dir(QDir::currentPath());
    for (int i = 0; i < 10; ++i)
    {
        if (QFileInfo::exists(dir.filePath("CMakeLists.txt")) || QFileInfo::exists(dir.filePath(".git")))
            return dir.absolutePath();
        if (!dir.cdUp())
            break;
    }
    return QDir::currentPath();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    const QString root = findProjectRoot();
    QDir::setCurrent(root);

    FileContactRepository fileRepo(QDir(root).filePath("contacts.txt"));

    DbConfig cfg;
    const bool cfgOk = cfg.isValid();

    DbContactRepository dbRepo(cfg.host, cfg.port, cfg.name, cfg.user, cfg.password);
    const bool dbOk = cfgOk && dbRepo.initialize();

    DualContactRepository dualRepo(dbRepo, fileRepo);
    ContactRepository *repo = dbOk ? static_cast<ContactRepository *>(&dualRepo)
                                   : static_cast<ContactRepository *>(&fileRepo);

    MainWindow w(*repo);

    if (dbOk)
        w.setDbStatus(true, "DB: online");
    else
        w.setDbStatus(false, cfgOk ? ("DB: offline " + dbRepo.lastError())
                                   : "DB: offline (invalid config)");

    w.show();
    return app.exec();
}
