#include "modifydialog.h"
#include "ui_modifydialog.h"

modifyDialog::modifyDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::modifyDialog)
{
    ui->setupUi(this);
}

modifyDialog::~modifyDialog()
{
    delete ui;
}

void modifyDialog::on_genModButton_clicked()
{

}

