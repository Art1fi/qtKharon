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

    QFile styleFile(":/styles/app.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&styleFile);
        a.setStyleSheet(stream.readAll());
    }

    DatabaseManager dbManager;
    LoginDialog loginDialog;

    if (loginDialog.exec() != QDialog::Accepted) {
        return 0;
    }

    const QString password = loginDialog.getPassword();
    const QString path = loginDialog.getDatabasePath();

    if (!dbManager.openDatabase(path, password)) {
        QMessageBox::critical(nullptr,
                              "Ошибка",
                              "Не удалось открыть базу данных. Проверьте путь и пароль.");
        return 0;
    }

    MainWindow window(&dbManager);
    window.show();
    window.refreshTable();

    return a.exec();
}
