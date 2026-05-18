#include "databasemanager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <random>
#include <QCryptographicHash>

DatabaseManager::DatabaseManager() {}

bool DatabaseManager::openDatabase(const QString &path, const QString &password) {
    // 1. Инициализируем драйвер SQLite
    main = QSqlDatabase::addDatabase("SQLITECIPHER");
    main.setDatabaseName(path);

    // 2. Пытаемся открыть файл
    if (!main.open()) {
        qDebug() << "Ошибка открытия файла:" << main.lastError().text();
        return false;
    }

    // 3. Устанавливаем ключ шифрования (SQLCipher)
    // Мы создаем объект запроса и выполняем PRAGMA key
    QSqlQuery query;
    if (!query.exec("PRAGMA key = '" + password + "';")) {
        qDebug() << "Ошибка установки ключа:" << query.lastError().text();
        return false;
    }

    // 4. Создаем таблицу, если её еще нет
    // Это сработает только если ключ подошел и база расшифровалась
    QString createTable = "CREATE TABLE IF NOT EXISTS passwords ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "title TEXT, "
                          "login TEXT, "
                          "password TEXT, "
                          "url TEXT, "
                          "notes TEXT,"
                          "category_id INTEGER)";
    QString createCatTable = "CREATE TABLE IF NOT EXISTS categories ("
                             "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                             "name TEXT UNIQUE)";

    if (!query.exec(createTable)) {
        qDebug() << "Ошибка создания таблицы (возможно, неверный пароль):" << query.lastError().text();
        return false;
    }
    if (!query.exec(createCatTable)) {
        qDebug() << "Ошибка создания таблицы категорий" << query.lastError().text();
        return false;
    }
    // Попробуем прочитать что-то из таблицы, чтобы проверить пароль
    if (!query.exec("SELECT count(*) FROM sqlite_master")) {
        qDebug() << "Ошибка: Скорее всего, введен неверный пароль!";
        main.close();
        return false;
    }
    if (!query.exec("INSERT OR IGNORE INTO categories (name) VALUES ('Общее')")) {
        qDebug() << "Ошибка вставки базовой категории" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::addEntry(const DatabaseManager::PasswordEntry &entry) {
    QSqlQuery query;

    query.prepare("INSERT INTO passwords (title, login, password, url, notes, category_id) "
                  "VALUES (:title, :login, :password, :url, :notes, :category_id)");
    query.bindValue(":title", entry.title);
    query.bindValue(":login", entry.login);
    query.bindValue(":password", entry.password);
    query.bindValue(":url", entry.url);
    query.bindValue(":notes", entry.notes);
    query.bindValue(":category_id", entry.category_id);

    return query.exec();
}

QList<DatabaseManager::PasswordEntry> DatabaseManager::getAllEntries() {
    QList<DatabaseManager::PasswordEntry> list;
    QSqlQuery query;

    QString sql = "SELECT p.*, c.name AS category_name "
                  "FROM passwords p "
                  "LEFT JOIN categories c ON p.category_id = c.id";

    if (!query.exec(sql)) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        return list;
    }

    while(query.next()) {
        DatabaseManager::PasswordEntry entry;
        entry.id = query.value("id").toInt();
        entry.password = query.value("password").toString();
        entry.login = query.value("login").toString();
        entry.url = query.value("url").toString();
        entry.notes = query.value("notes").toString();
        entry.title = query.value("title").toString();
        entry.category_id = query.value("category_id").toInt();

        entry.category = query.value("category_name").toString();

        list.append(entry);
    }

    return list;
}

bool DatabaseManager::deleteEntry(int id) {
    QSqlQuery query;

    query.prepare("DELETE FROM passwords WHERE id = :id");
    query.bindValue(":id", id);

    return query.exec();
}

QString DatabaseManager::generatePassword(int length, bool useSymbols) {
    QString possibleChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    QString password;
    std::random_device rd;
    std::mt19937 gen(rd());


    if(useSymbols) possibleChars += "!@#$%^&*()+-=";
    std::uniform_int_distribution<> dist(0, possibleChars.size()-1);
    for(int i {}; i < length; i++) {
        int random_number = dist(gen);
        password += possibleChars[random_number];
    }

    return password;
}

bool DatabaseManager::updateEntry(const DatabaseManager::PasswordEntry &entry) {
    QSqlQuery query;

    query.prepare("UPDATE passwords SET "
                  "title = :title, "
                  "login = :login, "
                  "password = :password, "
                  "url = :url, "
                  "notes = :notes, "
                  "category_id = :category_id "
                  "WHERE id = :id");

    query.bindValue(":title", entry.title);
    query.bindValue(":login", entry.login);
    query.bindValue(":password", entry.password);
    query.bindValue(":url", entry.url);
    query.bindValue(":notes", entry.notes);
    query.bindValue(":category_id", entry.category_id);
    query.bindValue(":id", entry.id);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления записи:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::updateEntryNote(const DatabaseManager::PasswordEntry &entry) {
    QSqlQuery query;

    query.prepare("UPDATE passwords SET "
                  "notes = :notes "
                  "WHERE id = :id");

    query.bindValue(":notes", entry.notes);
    query.bindValue(":id", entry.id);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления записи:" << query.lastError().text();
        return false;
    }

    return true;
}

QList<DatabaseManager::Category> DatabaseManager::getCategories() {
    QList<Category> list;
    QSqlQuery query("SELECT id, name FROM categories ORDER BY name ASC");

    while (query.next()) {
        Category cat;
        cat.id = query.value("id").toInt();
        cat.name = query.value("name").toString();
        list.append(cat);
    }
    return list;
}

bool DatabaseManager::addCategory(const QString &name) {
    if (name.isEmpty()) return false;

    QSqlQuery query;
    query.prepare("INSERT INTO categories (name) VALUES (:name)");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления категории:" << query.lastError().text();
        return false;
    }
    return true;
}

QString DatabaseManager::hashPassword(const QString &password) {
    QByteArray data = password.toUtf8();

    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);

    return hash.toHex();
}
