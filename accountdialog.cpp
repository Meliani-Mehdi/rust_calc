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

bool accountdialog::get_tpe() {
    return ui->tpeCheck->isChecked();
}

accountdialog::~accountdialog()
{
    delete ui;
}
