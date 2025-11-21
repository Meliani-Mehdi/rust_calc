#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QVector>
#include <QMap>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(const QString &path, QObject *parent = nullptr);

    bool isOpen() const;

    // ---------- CLIENT ----------
    bool addClient(const QString &name, const QString &email, const QString &phone, const QString &date_of_birth);
    QMap<QString, QVariant> getClientById(int id);
    QVector<QMap<QString, QVariant>> getAllClients() const;
    bool updateClient(int id, const QString &name, const QString &email, const QString &phone, const QString &date_of_birth);
    bool deleteClient(int id);

    // ---------- ACCOUNT ----------
    bool addAccount(int clientId);
    QMap<QString, QVariant> getAccountById(int id);
    QVector<QMap<QString, QVariant>> getAllAccounts() const;
    QVector<QMap<QString, QVariant>> getAccountsByClient(int clientId);
    bool updateAccount(int id, int clientId, double balance, const QString &date_of_birth);
    bool deleteAccount(int id);

    // ---------- TRANSACTION ----------
    bool addTransaction(int accountId, double amount, const QString &type);
    QVector<QMap<QString, QVariant>> getAllTransactions() const;
    bool updateTransaction(int id, int accountId, double amount, const QString &type);
    bool deleteTransaction(int id);

private:
    QSqlDatabase m_db;
    void initializeTables();
};

#endif // DATABASEMANAGER_H
