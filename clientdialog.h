#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QDialog>

namespace Ui {
class clientdialog;
}

class clientdialog : public QDialog
{
    Q_OBJECT

public:
    explicit clientdialog(QWidget *parent = nullptr);
    ~clientdialog();

    void setClientData(const QString &name, const QString &email, const QString &phone, const QString &dob);
    QString getName() const;
    QString getEmail() const;
    QString getPhone() const;
    QString getDob() const;

private:
    Ui::clientdialog *ui;
};

#endif // CLIENTDIALOG_H
