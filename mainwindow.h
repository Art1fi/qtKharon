#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>

#include "databasemanager.h"

class QGraphicsOpacityEffect;
class QLabel;
class QParallelAnimationGroup;
class QWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(DatabaseManager *dbManager, QWidget *parent = nullptr);
    ~MainWindow();

    void refreshTable();

    void refreshCategories();

private slots:
    void on_addButton_clicked();

    void on_deleteButton_clicked();

    void on_copyButton_clicked();

    void on_searchBar_textChanged(const QString &arg1);

    void on_modifyButton_clicked();

    void on_addCategoryButton_clicked();

    void on_categoryListWidget_currentRowChanged(int currentRow);

private:
    void applyFilters();
    void setupSorting();
    void setupIntroAnimation();
    void startContentFadeIn();

    Ui::MainWindow *ui;
    DatabaseManager *db;
    QPointer<QWidget> introOverlay;
    QPointer<QLabel> gifPlaceholderLabel;
    QList<QPointer<QWidget>> animatableWidgets;

protected:
    void resizeEvent(QResizeEvent *event) override;
};
#endif // MAINWINDOW_H
