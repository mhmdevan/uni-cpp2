#pragma once

#include <QMainWindow>
#include <vector>

#include "contact.hpp"
#include "contact_repository.hpp"

class QTableView;
class QLineEdit;
class QAction;
class QCloseEvent;

class ContactTableModel;
class MultiFieldProxyModel;

class MainWindow final : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(ContactRepository &repo);

    void setDbStatus(bool online, const QString &message);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    ContactRepository &repo_;
    std::vector<Contact> contacts_;

    QTableView *table_{nullptr};
    QLineEdit *search_{nullptr};

    ContactTableModel *model_{nullptr};
    MultiFieldProxyModel *proxy_{nullptr};

    QAction *refreshAction_{nullptr};
    QAction *addAction_{nullptr};
    QAction *editAction_{nullptr};
    QAction *removeAction_{nullptr};

    bool dbOnline_{false};
    QString dbMsg_;

    void buildUi();
    void buildMenu();
    void buildToolbar();

    void refreshModel();
    void updateStatusLine(const QString &extra);

    int selectedSourceRow() const;

    void refreshNow();

    void addContact();
    void editContact();
    void removeContact();

    void loadFromStorage();
    void saveToStorage();

    void applySearch(const QString &text);
};
