#include "mainwindow.h"
#include "logindialog.h"
#include "databasemanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DatabaseManager dbManager;
    MainWindow w(&dbManager);
    LoginDialog l;
    QString path = "";
    qDebug() << QSqlDatabase::drivers();


    if (l.exec() == QDialog::Accepted) {
        QString password = l.getPassword();
        path = l.getDatabasePath();
        if (dbManager.openDatabase(path, password)) {
            w.show();
            w.MainWindow::refreshTable();

        }
        else {
            return 0;
        }
    }

    return a.exec();
}
