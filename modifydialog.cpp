#include "modifydialog.h"
#include "ui_modifydialog.h"
#include "databasemanager.h"

modifyDialog::modifyDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::modifyDialog)
{
    ui->setupUi(this);
}

modifyDialog::~modifyDialog()
{
    delete ui;
}

void modifyDialog::on_genModButton_clicked()
{
    int length = ui->passwordLengthMod->value();
    bool useSymbols = false;
    if (ui->symbolsMod->isChecked()) {
        useSymbols = true;
    }
    QString password = DatabaseManager::generatePassword(length, useSymbols);
    ui->passwordModify->setText(password);
}

void modifyDialog::setEntryData(const DatabaseManager::PasswordEntry &entry) {
    m_id = entry.id;
    ui->nameModify->setText(entry.title);
    ui->loginModify->setText(entry.login);
    ui->passwordModify->setText(entry.password);
    ui->urlModify->setText(entry.url);
    ui->notesModify->setText(entry.notes);
    ui->categorySelect->setCurrentText(entry.category);
}

DatabaseManager::PasswordEntry modifyDialog::getEntryData() const {
    DatabaseManager::PasswordEntry entry;
    entry.id = m_id;
    entry.title = ui->nameModify->text();
    entry.login = ui->loginModify->text();
    entry.password = ui->passwordModify->text();
    entry.url = ui->urlModify->text();
    entry.notes = ui->notesModify->text();
    entry.category = ui->categorySelect->currentText();
    return entry;
}

void modifyDialog::on_saveModButton_clicked()
{
    accept();
}

