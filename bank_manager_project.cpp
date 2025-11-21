#include "bank_manager_project.h"
#include "./ui_bank_manager_project.h"
#include "clientdialog.h"
#include "accountdialog.h"
#include "transactiondialog.h"

#include <QMessageBox>
#include <QMenu>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <qdatetime.h>
#include <qsqlerror.h>
#include <QCompleter>
#include <QStandardItemModel>

bank_manager_project::bank_manager_project(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::bank_manager_project)
{
    ui->setupUi(this);

    db = new DatabaseManager("bank.sqlite", this);
    if (!db->isOpen()) {
        QMessageBox::critical(this, "Database Error", "Could not open database!");
        return;
    }

    setupClientTable();
    setupAccountTable();
    setupTransactionTable();

    setupSearchAutoCompleteClients();
    setupButtons();

}

void bank_manager_project::setupSearchAutoCompleteClients(){
    QStringList clientIds;
    for (const auto &client : db->getAllClients()) {
        // You can show ID or ID+name
        clientIds << QString::number(client["id"].toInt());
        // or, for richer autocomplete:
        // clientIds << QString("%1 - %2").arg(client["id"].toInt()).arg(client["name"].toString());
    }

    QCompleter *clientCompleter = new QCompleter(clientIds, this);
    clientCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->client_set_id->setCompleter(clientCompleter);

}

void bank_manager_project::setupSearchAutoCompleteAcounts(){
    QStringList acountIds;
    for (const auto &acc : db->getAllAccounts()) {
        // You can show ID or ID+name
        acountIds << QString::number(acc["id"].toInt());
        // or, for richer autocomplete:
        // clientIds << QString("%1 - %2").arg(client["id"].toInt()).arg(client["name"].toString());
    }

    QCompleter *accountCompleter = new QCompleter(acountIds, this);
    accountCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->account_set_id->setCompleter(accountCompleter);

}

void bank_manager_project::setupSearchAutoCompleteTrans(){
    QStringList clientIds;
    for (const auto &client : db->getAllClients()) {
        // You can show ID or ID+name
        clientIds << QString::number(client["id"].toInt());
        // or, for richer autocomplete:
        // clientIds << QString("%1 - %2").arg(client["id"].toInt()).arg(client["name"].toString());
    }

    QCompleter *clientCompleter = new QCompleter(clientIds, this);
    clientCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->client_set_id->setCompleter(clientCompleter);

}

void bank_manager_project::showClientContextMenu(const QPoint &pos) {
    QModelIndex index = ui->clientTable->indexAt(pos);
    if (!index.isValid())
        return;

    QMenu contextMenu(this);

    QAction *editAction = contextMenu.addAction("Edit");
    QAction *deleteAction = contextMenu.addAction("Delete");

    QAction *selectedAction = contextMenu.exec(ui->clientTable->viewport()->mapToGlobal(pos));
    if (selectedAction == editAction) {
        editClient(index.row());
    } else if (selectedAction == deleteAction) {
        deleteClient(index.row());
    }
}

void bank_manager_project::editClient(int row) {
    if (row < 0) return;

    QSqlRecord data = clientModel->record(row);
    int id = data.value("id").toInt();

    clientdialog dlg(this);
    QString name = data.value("name").toString();
    QString email = data.value("email").toString();
    QString phone = data.value("phone").toString();
    QString dob = data.value("date_of_birth").toString();
    dlg.setClientData(name, email, phone, dob);

    if (dlg.exec() == QDialog::Accepted) {
        if (!db->updateClient(id, name, email, phone, dob)) {
            QMessageBox::warning(this, "Error", "Failed to update client");
        } else {
            clientModel->select();
        }
    }
}

void bank_manager_project::deleteClient(int row) {
    if (row < 0) return;

    int id = clientModel->record(row).value("id").toInt();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Client", "Are you sure you want to delete this client?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {

        if (!db->deleteClient(id)) {
            QMessageBox::warning(this, "Error", "Failed to delete client");
        } else {
            clientModel->select();
        }
    }
}

void bank_manager_project::showAccountContextMenu(const QPoint &pos) {
    QModelIndex index = ui->accountTable->indexAt(pos);
    if (!index.isValid())
        return;

    QMenu contextMenu(this);

    QAction *deleteAction = contextMenu.addAction("Delete");

    QAction *selectedAction = contextMenu.exec(ui->accountTable->viewport()->mapToGlobal(pos));
    if (selectedAction == deleteAction) {
        deleteAccount(index.row());
    }
}

void bank_manager_project::deleteAccount(int row) {
    if (row < 0) return;

    int id = accountModel->record(row).value("id").toInt();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete Account", "Are you sure you want to delete this account?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {

        if (!db->deleteAccount(id)) {
            QMessageBox::warning(this, "Error", "Failed to delete account");
        } else {
            clientModel->select();
        }
    }}

void bank_manager_project::setupClientTable(){
    ui->clientTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->clientTable, &QTableView::customContextMenuRequested,
            this, &bank_manager_project::showClientContextMenu);


    clientModel = new QSqlTableModel(this);
    clientModel->setTable("Client");
    clientModel->select();

    clientModel->setHeaderData(0, Qt::Horizontal, "id");
    clientModel->setHeaderData(1, Qt::Horizontal, "Name");
    clientModel->setHeaderData(2, Qt::Horizontal, "Email");
    clientModel->setHeaderData(3, Qt::Horizontal, "Phone");
    clientModel->setHeaderData(4, Qt::Horizontal, "Date of Birth");

    ui->clientTable->setModel(clientModel);
    ui->clientTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->clientTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->clientTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->clientTable->horizontalHeader()->setStretchLastSection(true);

    connect(ui->clientSearchBar, &QLineEdit::textChanged, this, [=](const QString &text){
        QString filter = QString("name LIKE '%%1%' OR email LIKE '%%1%' OR phone LIKE '%%1%'").arg(text);
        clientModel->setFilter(filter);
        clientModel->select();
    });
}

void bank_manager_project::setupAccountTable() {
    ui->accountTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->accountTable, &QTableView::customContextMenuRequested,
            this, &bank_manager_project::showAccountContextMenu);

    accountModel = new QSqlTableModel(this);
    accountModel->setTable("Account");

    accountModel->setFilter("1=0");
    accountModel->select();

    accountModel->setHeaderData(0, Qt::Horizontal, "ID");
    accountModel->setHeaderData(1, Qt::Horizontal, "Client ID");
    accountModel->setHeaderData(2, Qt::Horizontal, "Balance");
    accountModel->setHeaderData(3, Qt::Horizontal, "Creation Date");

    ui->accountTable->setModel(accountModel);
    ui->accountTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->accountTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->accountTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->accountTable->horizontalHeader()->setStretchLastSection(true);
}

void bank_manager_project::setupTransactionTable(){
    transactionModel = new QSqlTableModel(this);
    transactionModel->setTable("Transaction_tb");

    transactionModel->setFilter("1=0");
    transactionModel->select();

    transactionModel->setHeaderData(0, Qt::Horizontal, "ID");
    transactionModel->setHeaderData(1, Qt::Horizontal, "Account ID");
    transactionModel->setHeaderData(2, Qt::Horizontal, "Date");
    transactionModel->setHeaderData(3, Qt::Horizontal, "Amount");
    transactionModel->setHeaderData(4, Qt::Horizontal, "Type");

    ui->transactionTable->setModel(transactionModel);
    ui->transactionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->transactionTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->transactionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->transactionTable->horizontalHeader()->setStretchLastSection(true);
}


void bank_manager_project::setupButtons(){
    connect(ui->clientButton, &QPushButton::clicked, this, [=](){
        clientdialog dlg(this);
        if (dlg.exec() == QDialog::Accepted) {
            if (!db->addClient(dlg.getName(), dlg.getEmail(), dlg.getPhone(), dlg.getDob())) {
                QMessageBox::warning(this, "Error", "Failed to insert client");
            } else {
                clientModel->select();
            }
        }
    });

    connect(ui->account_button, &QPushButton::clicked, this, [=](){
        accountdialog dlg(this);
        if (used_client_id == -1) return;
        const auto client = db->getClientById(used_client_id);
        dlg.set_client(client.value("name").toString());

        if (dlg.exec() == QDialog::Accepted) {
            if (!db->addAccount(used_client_id)) {
                QMessageBox::warning(this, "Error", "Failed to add account");
            } else {
                accountModel->select();
            }
        }
    });

    connect(ui->transactionButton, &QPushButton::clicked, this, [=](){
        transactiondialog dlg(this);
        if (used_account_id == -1) return;
        const auto account = db->getAccountById(used_account_id);
        dlg.set_balance(account.value("balance").toString());

        const auto client = db->getClientById(account.value("client_id").toInt());
        dlg.set_account_name(client.value("name").toString());

        if (dlg.exec() == QDialog::Accepted) {
            if (!db->addTransaction(used_account_id, dlg.get_amount().toDouble(), dlg.get_type())) {
                QMessageBox::warning(this, "Error", "Failed to add transaction ");
            } else {
                transactionModel->select();
            }
        }
    });

    connect(ui->client_set_btn, &QPushButton::clicked, this, [this]() {
        bool ok;
        int clientId = ui->client_set_id->text().toInt(&ok);
        if (!ok) {
            used_client_id = -1;
            ui->cl_ac_name->setText("Client Name :");
            ui->cal_ac_dob->setText("Client date of birth :");
            ui->cl_ac_phone->setText("Phone :");
            ui->cl_ac_mail->setText("Email :");

            accountModel->setFilter("1=0");
            accountModel->select();

            QMessageBox::warning(this, "Invalid ID", "Please enter a valid client ID.");

            return;
        }

        used_client_id = clientId;
        // Apply filter directly on the existing model

        QString filter = QString("client_id = %1").arg(clientId);

        const auto client = db->getClientById(clientId);
        ui->cl_ac_name->setText(QString("Client Name : %1").arg(client.value("name").toString()));
        ui->cal_ac_dob->setText(QString("Client date of birth : %1").arg(client.value("date_of_birth").toString()));
        ui->cl_ac_phone->setText(QString("Phone : %1").arg(client.value("phone").toString()));
        ui->cl_ac_mail->setText(QString("Email : %1").arg(client.value("email").toString()));


        accountModel->setFilter(filter);
        accountModel->select();
    });

    connect(ui->account_set_btn, &QPushButton::clicked, this, [this]() {
        bool ok;
        int accountId = ui->account_set_id->text().toInt(&ok);
        if (!ok) {
            used_account_id = -1;
            ui->cl_tran_name->setText("Client Name :");
            ui->cl_tran_dob->setText("Client date of birth :");
            ui->cl_tran_phone->setText("Phone :");
            ui->cl_tran_mail->setText("Email :");
            ui->cl_tran_balance->setText("Balance :");

            transactionModel->setFilter("1=0");
            transactionModel->select();
            QMessageBox::warning(this, "Invalid ID", "Please enter a valid Account ID.");

            return;
        }

        used_account_id = accountId;
        // Apply filter directly on the existing model

        QString filter = QString("account_id = %1").arg(accountId);

        const auto account = db->getAccountById(accountId);
        const auto client = db->getClientById(account.value("client_id").toInt());
        ui->cl_tran_name->setText(QString("Client Name : %1").arg(client.value("name").toString()));
        ui->cl_tran_dob->setText(QString("Client date of birth : %1").arg(client.value("date_of_birth").toString()));
        ui->cl_tran_phone->setText(QString("Phone : %1").arg(client.value("phone").toString()));
        ui->cl_tran_mail->setText(QString("Email : %1").arg(client.value("email").toString()));
        ui->cl_tran_balance->setText(QString("Balance : %1").arg(account.value("balance").toString()));


        transactionModel->setFilter(filter);
        transactionModel->select();
    });


}

bank_manager_project::~bank_manager_project()
{
    delete ui;
    delete db;
}
