#include "transactiondialog.h"
#include "ui_transactiondialog.h"

transactiondialog::transactiondialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::transactiondialog)
{
    ui->setupUi(this);

    QRegularExpression amountRegex("^[0-9]+$");
    ui->amount->setValidator(new QRegularExpressionValidator(amountRegex, this));
}

void transactiondialog::set_account_name(QString name){
    ui->account_name_lab->setText(name);
}

void transactiondialog::set_balance(QString balnce){
    ui->balnce_lab->setText(balnce);
}

QString transactiondialog::get_amount(){
    return ui->amount->text();
}

QString transactiondialog::get_type(){

    return ui->type->currentText();
}

transactiondialog::~transactiondialog()
{
    delete ui;
}
