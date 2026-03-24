#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "databasemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(DatabaseManager *dbManager, QWidget *parent = nullptr);
    ~MainWindow();

    void refreshTable();

private slots:
    void on_addButton_clicked();

    void on_deleteButton_clicked();

    void on_copyButton_clicked();

    void on_searchBar_textChanged(const QString &arg1);

    void on_modifyButton_clicked();

private:
    QList<DatabaseManager::PasswordEntry> list;
    Ui::MainWindow *ui;
    DatabaseManager *db;
};
#endif // MAINWINDOW_H
