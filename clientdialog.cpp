#include "clientdialog.h"
#include "ui_clientdialog.h"

clientdialog::clientdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::clientdialog)
{
    ui->setupUi(this);

    // Name: only letters, spaces, and hyphens
    QRegularExpression nameRegex("^[A-Za-z\\s'-]+$");
    ui->nameEdit->setValidator(new QRegularExpressionValidator(nameRegex, this));

    // Phone: only digits, allow + at start, 7–15 digits
    QRegularExpression phoneRegex("^\\+?[0-9]{7,15}$");
    ui->phoneEdit->setValidator(new QRegularExpressionValidator(phoneRegex, this));

    // Email: simple pattern (not 100% RFC but works for most)
    QRegularExpression emailRegex("^[\\w\\.]+@[\\w\\.]+\\.[a-zA-Z]{2,}$");
    ui->emailEdit->setValidator(new QRegularExpressionValidator(emailRegex, this));
}

void clientdialog::setClientData(const QString &name, const QString &email, const QString &phone, const QString &dob)
{
    ui->nameEdit->setText(name);
    ui->emailEdit->setText(email);
    ui->phoneEdit->setText(phone);

    QDate date = QDate::fromString(dob, "yyyy-MM-dd");
    if (date.isValid())
        ui->dobEdit->setDate(date);
    else
        ui->dobEdit->setDate(QDate::currentDate());
}

QString clientdialog::getName() const
{
    return ui->nameEdit->text();
}

QString clientdialog::getEmail() const
{
    return ui->emailEdit->text();
}

QString clientdialog::getPhone() const
{
    return ui->phoneEdit->text();
}

QString clientdialog::getDob() const
{
    return ui->dobEdit->date().toString("yyyy-MM-dd");
}

clientdialog::~clientdialog()
{
    delete ui;
}
