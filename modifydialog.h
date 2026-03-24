#ifndef MODIFYDIALOG_H
#define MODIFYDIALOG_H

#include <QDialog>
#include "databasemanager.h"

namespace Ui {
class modifyDialog;
}

class modifyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit modifyDialog(QWidget *parent = nullptr);
    ~modifyDialog();
    void setEntryData(const DatabaseManager::PasswordEntry &entry);
    DatabaseManager::PasswordEntry getEntryData() const;

private slots:
    void on_genModButton_clicked();

    void on_saveModButton_clicked();

private:
    Ui::modifyDialog *ui;
    int m_id;
};

#endif // MODIFYDIALOG_H
