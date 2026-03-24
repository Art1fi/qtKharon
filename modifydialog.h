#ifndef MODIFYDIALOG_H
#define MODIFYDIALOG_H

#include <QDialog>

namespace Ui {
class modifyDialog;
}

class modifyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit modifyDialog(QWidget *parent = nullptr);
    ~modifyDialog();

private slots:
    void on_genModButton_clicked();

private:
    Ui::modifyDialog *ui;
};

#endif // MODIFYDIALOG_H
