#include "contact_dialog.hpp"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QComboBox>

#include "validation.hpp"

ContactDialog::ContactDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Контакт");

    auto *root = new QVBoxLayout(this);

    auto *form = new QFormLayout();
    firstNameEdit_ = new QLineEdit(this);
    lastNameEdit_ = new QLineEdit(this);
    middleNameEdit_ = new QLineEdit(this);
    addressEdit_ = new QLineEdit(this);
    emailEdit_ = new QLineEdit(this);

    form->addRow("Имя", firstNameEdit_);
    form->addRow("Фамилия", lastNameEdit_);
    form->addRow("Отчество", middleNameEdit_);
    form->addRow("Адрес", addressEdit_);
    form->addRow("Email", emailEdit_);

    auto *birthRow = new QHBoxLayout();
    birthEnabled_ = new QCheckBox("Указана", this);
    birthEdit_ = new QDateEdit(this);
    birthEdit_->setCalendarPopup(true);
    birthEdit_->setDisplayFormat("dd.MM.yyyy");
    birthEdit_->setDate(QDate::currentDate().addYears(-18));
    birthEdit_->setMaximumDate(QDate::currentDate().addDays(-1));
    birthEdit_->setEnabled(false);

    birthRow->addWidget(birthEnabled_);
    birthRow->addWidget(birthEdit_);

    auto *birthWrap = new QWidget(this);
    birthWrap->setLayout(birthRow);
    form->addRow("Дата рождения", birthWrap);

    root->addLayout(form);

    auto *phonesBox = new QGroupBox("Телефоны", this);
    auto *phonesLayout = new QVBoxLayout(phonesBox);

    phonesTable_ = new QTableWidget(0, 2, this);
    phonesTable_->setHorizontalHeaderLabels(QStringList() << "Тип" << "Номер");
    phonesTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    phonesTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    phonesTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    phonesTable_->setSelectionMode(QAbstractItemView::SingleSelection);

    auto *btnRow = new QHBoxLayout();
    auto *addBtn = new QPushButton("Добавить", this);
    auto *rmBtn = new QPushButton("Удалить", this);
    btnRow->addWidget(addBtn);
    btnRow->addWidget(rmBtn);
    btnRow->addStretch();

    phonesLayout->addWidget(phonesTable_);
    phonesLayout->addLayout(btnRow);

    root->addWidget(phonesBox);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    root->addWidget(buttons);

    connect(birthEnabled_, &QCheckBox::toggled, this, [this](bool on)
            {
        birthEdit_->setEnabled(on);
        if (on)
            birthEdit_->setMaximumDate(QDate::currentDate().addDays(-1)); });

    connect(addBtn, &QPushButton::clicked, this, [this]
            { onAddPhone(); });
    connect(rmBtn, &QPushButton::clicked, this, [this]
            { onRemovePhone(); });

    connect(buttons, &QDialogButtonBox::accepted, this, [this]
            {
        if (validateAndBuild())
            accept(); });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    addPhoneRow(PhoneType::Home, QString());
}

void ContactDialog::setContact(const Contact &contact)
{
    contact_ = contact;

    firstNameEdit_->setText(contact.firstName());
    lastNameEdit_->setText(contact.lastName());
    middleNameEdit_->setText(contact.middleName());
    addressEdit_->setText(contact.address());
    emailEdit_->setText(contact.email());

    const bool hasBirth = contact.birthDate().isValid();
    birthEnabled_->setChecked(hasBirth);
    birthEdit_->setEnabled(hasBirth);
    if (hasBirth)
        birthEdit_->setDate(contact.birthDate());

    phonesTable_->setRowCount(0);
    for (const auto &p : contact.phoneNumbers())
        addPhoneRow(p.type(), p.value());

    if (phonesTable_->rowCount() == 0)
        addPhoneRow(PhoneType::Home, QString());
}

Contact ContactDialog::contact() const
{
    return contact_;
}

void ContactDialog::addPhoneRow(PhoneType type, const QString &value)
{
    const int row = phonesTable_->rowCount();
    phonesTable_->insertRow(row);

    auto *combo = new QComboBox(phonesTable_);
    combo->addItem("Рабочий");
    combo->addItem("Домашний");
    combo->addItem("Служебный");
    combo->setCurrentText(PhoneNumber::typeToLabel(type));

    auto *edit = new QLineEdit(phonesTable_);
    edit->setText(value);

    phonesTable_->setCellWidget(row, 0, combo);
    phonesTable_->setCellWidget(row, 1, edit);
}

std::vector<PhoneNumber> ContactDialog::readPhones() const
{
    std::vector<PhoneNumber> phones;

    const int rows = phonesTable_->rowCount();
    for (int r = 0; r < rows; ++r)
    {
        auto *combo = qobject_cast<QComboBox *>(phonesTable_->cellWidget(r, 0));
        auto *edit = qobject_cast<QLineEdit *>(phonesTable_->cellWidget(r, 1));
        if (!combo || !edit)
            continue;

        const QString number = normalizeEmail(trim(edit->text()));
        const QString label = combo->currentText();

        if (number.isEmpty())
            continue;

        phones.emplace_back(PhoneNumber::labelToType(label), number);
    }

    return phones;
}

void ContactDialog::onAddPhone()
{
    addPhoneRow(PhoneType::Home, QString());
}

void ContactDialog::onRemovePhone()
{
    const int row = phonesTable_->currentRow();
    if (row < 0)
        return;
    phonesTable_->removeRow(row);
}

bool ContactDialog::validateAndBuild()
{
    const QString first = trim(firstNameEdit_->text());
    const QString last = trim(lastNameEdit_->text());
    const QString middle = trim(middleNameEdit_->text());
    const QString addr = trim(addressEdit_->text());
    const QString email = normalizeEmail(trim(emailEdit_->text()));

    if (!isValidName(first))
    {
        QMessageBox::warning(this, "Ошибка", "Некорректное имя");
        return false;
    }
    if (!isValidName(last))
    {
        QMessageBox::warning(this, "Ошибка", "Некорректная фамилия");
        return false;
    }
    if (!middle.isEmpty() && !isValidName(middle))
    {
        QMessageBox::warning(this, "Ошибка", "Некорректное отчество");
        return false;
    }
    if (!isValidEmail(email))
    {
        QMessageBox::warning(this, "Ошибка", "Некорректный email");
        return false;
    }

    QDate birth;
    if (birthEnabled_->isChecked())
    {
        birth = birthEdit_->date();
        if (!isValidBirthDate(birth))
        {
            QMessageBox::warning(this, "Ошибка", "Некорректная дата рождения");
            return false;
        }
    }

    auto phones = readPhones();
    if (phones.empty())
    {
        QMessageBox::warning(this, "Ошибка", "Нужен хотя бы один номер телефона");
        return false;
    }

    for (const auto &p : phones)
    {
        if (!isValidPhoneNumber(p.value()))
        {
            QMessageBox::warning(this, "Ошибка", "Некорректный номер телефона: " + p.value());
            return false;
        }
    }

    Contact c;
    c.setFirstName(first);
    c.setLastName(last);
    c.setMiddleName(middle);
    c.setAddress(addr);
    c.setEmail(email);
    c.setBirthDate(birth);
    c.setPhoneNumbers(std::move(phones));

    contact_ = std::move(c);
    return true;
}
