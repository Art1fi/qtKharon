#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "addentrydialog.h"
#include "modifydialog.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QMessageBox>
#include <QTimer>
#include <QInputDialog>

MainWindow::MainWindow(DatabaseManager *dbManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , db(dbManager)
{
    ui->setupUi(this);

    ui->mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->mainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->mainTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->mainTable->setFocusPolicy(Qt::NoFocus);
    ui->mainTable->setAlternatingRowColors(true);

    ui->mainTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->mainTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->mainTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->mainTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->mainTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->mainTable->verticalHeader()->setDefaultSectionSize(38);

    refreshTable();
    refreshCategories();
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
        ui->mainTable->setItem(row, 2, new QTableWidgetItem("********"));
        ui->mainTable->setItem(row, 3, new QTableWidgetItem(e.url));
        ui->mainTable->setItem(row, 4, new QTableWidgetItem(e.notes));
        ui->mainTable->setItem(row, 5, new QTableWidgetItem(QString::number(e.id)));
        ui->mainTable->setItem(row, 6, new QTableWidgetItem(e.category));
        row++;
    }
}

void MainWindow::on_addButton_clicked()
{
    addEntryDialog dialog(db->getCategories(), this);

    if (dialog.exec() == QDialog::Accepted) {
        DatabaseManager::PasswordEntry newEntry = dialog.getEntryData();

        if (db->addEntry(newEntry)) {
            refreshTable();
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
    QString currentCat = ui->categoryListWidget->item(ui->categoryListWidget->currentRow())->text();
    for (int i = 0; i < ui->mainTable->rowCount(); ++i) {
        QString rowCat = ui->mainTable->item(i, 6)->text();
        bool match = false;
        if (ui->mainTable->item(i, 0)->text().contains(arg1, Qt::CaseInsensitive) && currentCat == rowCat ||
            ui->mainTable->item(i, 1)->text().contains(arg1, Qt::CaseInsensitive) && currentCat == rowCat) {
            match = true;
        }
        ui->mainTable->setRowHidden(i, !match);
    }
}

void MainWindow::on_modifyButton_clicked()
{
    int currentRow = ui->mainTable->currentRow();
    if (currentRow < 0 ) return;
    int id = ui->mainTable->item(currentRow, 5)->text().toInt();
    QList<DatabaseManager::PasswordEntry> list = db->getAllEntries();
    for (auto &e : list) {
        if (e.id == id ) {
            modifyDialog dialog(this);
            dialog.setEntryData(e, db);
            if (dialog.exec() == QDialog::Accepted) {
                DatabaseManager::PasswordEntry newEntry = dialog.getEntryData();

                if (db->updateEntry(newEntry)) {
                    refreshTable();
                }
            } else {
                QMessageBox::critical(this, "Ошибка", "Не удалось обновить данные в базе.");
            }
            break;
        }
    }
}

void MainWindow::refreshCategories() {
    ui->categoryListWidget->clear();
    ui->categoryListWidget->addItem("Все записи");

    QList<DatabaseManager::Category> cats = db->getCategories();
    for (const auto &cat : cats) {
        ui->categoryListWidget->addItem(cat.name);
    }
}

void MainWindow::on_addCategoryButton_clicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, "Новая папка",
                                         "Введите название категории:",
                                         QLineEdit::Normal, "", &ok);

    if (ok && !name.isEmpty()) {
        if (db->addCategory(name)) {
            refreshCategories();
        } else {
            QMessageBox::warning(this, "Ошибка", "Такая категория уже существует!");
        }
    }
}


void MainWindow::on_categoryListWidget_currentRowChanged(int currentRow)
{
    if (currentRow < 0) {
        return;
    }
    QString currentCat = ui->categoryListWidget->item(currentRow)->text();
    for (int i {}; i < ui->mainTable->rowCount(); ++i) {
        QString rowCat = ui->mainTable->item(i, 6)->text();

        bool show = (currentCat == "Все записи" || rowCat == currentCat);
        ui->mainTable->setRowHidden(i, !show);
    }
}

