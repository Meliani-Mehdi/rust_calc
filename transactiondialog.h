#ifndef TRANSACTIONDIALOG_H
#define TRANSACTIONDIALOG_H

#include <QDialog>

namespace Ui {
class transactiondialog;
}

class transactiondialog : public QDialog
{
    Q_OBJECT

public:
    explicit transactiondialog(QWidget *parent = nullptr);
    void set_balance(QString balance);
    void set_account_name(QString name);
    QString get_amount();
    QString get_type();
    ~transactiondialog();

private:
    Ui::transactiondialog *ui;
};

#endif // TRANSACTIONDIALOG_H
