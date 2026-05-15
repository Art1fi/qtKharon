#include "editdialog.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

EditDialog::EditDialog(const QString &text, QWidget *parent)
    : QDialog(parent)
    , lineEdit(new QLineEdit(this))
{
    setWindowTitle("Редактирование");
    setModal(true);

    auto *layout = new QVBoxLayout(this);
    auto *label = new QLabel("Измените значение:", this);

    lineEdit->setText(text);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    layout->addWidget(label);
    layout->addWidget(lineEdit);
    layout->addWidget(buttons);

    // Наследуем QSS приложения для визуального соответствия главному окну.
    setStyleSheet(qApp->styleSheet());
}

QString EditDialog::text() const
{
    return lineEdit->text().trimmed();
}
