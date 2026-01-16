#include "main_window.hpp"

#include <QAction>
#include <QAbstractItemView>
#include <QCloseEvent>
#include <QHeaderView>
#include <QKeySequence>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QRegularExpression>
#include <QStatusBar>
#include <QTableView>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

#include "contact_dialog.hpp"
#include "contact_table_model.hpp"
#include "multi_field_proxy_model.hpp"

MainWindow::MainWindow(ContactRepository &repo)
    : repo_(repo)
{
    buildUi();
    buildMenu();
    buildToolbar();
    loadFromStorage();
}

void MainWindow::setDbStatus(bool online, const QString &message)
{
    dbOnline_ = online;
    dbMsg_ = message.trimmed();
    updateStatusLine(dbMsg_);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveToStorage();
    QMainWindow::closeEvent(event);
}

void MainWindow::buildUi()
{
    model_ = new ContactTableModel(this);
    proxy_ = new MultiFieldProxyModel(this);

    proxy_->setSourceModel(model_);
    proxy_->setFilterKeyColumn(-1);
    proxy_->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy_->setDynamicSortFilter(true);

    table_ = new QTableView(this);
    table_->setModel(proxy_);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setSortingEnabled(true);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    search_ = new QLineEdit(this);
    search_->setPlaceholderText("Поиск (имя / фамилия / email / телефон)");
    connect(search_, &QLineEdit::textChanged, this, [this](const QString &t)
            { applySearch(t); });

    auto *root = new QWidget(this);
    auto *layout = new QVBoxLayout(root);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);
    layout->addWidget(search_);
    layout->addWidget(table_);

    setCentralWidget(root);

    statusBar()->showMessage("Старт...");
    resize(1100, 650);
}

void MainWindow::buildMenu()
{
    QMenu *storage = menuBar()->addMenu("Хранилище");

    refreshAction_ = storage->addAction("Обновить");
    refreshAction_->setShortcut(QKeySequence::Refresh);

    QAction *load = storage->addAction("Загрузить");
    QAction *save = storage->addAction("Сохранить");

    connect(refreshAction_, &QAction::triggered, this, [this]
            { refreshNow(); });
    connect(load, &QAction::triggered, this, [this]
            { loadFromStorage(); });
    connect(save, &QAction::triggered, this, [this]
            { saveToStorage(); });

    QMenu *app = menuBar()->addMenu("Приложение");
    QAction *exitAction = app->addAction("Выход");
    connect(exitAction, &QAction::triggered, this, [this]
            { close(); });
}

void MainWindow::buildToolbar()
{
    auto *tb = addToolBar("Действия");
    tb->setMovable(false);

    if (!refreshAction_)
    {
        refreshAction_ = tb->addAction("Обновить");
        refreshAction_->setShortcut(QKeySequence::Refresh);
        connect(refreshAction_, &QAction::triggered, this, [this]
                { refreshNow(); });
    }
    else
    {
        tb->addAction(refreshAction_);
    }

    addAction_ = tb->addAction("Добавить");
    editAction_ = tb->addAction("Изменить");
    removeAction_ = tb->addAction("Удалить");

    connect(addAction_, &QAction::triggered, this, [this]
            { addContact(); });
    connect(editAction_, &QAction::triggered, this, [this]
            { editContact(); });
    connect(removeAction_, &QAction::triggered, this, [this]
            { removeContact(); });

    connect(table_, &QTableView::doubleClicked, this, [this]
            { editContact(); });
}

void MainWindow::refreshModel()
{
    model_->setContacts(contacts_);
}

void MainWindow::updateStatusLine(const QString &extra)
{
    const QString db = dbOnline_ ? "DB: online" : "DB: offline";
    const QString base = QString("Контактов: %1 | %2").arg(contacts_.size()).arg(db);

    const QString e = extra.trimmed();
    if (e.isEmpty())
        statusBar()->showMessage(base);
    else
        statusBar()->showMessage(base + " | " + e);
}

int MainWindow::selectedSourceRow() const
{
    if (!table_->selectionModel())
        return -1;

    const QModelIndexList selected = table_->selectionModel()->selectedRows();
    if (selected.isEmpty())
        return -1;

    const QModelIndex viewIndex = selected.front();
    const QModelIndex srcIndex = proxy_->mapToSource(viewIndex);
    if (!srcIndex.isValid())
        return -1;

    const int row = srcIndex.row();
    if (row < 0 || static_cast<std::size_t>(row) >= contacts_.size())
        return -1;

    return row;
}

void MainWindow::refreshNow()
{
    updateStatusLine("Обновляю...");
    loadFromStorage();
}

void MainWindow::addContact()
{
    ContactDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    contacts_.push_back(dlg.contact());

    refreshModel();
    saveToStorage();

    if (repo_.lastError().trimmed().isEmpty())
        updateStatusLine("Добавлен контакт");
}

void MainWindow::editContact()
{
    const int row = selectedSourceRow();
    if (row < 0)
        return;

    ContactDialog dlg(this);
    dlg.setContact(contacts_[static_cast<std::size_t>(row)]);

    if (dlg.exec() != QDialog::Accepted)
        return;

    contacts_[static_cast<std::size_t>(row)] = dlg.contact();

    refreshModel();
    saveToStorage();

    if (repo_.lastError().trimmed().isEmpty())
        updateStatusLine("Контакт изменён");
}

void MainWindow::removeContact()
{
    const int row = selectedSourceRow();
    if (row < 0)
        return;

    const auto r = QMessageBox::question(this, "Удаление", "Удалить выбранный контакт?");
    if (r != QMessageBox::Yes)
        return;

    contacts_.erase(contacts_.begin() + row);

    refreshModel();
    saveToStorage();

    if (repo_.lastError().trimmed().isEmpty())
        updateStatusLine("Контакт удалён");
}

void MainWindow::loadFromStorage()
{
    contacts_ = repo_.loadAll();
    refreshModel();

    const QString err = repo_.lastError().trimmed();
    if (!err.isEmpty())
    {
        updateStatusLine("Ошибка: " + err);
        return;
    }

    updateStatusLine(QString("Загружено (%1)").arg(contacts_.size()));
}

void MainWindow::saveToStorage()
{
    repo_.saveAll(contacts_);

    const QString err = repo_.lastError().trimmed();
    if (!err.isEmpty())
    {
        updateStatusLine("Ошибка: " + err);
        return;
    }

    updateStatusLine(QString("Сохранено (%1)").arg(contacts_.size()));
}

void MainWindow::applySearch(const QString &text)
{
    const QString t = text.trimmed();
    if (t.isEmpty())
    {
        proxy_->setFilterRegularExpression(QRegularExpression());
        return;
    }

    const QString escaped = QRegularExpression::escape(t);
    proxy_->setFilterRegularExpression(QRegularExpression(escaped, QRegularExpression::CaseInsensitiveOption));
}
