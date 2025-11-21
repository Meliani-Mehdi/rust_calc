#include "accountdialog.h"
#include "ui_accountdialog.h"

accountdialog::accountdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::accountdialog)
{
    ui->setupUi(this);
}

void accountdialog::set_client(QString name) {
    ui->client_info->setText(name);
}

accountdialog::~accountdialog()
{
    delete ui;
}
