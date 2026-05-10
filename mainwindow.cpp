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
#include <QFile>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>
#include <utility>

MainWindow::MainWindow(DatabaseManager *dbManager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , db(dbManager)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground, false);

    ui->mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->mainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->mainTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->mainTable->setFocusPolicy(Qt::NoFocus);
    ui->mainTable->setAlternatingRowColors(true);

    ui->mainTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->mainTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->mainTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->mainTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Interactive);
    ui->mainTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->mainTable->horizontalHeader()->setMinimumSectionSize(90);
    ui->mainTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->mainTable->horizontalHeader()->setSectionsClickable(true);
    ui->mainTable->setWordWrap(false);
    ui->mainTable->setTextElideMode(Qt::ElideRight);
    ui->mainTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->mainTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->mainTable->verticalHeader()->setDefaultSectionSize(38);
    ui->mainTable->setColumnWidth(0, 170);
    ui->mainTable->setColumnWidth(1, 150);
    ui->mainTable->setColumnWidth(2, 125);
    ui->mainTable->setColumnWidth(3, 220);

    setupSorting();
    refreshCategories();
    setupIntroAnimation();
    applyTheme(false);
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
        auto *siteItem = ui->mainTable->item(i, 0);
        auto *loginItem = ui->mainTable->item(i, 1);
        auto *urlItem = ui->mainTable->item(i, 3);
        auto *catItem = ui->mainTable->item(i, 6);
        if (!siteItem || !loginItem || !urlItem || !catItem) {
            ui->mainTable->setRowHidden(i, true);
            continue;
        }

        const QString rowCat = catItem->text();
        const bool categoryMatch = (currentCat == "Все записи" || rowCat == currentCat);

        const bool searchMatch = search.isEmpty()
                                 || siteItem->text().contains(search, Qt::CaseInsensitive)
                                 || loginItem->text().contains(search, Qt::CaseInsensitive)
                                 || urlItem->text().contains(search, Qt::CaseInsensitive);

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

    auto *welcomeLabel = new QLabel("Добро пожаловать", introOverlay);
    welcomeLabel->setObjectName("welcomeLabel");
    welcomeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    overlayLayout->addStretch();
    overlayLayout->addWidget(welcomeLabel);
    overlayLayout->addSpacing(18);
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
        fadeOverlay->setEasingCurve(QEasingCurve::InCubic);

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
        auto *moveAnim = new QPropertyAnimation(widget, "pos", group);
        QPoint endPos = widget->pos();
        QPoint startPos = endPos + QPoint(0, 20);

        moveAnim->setDuration(500);
        moveAnim->setStartValue(startPos);
        moveAnim->setEndValue(endPos);
        moveAnim->setEasingCurve(QEasingCurve::OutBack);

        group->addAnimation(moveAnim);
        auto *anim = new QPropertyAnimation(widget->graphicsEffect(), "opacity", group);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->setDuration(160 + idx * delayStep);
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

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (introOverlay) {
        introOverlay->setGeometry(ui->centralwidget->rect());
    }
}

QString MainWindow::loadStyleSheet(const QString &resourcePath) const
{
    QFile styleFile(resourcePath);
    if (!styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

    QTextStream stream(&styleFile);
    return stream.readAll();
}

void MainWindow::applyTheme(bool darkTheme)
{
    const QString stylePath = darkTheme ? QStringLiteral(":/styles/styles/dark.qss")
                                        : QStringLiteral(":/styles/styles/app.qss");
    const QString styleSheet = loadStyleSheet(stylePath);
    if (!styleSheet.isEmpty()) {
        qApp->setStyleSheet(styleSheet);
        isDarkTheme = darkTheme;
        ui->themeToggleButton->setText(isDarkTheme ? "Светлая тема" : "Тёмная тема");
    }
}

void MainWindow::on_themeToggleButton_clicked()
{
    applyTheme(!isDarkTheme);
}
