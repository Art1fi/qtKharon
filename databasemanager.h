#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QList>

class DatabaseManager
{

private:
    QSqlDatabase main;
public:

    DatabaseManager();
    struct Category {
        int id;
        QString name;
    };
    struct PasswordEntry {
        int id;
        QString title;
        QString login;
        QString password;
        QString url;
        QString notes;
        int category_id;
        QString category;
    };

    bool addEntry(const PasswordEntry &entry);
    bool updateEntry(const PasswordEntry &entry);
    bool openDatabase(const QString &path, const QString &password);
    QList<PasswordEntry> getAllEntries();
    bool deleteEntry(int id);
    void closeDatabase();
    static QString generatePassword(int length, bool useSymbols);
    QList<Category> getCategories();
    bool addCategory(const QString &name);
    static QString hashPassword(const QString &password);
    bool updateEntryNote(const DatabaseManager::PasswordEntry &entry);
};

#endif // DATABASEMANAGER_H
