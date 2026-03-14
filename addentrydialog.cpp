#include "addentrydialog.h"
#include "ui_addentrydialog.h"
#include "databasemanager.h"
#include <random>

addEntryDialog::addEntryDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addEntryDialog)
{
    ui->setupUi(this);

}

addEntryDialog::~addEntryDialog()
{
    delete ui;
}

void addEntryDialog::on_saveButton_clicked()
{
    accept();
}

DatabaseManager::PasswordEntry addEntryDialog::getEntryData() const {
    DatabaseManager::PasswordEntry entry;
    entry.title = ui->titleInput->text(); // Проверь имена объектов в дизайнере!
    entry.login = ui->loginInput->text();
    entry.password = ui->passwordInput->text();
    entry.url = ui->urlInput->text();
    entry.notes = ui->noteInput->text();
    return entry;
}

QString addEntryDialog::generatePassword(int length, bool useSymbols) {
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

void addEntryDialog::on_genButton_clicked()
{
    int length = ui->passwordLength->value();
    bool useSymbols = false;
    if (ui->useSymbols->isChecked()) {
        useSymbols = true;
    }
    QString password = this->generatePassword(length, useSymbols);
    ui->passwordInput->setText(password);
}

