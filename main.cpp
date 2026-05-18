#include "mainwindow.h"
#include "logindialog.h"
#include "databasemanager.h"

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile styleFile(":/styles/styles/app.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&styleFile);
        a.setStyleSheet(stream.readAll());
    }

    DatabaseManager dbManager;
    LoginDialog loginDialog;

    bool isDbOpened = false;

    while (!isDbOpened) {
        if (loginDialog.exec() != QDialog::Accepted) {
            return 0;
        }

        const QString password = loginDialog.getPassword();
        const QString path = loginDialog.getDatabasePath();

        if (dbManager.openDatabase(path, password)) {
            isDbOpened = true;
        } else {
            QMessageBox::critical(nullptr,
                                  "Ошибка",
                                  "Не удалось открыть базу данных. Проверьте путь и пароль.");
        }
    }

    MainWindow window(&dbManager);
    window.show();
    window.refreshTable();

    return a.exec();
}
