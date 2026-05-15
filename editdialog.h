#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>

class QLineEdit;

class EditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDialog(const QString &text, QWidget *parent = nullptr);
    QString text() const;

private:
    QLineEdit *lineEdit;
};

#endif // EDITDIALOG_H
