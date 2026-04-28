#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "addentrydialog.h"
#include "modifydialog.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QClipboard>
#include <QGraphicsOpacityEffect>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QTableWidgetItem>
#include <QTimer>
#include <QVBoxLayout>
#include <utility>

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

    setupSorting();
    refreshCategories();
    setupIntroAnimation();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupSorting()
{
    ui->mainTable->setSortingEnabled(true);
    ui->mainTable->horizontalHeader()->setSortIndicatorShown(true);
    ui->mainTable->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
}

void MainWindow::refreshTable()
{
    const int sortColumn = ui->mainTable->horizontalHeader()->sortIndicatorSection();
    const Qt::SortOrder sortOrder = ui->mainTable->horizontalHeader()->sortIndicatorOrder();

    const QList<DatabaseManager::PasswordEntry> entries = db->getAllEntries();

    ui->mainTable->setSortingEnabled(false);
    ui->mainTable->setRowCount(entries.size());
    ui->mainTable->clearContents();
    ui->mainTable->setColumnHidden(5, true);
    ui->mainTable->setColumnHidden(6, true);

    int row = 0;
    for (const auto &e : entries) {
        ui->mainTable->setItem(row, 0, new QTableWidgetItem(e.title));
        ui->mainTable->setItem(row, 1, new QTableWidgetItem(e.login));
        ui->mainTable->setItem(row, 2, new QTableWidgetItem("********"));
        ui->mainTable->setItem(row, 3, new QTableWidgetItem(e.url));
        ui->mainTable->setItem(row, 4, new QTableWidgetItem(e.notes));
        ui->mainTable->setItem(row, 5, new QTableWidgetItem(QString::number(e.id)));
        ui->mainTable->setItem(row, 6, new QTableWidgetItem(e.category));
        row++;
    }

    ui->mainTable->setSortingEnabled(true);
    ui->mainTable->sortByColumn(sortColumn, sortOrder);
    applyFilters();
}

void MainWindow::applyFilters()
{
    const QString search = ui->searchBar->text().trimmed();
    const QListWidgetItem *currentItem = ui->categoryListWidget->currentItem();
    const QString currentCat = currentItem ? currentItem->text() : QStringLiteral("Все записи");

    for (int i = 0; i < ui->mainTable->rowCount(); ++i) {
        const QString rowCat = ui->mainTable->item(i, 6)->text();
        const bool categoryMatch = (currentCat == "Все записи" || rowCat == currentCat);

        const bool searchMatch = search.isEmpty()
                                 || ui->mainTable->item(i, 0)->text().contains(search, Qt::CaseInsensitive)
                                 || ui->mainTable->item(i, 1)->text().contains(search, Qt::CaseInsensitive)
                                 || ui->mainTable->item(i, 3)->text().contains(search, Qt::CaseInsensitive);

        ui->mainTable->setRowHidden(i, !(categoryMatch && searchMatch));
    }
}

void MainWindow::setupIntroAnimation()
{
    introOverlay = new QWidget(ui->centralwidget);
    introOverlay->setObjectName("introOverlay");
    introOverlay->setGeometry(ui->centralwidget->rect());

    auto *overlayLayout = new QVBoxLayout(introOverlay);
    overlayLayout->setContentsMargins(24, 24, 24, 24);

    auto *welcomeLabel = new QLabel("Добро пожаловать в Kharon", introOverlay);
    welcomeLabel->setObjectName("welcomeLabel");
    welcomeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    gifPlaceholderLabel = new QLabel("GIF placeholder\n(загрузите сюда вашу анимацию)", introOverlay);
    gifPlaceholderLabel->setObjectName("gifPlaceholderLabel");
    gifPlaceholderLabel->setAlignment(Qt::AlignCenter);
    gifPlaceholderLabel->setMinimumHeight(220);

    overlayLayout->addStretch();
    overlayLayout->addWidget(welcomeLabel);
    overlayLayout->addSpacing(18);
    overlayLayout->addWidget(gifPlaceholderLabel);
    overlayLayout->addStretch();

    introOverlay->raise();

    const QList<QWidget *> widgetsToFade = {
        ui->categoryListWidget,
        ui->addCategoryButton,
        ui->titleLabel,
        ui->searchLabel,
        ui->searchBar,
        ui->addButton,
        ui->deleteButton,
        ui->copyButton,
        ui->modifyButton,
        ui->mainTable
    };

    for (QWidget *w : widgetsToFade) {
        auto *effect = new QGraphicsOpacityEffect(w);
        effect->setOpacity(0.0);
        w->setGraphicsEffect(effect);
        animatableWidgets.append(w);
    }

    QTimer::singleShot(1700, this, [this]() {
        if (!introOverlay) {
            startContentFadeIn();
            return;
        }

        auto *overlayEffect = new QGraphicsOpacityEffect(introOverlay);
        introOverlay->setGraphicsEffect(overlayEffect);

        auto *fadeOverlay = new QPropertyAnimation(overlayEffect, "opacity", this);
        fadeOverlay->setDuration(700);
        fadeOverlay->setStartValue(1.0);
        fadeOverlay->setEndValue(0.0);

        connect(fadeOverlay, &QPropertyAnimation::finished, this, [this]() {
            if (introOverlay) {
                introOverlay->deleteLater();
            }
            refreshTable();
            startContentFadeIn();
        });

        fadeOverlay->start(QAbstractAnimation::DeleteWhenStopped);
    });
}

void MainWindow::startContentFadeIn()
{
    auto *group = new QParallelAnimationGroup(this);

    int delayStep = 45;
    int idx = 0;
    for (const QPointer<QWidget> &widget : std::as_const(animatableWidgets)) {
        if (!widget || !widget->graphicsEffect()) {
            continue;
        }

        auto *anim = new QPropertyAnimation(widget->graphicsEffect(), "opacity", group);
        anim->setDuration(320 + idx * delayStep);
        anim->setStartValue(0.0);
        anim->setEndValue(1.0);
        group->addAnimation(anim);
        ++idx;
    }

    group->start(QAbstractAnimation::DeleteWhenStopped);
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

void MainWindow::on_deleteButton_clicked()
{
    const int currentRow = ui->mainTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::information(this, "Внимание", "Выберите запись для удаления");
        return;
    }

    const int id = ui->mainTable->item(currentRow, 5)->text().toInt();

    if (QMessageBox::question(this, "Удаление", "Вы уверены?") == QMessageBox::Yes) {
        if (db->deleteEntry(id)) {
            refreshTable();
        }
    }
}

void MainWindow::on_copyButton_clicked()
{
    const int currentRow = ui->mainTable->currentRow();
    if (currentRow < 0) {
        return;
    }

    const int id = ui->mainTable->item(currentRow, 5)->text().toInt();
    const QList<DatabaseManager::PasswordEntry> entries = db->getAllEntries();

    for (const auto &e : entries) {
        if (e.id == id) {
            QApplication::clipboard()->setText(e.password);
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
    Q_UNUSED(arg1);
    applyFilters();
}

void MainWindow::on_modifyButton_clicked()
{
    const int currentRow = ui->mainTable->currentRow();
    if (currentRow < 0) {
        return;
    }

    const int id = ui->mainTable->item(currentRow, 5)->text().toInt();
    const QList<DatabaseManager::PasswordEntry> entries = db->getAllEntries();

    for (const auto &e : entries) {
        if (e.id == id) {
            modifyDialog dialog(this);
            dialog.setEntryData(e, db);
            if (dialog.exec() == QDialog::Accepted) {
                const DatabaseManager::PasswordEntry newEntry = dialog.getEntryData();

                if (db->updateEntry(newEntry)) {
                    refreshTable();
                }
            }
            break;
        }
    }
}

void MainWindow::refreshCategories()
{
    ui->categoryListWidget->clear();
    ui->categoryListWidget->addItem("Все записи");

    const QList<DatabaseManager::Category> cats = db->getCategories();
    for (const auto &cat : cats) {
        ui->categoryListWidget->addItem(cat.name);
    }

    ui->categoryListWidget->setCurrentRow(0);
}

void MainWindow::on_addCategoryButton_clicked()
{
    bool ok = false;
    const QString name = QInputDialog::getText(this,
                                               "Новая папка",
                                               "Введите название категории:",
                                               QLineEdit::Normal,
                                               "",
                                               &ok);

    if (ok && !name.isEmpty()) {
        if (db->addCategory(name)) {
            refreshCategories();
            applyFilters();
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

    applyFilters();
}
