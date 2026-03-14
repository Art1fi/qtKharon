#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , mode(StartupMode::OpenExisting)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->choicePage);
    ui->passwordField->setEchoMode(QLineEdit::Password);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_openExistingButton_clicked()
{
    mode = StartupMode::OpenExisting;
    selectedPath.clear();
    ui->pathValueLabel->setText("Файл не выбран");
    ui->passwordField->clear();
    updateAuthPageTexts();
    ui->stackedWidget->setCurrentWidget(ui->authPage);
}

void LoginDialog::on_createNewButton_clicked()
{
    mode = StartupMode::CreateNew;
    selectedPath.clear();
    ui->pathValueLabel->setText("Папка не выбрана");
    ui->passwordField->clear();
    updateAuthPageTexts();
    ui->stackedWidget->setCurrentWidget(ui->authPage);
}

void LoginDialog::on_choosePathButton_clicked()
{
    if (mode == StartupMode::OpenExisting) {
        const QString path = QFileDialog::getOpenFileName(
            this,
            "Выберите базу данных",
            QString(),
            "Базы данных (*.db *.sqlite);;Все файлы (*.*)");

        if (!path.isEmpty()) {
            selectedPath = path;
            ui->pathValueLabel->setText(QFileInfo(path).fileName());
            ui->pathValueLabel->setToolTip(path);
        }
        return;
    }

    const QString dirPath = QFileDialog::getExistingDirectory(this, "Выберите папку для новой базы");
    if (!dirPath.isEmpty()) {
        selectedPath = QDir(dirPath).filePath("kharon.db");
        ui->pathValueLabel->setText(selectedPath);
        ui->pathValueLabel->setToolTip(selectedPath);
    }
}

void LoginDialog::on_confirmButton_clicked()
{
    if (validateForm()) {
        accept();
    }
}

void LoginDialog::on_backButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->choicePage);
}

QString LoginDialog::getPassword() const
{
    return ui->passwordField->text();
}

QString LoginDialog::getDatabasePath() const
{
    return selectedPath;
}

LoginDialog::StartupMode LoginDialog::getMode() const
{
    return mode;
}

void LoginDialog::updateAuthPageTexts()
{
    if (mode == StartupMode::OpenExisting) {
        ui->authTitleLabel->setText("Открытие существующей базы");
        ui->pathHintLabel->setText("Файл базы данных:");
        ui->choosePathButton->setText("Выбрать файл");
        ui->passwordHintLabel->setText("Введите пароль:");
        ui->passwordRulesLabel->setText("Введите пароль для расшифровки базы");
        ui->confirmButton->setText("Открыть");
        return;
    }

    ui->authTitleLabel->setText("Создание новой базы");
    ui->pathHintLabel->setText("Папка для базы:");
    ui->choosePathButton->setText("Выбрать папку");
    ui->passwordHintLabel->setText("Придумайте пароль:");
    ui->passwordRulesLabel->setText("Минимум 8 символов");
    ui->confirmButton->setText("Создать базу");
}

bool LoginDialog::validateForm()
{
    if (selectedPath.isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Сначала выберите путь к базе данных.");
        return false;
    }

    const QString password = ui->passwordField->text();
    if (password.isEmpty()) {
        QMessageBox::warning(this, "Недостаточно данных", "Введите пароль.");
        return false;
    }

    if (mode == StartupMode::CreateNew && password.size() < 8) {
        QMessageBox::warning(this, "Слишком короткий пароль", "Для новой базы пароль должен быть не менее 8 символов.");
        return false;
    }

    return true;
}
