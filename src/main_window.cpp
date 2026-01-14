#include "main_window.hpp"

#include <QAction>
#include <QMessageBox>
#include <QRegularExpression>
#include <QToolBar>
#include <QStatusBar>

#include "contact_dialog.hpp"

MainWindow::MainWindow(ContactRepository &repository, QWidget *parent)
    : QMainWindow(parent), repository_(repository)
{
    setWindowTitle("Телефонный справочник");
    resize(1100, 650);

    table_ = new QTableView(this);
    setCentralWidget(table_);

    model_.setContacts(&contacts_);
    proxy_.setSourceModel(&model_);
    table_->setModel(&proxy_);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSortingEnabled(true);

    auto *bar = addToolBar("main");
    auto *addAct = new QAction("Добавить", this);
    auto *editAct = new QAction("Изменить", this);
    auto *delAct = new QAction("Удалить", this);

    search_ = new QLineEdit(this);
    search_->setPlaceholderText("Поиск");

    bar->addAction(addAct);
    bar->addAction(editAct);
    bar->addAction(delAct);
    bar->addSeparator();
    bar->addWidget(search_);

    connect(addAct, &QAction::triggered, this, [this]
            { addContact(); });
    connect(editAct, &QAction::triggered, this, [this]
            { editContact(); });
    connect(delAct, &QAction::triggered, this, [this]
            { removeContact(); });

    connect(search_, &QLineEdit::textChanged, this, [this](const QString &text)
            {
        const QString t = text.trimmed();
        if (t.isEmpty())
        {
            proxy_.setFilterRegularExpression(QRegularExpression());
            return;
        }
        proxy_.setFilterRegularExpression(QRegularExpression(QRegularExpression::escape(t), QRegularExpression::CaseInsensitiveOption)); });

    connect(table_, &QTableView::doubleClicked, this, [this]
            { editContact(); });

    load();
}

void MainWindow::load()
{
    contacts_ = repository_.loadAll();
    model_.setContacts(&contacts_);
    statusBar()->showMessage(QString("Контактов: %1").arg(contacts_.size()));
}

void MainWindow::save()
{
    repository_.saveAll(contacts_);
    statusBar()->showMessage(QString("Сохранено. Контактов: %1").arg(contacts_.size()));
}

int MainWindow::selectedSourceRow() const
{
    const QModelIndexList selected = table_->selectionModel()->selectedRows();
    if (selected.isEmpty())
        return -1;

    const QModelIndex proxyIndex = selected.first();
    const QModelIndex sourceIndex = proxy_.mapToSource(proxyIndex);
    return sourceIndex.row();
}

void MainWindow::addContact()
{
    ContactDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    model_.addContact(dlg.contact());
    save();
}

void MainWindow::editContact()
{
    const int row = selectedSourceRow();
    if (row < 0)
        return;

    const Contact *c = model_.contactAt(row);
    if (!c)
        return;

    ContactDialog dlg(this);
    dlg.setContact(*c);

    if (dlg.exec() != QDialog::Accepted)
        return;

    model_.updateContact(row, dlg.contact());
    save();
}

void MainWindow::removeContact()
{
    const int row = selectedSourceRow();
    if (row < 0)
        return;

    const auto btn = QMessageBox::question(this, "Удаление", "Удалить выбранный контакт?");
    if (btn != QMessageBox::Yes)
        return;

    model_.removeContact(row);
    save();
}
