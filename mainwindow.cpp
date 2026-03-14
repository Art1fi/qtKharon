#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "addentrydialog.h"
#include <QMessageBox>
#include <QClipboard>
#include <QTimer>

MainWindow::MainWindow(DatabaseManager *dbManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , db(dbManager)
{
    ui->setupUi(this);
    refreshTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshTable() {
    QList<DatabaseManager::PasswordEntry> list = db->getAllEntries();

    ui->mainTable->setRowCount(list.size());
    ui->mainTable->clearContents();
    ui->mainTable->setColumnHidden(5, true);

    int row = 0;
    for (const auto &e : list) {
        ui->mainTable->setItem(row, 0, new QTableWidgetItem(e.title));
        ui->mainTable->setItem(row, 1, new QTableWidgetItem(e.login));
        ui->mainTable->setItem(row, 2, new QTableWidgetItem("********")); // Скрываем пароль
        ui->mainTable->setItem(row, 3, new QTableWidgetItem(e.url));
        ui->mainTable->setItem(row, 4, new QTableWidgetItem(e.notes));
        ui->mainTable->setItem(row, 5, new QTableWidgetItem(QString::number(e.id)));

        row++;
    }
}

void MainWindow::on_addButton_clicked()
{
    addEntryDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        DatabaseManager::PasswordEntry newEntry = dialog.getEntryData();

        if (db->addEntry(newEntry)) {
            refreshTable();
        } else {
            // Тут можно вывести QMessageBox с ошибкой
        }
    }
}


void MainWindow::on_deleteButton_clicked() {
    int currentRow = ui->mainTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::information(this, "Внимание", "Выберите запись для удаления");
        return;
    }

    int id = ui->mainTable->item(currentRow, 5)->text().toInt();

    if (QMessageBox::question(this, "Удаление", "Вы уверены?") == QMessageBox::Yes) {
        if (db->deleteEntry(id)) {
            refreshTable();
        }
    }
}


void MainWindow::on_copyButton_clicked()
{
    int currentRow = ui->mainTable->currentRow();
    QString foundPassword;
    if (currentRow < 0 ) return;

    int id = ui->mainTable->item(currentRow, 5)->text().toInt();
    QList<DatabaseManager::PasswordEntry> list = db->getAllEntries();
    for (auto &e : list) {
        if (e.id == id ) {
            foundPassword = e.password;
            QApplication::clipboard()->setText(foundPassword);
            QTimer::singleShot(15000, this, []() {
                QApplication::clipboard()->clear();
            });
            break;
        }
    }

    ui->statusbar->showMessage("Пароль скопирован. Очистка через 15 сек.", 5000);

}

void MainWindow::on_searchBar_textChanged(const QString &arg1)
{
    for (int i = 0; i < ui->mainTable->rowCount(); ++i) {
        bool match = false;
        if (ui->mainTable->item(i, 0)->text().contains(arg1, Qt::CaseInsensitive) ||
            ui->mainTable->item(i, 1)->text().contains(arg1, Qt::CaseInsensitive)) {
            match = true;
        }
        ui->mainTable->setRowHidden(i, !match);
    }
}

