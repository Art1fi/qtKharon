#ifndef ADDENTRYDIALOG_H
#define ADDENTRYDIALOG_H

#include <QDialog>
#include "databasemanager.h"

namespace Ui {
class addEntryDialog;
}

class addEntryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addEntryDialog(QWidget *parent = nullptr);
    ~addEntryDialog();
    DatabaseManager::PasswordEntry getEntryData() const;
    QString generatePassword(int length, bool useSymbols);

private slots:
    void on_saveButton_clicked();

    void on_genButton_clicked();

private:
    Ui::addEntryDialog *ui;
};

#endif // ADDENTRYDIALOG_H
