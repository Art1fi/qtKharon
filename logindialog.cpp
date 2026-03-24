    #include "logindialog.h"
#include "ui_logindialog.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegularExpression>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , mode(StartupMode::OpenExisting)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->choicePage);
    ui->passwordField->setEchoMode(QLineEdit::Password);
    ui->dbNameField->setVisible(false);
    ui->dbNameHintLabel->setVisible(false);
    ui->dbNameField->setText("kharon");
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_openExistingButton_clicked()
{
    mode = StartupMode::OpenExisting;
    selectedPath.clear();
    selectedDirectory.clear();
    ui->pathValueLabel->setText("Файл не выбран");
    ui->pathValueLabel->setToolTip(QString());
    ui->passwordField->clear();
    updateAuthPageTexts();
    ui->stackedWidget->setCurrentWidget(ui->authPage);
}

void LoginDialog::on_createNewButton_clicked()
{
    mode = StartupMode::CreateNew;
    selectedPath.clear();
    selectedDirectory.clear();
    ui->pathValueLabel->setText("Папка не выбрана");
    ui->pathValueLabel->setToolTip(QString());
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
        selectedDirectory = dirPath;
        updateCreatePathPreview();
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

void LoginDialog::on_dbNameField_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if (mode == StartupMode::CreateNew) {
        updateCreatePathPreview();
    }
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
        ui->dbNameHintLabel->setVisible(false);
        ui->dbNameField->setVisible(false);
        return;
    }

    ui->authTitleLabel->setText("Создание новой базы");
    ui->pathHintLabel->setText("Папка для базы:");
    ui->choosePathButton->setText("Выбрать папку");
    ui->passwordHintLabel->setText("Придумайте пароль:");
    ui->passwordRulesLabel->setText("Минимум 8 символов");
    ui->confirmButton->setText("Создать базу");
    ui->dbNameHintLabel->setVisible(true);
    ui->dbNameField->setVisible(true);
    updateCreatePathPreview();
}

void LoginDialog::updateCreatePathPreview()
{
    if (selectedDirectory.isEmpty()) {
        selectedPath.clear();
        ui->pathValueLabel->setText("Папка не выбрана");
        ui->pathValueLabel->setToolTip(QString());
        return;
    }

    const QString dbName = ui->dbNameField->text().trimmed();
    const QString safeName = dbName.isEmpty() ? "kharon" : dbName;
    selectedPath = QDir(selectedDirectory).filePath(safeName + ".db");
    ui->pathValueLabel->setText(selectedPath);
    ui->pathValueLabel->setToolTip(selectedPath);
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

    if (mode == StartupMode::CreateNew) {
        if (password.size() < 8) {
            QMessageBox::warning(this, "Слишком короткий пароль", "Для новой базы пароль должен быть не менее 8 символов.");
            return false;
        }

        const QString dbName = ui->dbNameField->text().trimmed();
        if (dbName.isEmpty()) {
            QMessageBox::warning(this, "Недостаточно данных", "Введите имя новой базы данных.");
            return false;
        }

        if (!QRegularExpression("^[A-Za-z0-9_-]+$").match(dbName).hasMatch()) {
            QMessageBox::warning(this, "Некорректное имя", "Имя базы может содержать только латинские буквы, цифры, '_' и '-'.");
            return false;
        }
    }

    return true;
}
