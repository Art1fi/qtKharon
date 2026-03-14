#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    QString getPassword();
    QString selectedPath;
    QString getDatabasePath();

private slots:
    void on_loginButton_clicked();

    void on_fileButton_clicked();

    void on_createButton_clicked();

private:
    Ui::LoginDialog *ui;

    void loginClicked();

};

#endif // LOGINDIALOG_H

