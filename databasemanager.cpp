#include "databasemanager.h"
#include <QSqlError>
#include <QDebug>

DatabaseManager::DatabaseManager(const QString &path, QObject *parent)
    : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open()) {
        qDebug() << "Error: Could not open database -" << m_db.lastError().text();
    } else {
        initializeTables();
    }
}

bool DatabaseManager::isOpen() const {
    return m_db.isOpen();
}

void DatabaseManager::initializeTables() {
    QSqlQuery query;

    query.exec("CREATE TABLE IF NOT EXISTS Client ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "name TEXT NOT NULL,"
               "email TEXT,"
               "phone TEXT,"
               "date_of_birth TEXT)");

    query.exec("CREATE TABLE IF NOT EXISTS Account ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "client_id INTEGER,"
               "balance REAL DEFAULT 0.0,"
               "creation_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
               "FOREIGN KEY(client_id) REFERENCES Client(id))");

    query.exec("CREATE TABLE IF NOT EXISTS Transaction_tb ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "account_id INTEGER,"
               "date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
               "amount REAL,"
               "type TEXT,"
               "FOREIGN KEY(account_id) REFERENCES Account(id))");
}

// ---------- CLIENT ----------
bool DatabaseManager::addClient(const QString &name, const QString &email, const QString &phone, const QString &date_of_birth) {
    QSqlQuery query;
    query.prepare("INSERT INTO Client (name, email, phone, date_of_birth) VALUES (?, ?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(email);
    query.addBindValue(phone);
    query.addBindValue(date_of_birth);
    return query.exec();
}

QMap<QString, QVariant> DatabaseManager::getClientById(int id) {
    QSqlQuery query;
    query.prepare("SELECT * FROM Client WHERE id = ?");
    query.addBindValue(id);

    QMap<QString, QVariant> row;

    if (!query.exec()) {
        qDebug() << "Database error:" << query.lastError().text();
        return row;
    }

    if (query.next()) {
        row["id"] = query.value("id");
        row["name"] = query.value("name");
        row["email"] = query.value("email");
        row["phone"] = query.value("phone");
        row["date_of_birth"] = query.value("date_of_birth");
    }

    return row;
}

QVector<QMap<QString, QVariant>> DatabaseManager::getAllClients() const {
    QVector<QMap<QString, QVariant>> results;
    QSqlQuery query("SELECT * FROM Client");
    while (query.next()) {
        QMap<QString, QVariant> row;
        row["id"] = query.value("id");
        row["client_id"] = query.value("client_id");
        row["email"] = query.value("email");
        row["phone"] = query.value("phone");
        row["date_of_birth"] = query.value("date_of_birth");
        results.append(row);
    }
    return results;
}

bool DatabaseManager::updateClient(int id, const QString &name, const QString &email, const QString &phone, const QString &date_of_birth) {
    QSqlQuery query;
    query.prepare("UPDATE Client SET name = ?, email = ?, phone = ?, date_of_birth = ? WHERE id = ?");
    query.addBindValue(name);
    query.addBindValue(email);
    query.addBindValue(phone);
    query.addBindValue(date_of_birth);
    query.addBindValue(id);
    return query.exec();
}

bool DatabaseManager::deleteClient(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM Client WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

// ---------- ACCOUNT ----------
bool DatabaseManager::addAccount(int clientId) {
    QSqlQuery query;
    query.prepare("INSERT INTO Account (client_id) VALUES (?)");
    query.addBindValue(clientId);
    return query.exec();
}

QMap<QString, QVariant> DatabaseManager::getAccountById(int id) {
    QSqlQuery query;
    query.prepare("SELECT * FROM Account WHERE id = ?");
    query.addBindValue(id);

    QMap<QString, QVariant> row;

    if (!query.exec()) {
        qDebug() << "Database error:" << query.lastError().text();
        return row;
    }

    if (query.next()) {
        row["id"] = query.value("id");
        row["client_id"] = query.value("client_id");
        row["balance"] = query.value("balance");
        row["creation_date"] = query.value("creation_date");
    }

    return row;
}

QVector<QMap<QString, QVariant>> DatabaseManager::getAllAccounts() const {
    QVector<QMap<QString, QVariant>> results;
    QSqlQuery query("SELECT * FROM Account");
    while (query.next()) {
        QMap<QString, QVariant> row;
        row["id"] = query.value("id");
        row["client_id"] = query.value("client_id");
        row["balance"] = query.value("balance");
        row["creation_date"] = query.value("creation_date");
        results.append(row);
    }
    return results;
}

QVector<QMap<QString, QVariant>> DatabaseManager::getAccountsByClient(int clientId)  {
    QVector<QMap<QString, QVariant>> results;
    QSqlQuery query;
    query.prepare("SELECT * FROM Account WHERE client_id = ?");
    query.addBindValue(clientId);
    if (query.exec()) {
        while (query.next()) {
            QMap<QString, QVariant> row;
            row["id"] = query.value("id");
            row["client_id"] = query.value("client_id");
            row["balance"] = query.value("balance");
            row["creation_date"] = query.value("creation_date");
            results.append(row);
        }
    }
    return results;
}


bool DatabaseManager::updateAccount(int id, int clientId, double balance, const QString &date_of_birth) {
    QSqlQuery query;
    query.prepare("UPDATE Account SET client_id = ?, balance = ?, creation_date = ? WHERE id = ?");
    query.addBindValue(clientId);
    query.addBindValue(balance);
    query.addBindValue(date_of_birth);
    query.addBindValue(id);
    return query.exec();
}

bool DatabaseManager::deleteAccount(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM Account WHERE id = ?");
    query.addBindValue(id);
    return query.exec();
}

// ---------- TRANSACTION ----------
bool DatabaseManager::addTransaction(int accountId, double amount, const QString &type) {
    if (type != "Deposit" && type != "Withdraw") {
        qDebug() << "Error: Transaction type must be Deposit or Withdraw";
        return false;
    }

    QSqlQuery query;
    if (!m_db.transaction()) { // Start transaction
        qDebug() << "Failed to start DB transaction:" << m_db.lastError().text();
        return false;
    }

    // 1. Insert into Transaction table
    query.prepare("INSERT INTO Transaction_tb (account_id, amount, type) VALUES (?, ?, ?)");
    query.addBindValue(accountId);
    query.addBindValue(amount);
    query.addBindValue(type);

    if (!query.exec()) {
        qDebug() << "Failed to insert transaction:" << query.lastError().text();
        m_db.rollback();
        return false;
    }

    // 2. Update account balance
    QString balanceUpdate;
    if (type == "Deposit") {
        balanceUpdate = "UPDATE Account SET balance = balance + ? WHERE id = ?";
    } else if (type == "Withdraw") {
        balanceUpdate = "UPDATE Account SET balance = balance - ? WHERE id = ?";
    }

    query.prepare(balanceUpdate);
    query.addBindValue(amount);
    query.addBindValue(accountId);

    if (!query.exec()) {
        qDebug() << "Failed to update account balance:" << query.lastError().text();
        m_db.rollback();
        return false;
    }

    // 3. Commit transaction
    if (!m_db.commit()) {
        qDebug() << "Failed to commit DB transaction:" << m_db.lastError().text();
        m_db.rollback();
        return false;
    }

    return true;
}


QVector<QMap<QString, QVariant>> DatabaseManager::getAllTransactions() const {
    QVector<QMap<QString, QVariant>> results;
    QSqlQuery query("SELECT * FROM Transaction_tb");
    while (query.next()) {
        QMap<QString, QVariant> row;
        row["id"] = query.value("id");
        row["account_id"] = query.value("account_id");
        row["date"] = query.value("date");
        row["amount"] = query.value("amount");
        row["type"] = query.value("type");
        results.append(row);
    }
    return results;
}

bool DatabaseManager::updateTransaction(int id, int accountId, double amount, const QString &type) {
    if (type != "ADD" && type != "SUB") {
        qDebug() << "Error: Transaction type must be ADD or SUB";
        return false;
    }

    if (!m_db.transaction()) {
        qDebug() << "Failed to start DB transaction:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query;

    // 1. Get old transaction
    query.prepare("SELECT account_id, amount, type FROM Transaction_tb WHERE id = ?");
    query.addBindValue(id);
    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to fetch old transaction:" << query.lastError().text();
        m_db.rollback();
        return false;
    }

    int oldAccountId = query.value("account_id").toInt();
    double oldAmount = query.value("amount").toDouble();
    QString oldType = query.value("type").toString();

    // 2. Undo old transaction
    QString undoSql;
    if (oldType == "ADD") {
        undoSql = "UPDATE Account SET balance = balance - ? WHERE id = ?";
    } else if (oldType == "SUB") {
        undoSql = "UPDATE Account SET balance = balance + ? WHERE id = ?";
    }
    query.prepare(undoSql);
    query.addBindValue(oldAmount);
    query.addBindValue(oldAccountId);
    if (!query.exec()) {
        qDebug() << "Failed to undo old transaction:" << query.lastError().text();
        m_db.rollback();
        return false;
    }

    // 3. Update transaction row
    query.prepare("UPDATE Transaction_tb SET account_id = ?, date = ?, amount = ?, type = ? WHERE id = ?");
    query.addBindValue(accountId);
    query.addBindValue(amount);
    query.addBindValue(type);
    query.addBindValue(id);
    if (!query.exec()) {
        qDebug() << "Failed to update transaction row:" << query.lastError().text();
        m_db.rollback();
        return false;
    }

    // 4. Apply new transaction
    QString applySql;
    if (type == "ADD") {
        applySql = "UPDATE Account SET balance = balance + ? WHERE id = ?";
    } else if (type == "SUB") {
        applySql = "UPDATE Account SET balance = balance - ? WHERE id = ?";
    }
    query.prepare(applySql);
    query.addBindValue(amount);
    query.addBindValue(accountId);
    if (!query.exec()) {
        qDebug() << "Failed to apply new transaction:" << query.lastError().text();
        m_db.rollback();
        return false;
    }

    // 5. Commit
    if (!m_db.commit()) {
        qDebug() << "Failed to commit update transaction:" << m_db.lastError().text();
        m_db.rollback();
        return false;
    }

    return true;
}

bool DatabaseManager::deleteTransaction(int id) {
    if (!m_db.transaction()) {
        qDebug() << "Failed to start DB transaction:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query;

    // 1. Get old transaction
    query.prepare("SELECT account_id, amount, type FROM Transaction_tb WHERE id = ?");
    query.addBindValue(id);
    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to fetch transaction for deletion:" << query.lastError().text();
        m_db.rollback();
        return false;
    }

    int accountId = query.value("account_id").toInt();
    double amount = query.value("amount").toDouble();
    QString type = query.value("type").toString();

    // 2. Undo its effect
    QString undoSql;
    if (type == "ADD") {
        undoSql = "UPDATE Account SET balance = balance - ? WHERE id = ?";
    } else if (type == "SUB") {
        undoSql = "UPDATE Account SET balance = balance + ? WHERE id = ?";
    }
    query.prepare(undoSql);
    query.addBindValue(amount);
    query.addBindValue(accountId);
    if (!query.exec()) {
        qDebug() << "Failed to undo balance before delete:" << query.lastError().text();
        m_db.rollback();
        return false;
    }

    // 3. Delete transaction row
    query.prepare("DELETE FROM Transaction_tb WHERE id = ?");
    query.addBindValue(id);
    if (!query.exec()) {
        qDebug() << "Failed to delete transaction:" << query.lastError().text();
        m_db.rollback();
        return false;
    }

    // 4. Commit
    if (!m_db.commit()) {
        qDebug() << "Failed to commit delete transaction:" << m_db.lastError().text();
        m_db.rollback();
        return false;
    }

    return true;
}

