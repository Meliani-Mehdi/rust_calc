#ifndef ACCOUNTDIALOG_H
#define ACCOUNTDIALOG_H

#include <QDialog>

namespace Ui {
class accountdialog;
}

class accountdialog : public QDialog
{
    Q_OBJECT

public:
    explicit accountdialog(QWidget *parent = nullptr);
    ~accountdialog();
    void set_client(QString name);

private:
    Ui::accountdialog *ui;
};

#endif // ACCOUNTDIALOG_H
