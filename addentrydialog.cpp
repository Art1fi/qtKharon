#include "addentrydialog.h"
#include "ui_addentrydialog.h"
#include "databasemanager.h"
#include <QDateTime>

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
    entry.title = ui->titleInput->text();
    entry.login = ui->loginInput->text();
    entry.password = ui->passwordInput->text();
    entry.url = ui->urlInput->text();
    entry.notes = ui->noteInput->text();
    return entry;
}

void addEntryDialog::on_genButton_clicked()
{
    int length = ui->passwordLength->value();
    bool useSymbols = false;
    if (ui->useSymbols->isChecked()) {
        useSymbols = true;
    }
    QString password = DatabaseManager::generatePassword(length, useSymbols);
    ui->passwordInput->setText(password);
}


void addEntryDialog::on_dateButton_clicked()
{
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");

    QString currentNotes = ui->noteInput->text();

    if (currentNotes.isEmpty()) {
        ui->noteInput->setText("Запись от: " + currentDateTime);
    } else {
        ui->noteInput->setText(currentNotes + " (" + currentDateTime + ")");
    }
}

