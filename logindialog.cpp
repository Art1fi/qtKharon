#include "logindialog.h"
#include "ui_logindialog.h"
#include <QFileDialog>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_loginButton_clicked()
{
    accept();
}

QString LoginDialog::getPassword() {
    return ui->loginField->text();
}

void LoginDialog::on_fileButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Открыть базу данных", "", "Базы данных (*.db *.sqlite);;Все файлы (*.*)");

    if (!path.isEmpty()) {
        selectedPath = path;
        this->setWindowTitle("База: " + QFileInfo(path).fileName());
    }
}

QString LoginDialog::getDatabasePath() {
    return selectedPath;
}


void LoginDialog::on_createButton_clicked()
{
    // QFileDialog::getSaveFileName вызывает окно сохранения файла
    QString path = QFileDialog::getSaveFileName(this, "Создать новую базу данных", "", "Базы данных (*.db)");

    if (!path.isEmpty()) {
        selectedPath = path; // Сохраняем путь в переменную класса
        this->setWindowTitle("Новая база: " + QFileInfo(path).fileName());
        // После выбора пути пользователь вводит пароль в поле и жмет "Войти" (или мы можем сразу принять)
    }
}

