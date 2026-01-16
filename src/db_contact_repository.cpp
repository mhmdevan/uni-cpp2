#include "db_contact_repository.hpp"

#include <QLoggingCategory>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QUuid>
#include <QHash>

#include "phone_number.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QPluginLoader>
#include <QtSql/QSqlDatabase>

static QString diagnoseQpsqlPlugin()
{
    QStringList searched;

    const QStringList libPaths = QCoreApplication::libraryPaths();
    for (const QString &base : libPaths)
    {
        const QString candidate = QDir(base).filePath("sqldrivers/libqsqlpsql.dylib");
        searched << candidate;

        if (QFileInfo::exists(candidate))
        {
            QPluginLoader loader(candidate);
            if (loader.load())
            {
                loader.unload();
                return QString("QPSQL plugin found and loadable: %1").arg(candidate);
            }
            else
            {
                return QString("QPSQL plugin found BUT failed to load: %1\nPlugin error: %2")
                    .arg(candidate, loader.errorString());
            }
        }
    }

    return QString("QPSQL plugin NOT found.\nSearched:\n- %1")
        .arg(searched.join("\n- "));
}


Q_LOGGING_CATEGORY(logDb, "phonebook.db")

DbContactRepository::DbContactRepository(QString host,
                                         int port,
                                         QString dbName,
                                         QString user,
                                         QString password)
    : host_(std::move(host)),
      port_(port),
      dbName_(std::move(dbName)),
      user_(std::move(user)),
      password_(std::move(password)),
      connectionName_(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
}

QString DbContactRepository::host() const { return host_; }
int DbContactRepository::port() const { return port_; }
QString DbContactRepository::dbName() const { return dbName_; }
QString DbContactRepository::user() const { return user_; }
QString DbContactRepository::password() const { return password_; }

bool DbContactRepository::initialize()
{
    lastError_.clear();
    available_ = false;

    if (!QSqlDatabase::isDriverAvailable("QPSQL"))
    {
        lastError_ =
            "Qt SQL driver QPSQL is NOT available.\n"
            "Available drivers: " +
            QSqlDatabase::drivers().join(", ") + "\n\n" +
            diagnoseQpsqlPlugin() + "\n\n" +
            "Hint: either libqsqlpsql.dylib is missing, or it can't load due to missing libpq / wrong architecture.";
        return false;
    }

    if (!open())
        return false;

    if (!ensureSchema())
        return false;

    available_ = true;
    return true;
}

bool DbContactRepository::isAvailable() const
{
    return available_;
}

QString DbContactRepository::lastError() const
{
    return lastError_;
}

QSqlDatabase DbContactRepository::db()
{
    if (QSqlDatabase::contains(connectionName_))
        return QSqlDatabase::database(connectionName_);

    QSqlDatabase database = QSqlDatabase::addDatabase("QPSQL", connectionName_);
    database.setHostName(host_);
    database.setPort(port_);
    database.setDatabaseName(dbName_);
    database.setUserName(user_);
    database.setPassword(password_);
    database.setConnectOptions("connect_timeout=2");
    return database;
}

bool DbContactRepository::open()
{
    lastError_.clear();

    QSqlDatabase database = db();
    if (database.isOpen())
        return true;

    if (database.open())
    {
        qCInfo(logDb) << "DB connected:" << database.hostName() << database.port() << database.databaseName() << database.userName();
        return true;
    }

    lastError_ = database.lastError().text();
    qCWarning(logDb) << "DB open failed:" << lastError_;
    return false;
}

bool DbContactRepository::ensureSchema()
{
    lastError_.clear();

    QSqlDatabase database = db();
    QSqlQuery q(database);

    if (!q.exec("CREATE TABLE IF NOT EXISTS contacts ("
                "id BIGSERIAL PRIMARY KEY,"
                "first_name TEXT NOT NULL,"
                "last_name TEXT NOT NULL,"
                "middle_name TEXT NOT NULL DEFAULT '',"
                "address TEXT NOT NULL DEFAULT '',"
                "birth_date DATE,"
                "email TEXT NOT NULL"
                ");"))
    {
        lastError_ = q.lastError().text();
        qCWarning(logDb) << "ensureSchema contacts failed:" << lastError_;
        return false;
    }

    if (!q.exec("CREATE TABLE IF NOT EXISTS phones ("
                "id BIGSERIAL PRIMARY KEY,"
                "contact_id BIGINT NOT NULL REFERENCES contacts(id) ON DELETE CASCADE,"
                "type SMALLINT NOT NULL,"
                "value TEXT NOT NULL"
                ");"))
    {
        lastError_ = q.lastError().text();
        qCWarning(logDb) << "ensureSchema phones failed:" << lastError_;
        return false;
    }

    q.exec("CREATE INDEX IF NOT EXISTS idx_contacts_last_name ON contacts(last_name);");
    q.exec("CREATE INDEX IF NOT EXISTS idx_contacts_first_name ON contacts(first_name);");
    q.exec("CREATE INDEX IF NOT EXISTS idx_contacts_email ON contacts(email);");
    q.exec("CREATE INDEX IF NOT EXISTS idx_phones_value ON phones(value);");

    qCInfo(logDb) << "Schema ensured";
    return true;
}

std::vector<Contact> DbContactRepository::loadAll()
{
    lastError_.clear();

    if (!open())
        return {};

    if (!ensureSchema())
        return {};

    QSqlDatabase database = db();

    QSqlQuery c(database);
    if (!c.exec("SELECT id, first_name, last_name, middle_name, address, birth_date, email "
                "FROM contacts ORDER BY id;"))
    {
        lastError_ = c.lastError().text();
        qCWarning(logDb) << "loadAll contacts query failed:" << lastError_;
        return {};
    }

    std::vector<Contact> contacts;
    contacts.reserve(256);

    QHash<qint64, int> idToIndex;

    while (c.next())
    {
        const qint64 id = c.value(0).toLongLong();

        Contact contact;
        contact.setFirstName(c.value(1).toString());
        contact.setLastName(c.value(2).toString());
        contact.setMiddleName(c.value(3).toString());
        contact.setAddress(c.value(4).toString());
        contact.setBirthDate(c.value(5).toDate());
        contact.setEmail(c.value(6).toString());

        idToIndex.insert(id, static_cast<int>(contacts.size()));
        contacts.push_back(std::move(contact));
    }

    if (contacts.empty())
    {
        qCInfo(logDb) << "loadAll: DB empty";
        return contacts;
    }

    QSqlQuery p(database);
    if (!p.exec("SELECT contact_id, type, value FROM phones ORDER BY contact_id, id;"))
    {
        lastError_ = p.lastError().text();
        qCWarning(logDb) << "loadAll phones query failed:" << lastError_;
        return {};
    }

    std::vector<std::vector<PhoneNumber>> phonesByRow;
    phonesByRow.resize(contacts.size());

    while (p.next())
    {
        const qint64 contactId = p.value(0).toLongLong();
        const int type = p.value(1).toInt();
        const QString value = p.value(2).toString();

        auto it = idToIndex.find(contactId);
        if (it == idToIndex.end())
            continue;

        PhoneType phoneType = PhoneType::Home;
        if (type == 0)
            phoneType = PhoneType::Work;
        else if (type == 2)
            phoneType = PhoneType::Service;

        phonesByRow[static_cast<std::size_t>(*it)].emplace_back(phoneType, value);
    }

    for (std::size_t i = 0; i < contacts.size(); ++i)
    {
        contacts[i].setPhoneNumbers(std::move(phonesByRow[i]));
    }

    qCInfo(logDb) << "loadAll OK. contacts:" << contacts.size();
    return contacts;
}

void DbContactRepository::saveAll(const std::vector<Contact> &contacts)
{
    lastError_.clear();

    if (!open())
        return;

    if (!ensureSchema())
        return;

    QSqlDatabase database = db();
    if (!database.transaction())
    {
        lastError_ = database.lastError().text();
        qCWarning(logDb) << "transaction failed:" << lastError_;
        return;
    }

    QSqlQuery q(database);

    if (!q.exec("DELETE FROM phones;"))
    {
        lastError_ = q.lastError().text();
        database.rollback();
        qCWarning(logDb) << "delete phones failed:" << lastError_;
        return;
    }

    if (!q.exec("DELETE FROM contacts;"))
    {
        lastError_ = q.lastError().text();
        database.rollback();
        qCWarning(logDb) << "delete contacts failed:" << lastError_;
        return;
    }

    QSqlQuery insertContact(database);
    if (!insertContact.prepare(
            "INSERT INTO contacts(first_name, last_name, middle_name, address, birth_date, email) "
            "VALUES (?, ?, ?, ?, ?, ?) RETURNING id;"))
    {
        lastError_ = insertContact.lastError().text();
        database.rollback();
        qCWarning(logDb) << "prepare insertContact failed:" << lastError_;
        return;
    }

    QSqlQuery insertPhone(database);
    if (!insertPhone.prepare("INSERT INTO phones(contact_id, type, value) VALUES (?, ?, ?);"))
    {
        lastError_ = insertPhone.lastError().text();
        database.rollback();
        qCWarning(logDb) << "prepare insertPhone failed:" << lastError_;
        return;
    }

    for (const auto &c : contacts)
    {
        insertContact.bindValue(0, c.firstName());
        insertContact.bindValue(1, c.lastName());
        insertContact.bindValue(2, c.middleName());
        insertContact.bindValue(3, c.address());
        insertContact.bindValue(4, c.birthDate());
        insertContact.bindValue(5, c.email());

        if (!insertContact.exec())
        {
            lastError_ = insertContact.lastError().text();
            database.rollback();
            qCWarning(logDb) << "insert contact failed:" << lastError_;
            return;
        }

        qint64 contactId = 0;
        if (insertContact.next())
            contactId = insertContact.value(0).toLongLong();

        for (const auto &ph : c.phoneNumbers())
        {
            int type = 1;
            if (ph.type() == PhoneType::Work)
                type = 0;
            else if (ph.type() == PhoneType::Service)
                type = 2;

            insertPhone.bindValue(0, contactId);
            insertPhone.bindValue(1, type);
            insertPhone.bindValue(2, ph.value());

            if (!insertPhone.exec())
            {
                lastError_ = insertPhone.lastError().text();
                database.rollback();
                qCWarning(logDb) << "insert phone failed:" << lastError_;
                return;
            }
        }
    }

    if (!database.commit())
    {
        lastError_ = database.lastError().text();
        database.rollback();
        qCWarning(logDb) << "commit failed:" << lastError_;
        return;
    }

    qCInfo(logDb) << "saveAll OK. contacts:" << contacts.size();
}
