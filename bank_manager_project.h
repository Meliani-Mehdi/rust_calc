#ifndef BANK_MANAGER_PROJECT_H
#define BANK_MANAGER_PROJECT_H

#include "databasemanager.h"
#include <QMainWindow>
#include <QSqlTableModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class bank_manager_project;
}
QT_END_NAMESPACE

class bank_manager_project : public QMainWindow
{
    Q_OBJECT

public:
    bank_manager_project(QWidget *parent = nullptr);

    void setupClientTable();
    void setupAccountTable();
    void setupTransactionTable();

    void setupSearchAutoCompleteClients();
    void setupSearchAutoCompleteAcounts();
    void setupSearchAutoCompleteTrans();

    void showClientContextMenu(const QPoint &pos);
    void showAccountContextMenu(const QPoint &pos);
    void deleteClient(int row);
    void editClient(int row);
    void deleteAccount(int row);
    void setupButtons();

    ~bank_manager_project();

private:
    Ui::bank_manager_project *ui;
    DatabaseManager *db;
    QSqlTableModel *clientModel;
    QSqlTableModel *accountModel;
    QSqlTableModel *transactionModel;
    int used_client_id = -1;
    int used_account_id = -1;
};
#endif // BANK_MANAGER_PROJECT_H
