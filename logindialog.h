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
    enum class StartupMode {
        OpenExisting,
        CreateNew
    };

    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    QString getPassword() const;
    QString getDatabasePath() const;
    StartupMode getMode() const;

private slots:
    void on_openExistingButton_clicked();
    void on_createNewButton_clicked();
    void on_choosePathButton_clicked();
    void on_confirmButton_clicked();
    void on_backButton_clicked();

private:
    Ui::LoginDialog *ui;

    QString selectedPath;
    StartupMode mode;

    void updateAuthPageTexts();
    bool validateForm();
};

#endif // LOGINDIALOG_H
