#include "CustomDialog.h"
#include "ui_CustomDialog.h"

CustomDialog::CustomDialog(QPoint rowRange, QPoint colRange, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::CustomDialog)
{
    ui->setupUi(this);
    ui->rowValue->setRange(rowRange.x(), rowRange.y());
    ui->colValue->setRange(colRange.x(), colRange.y());
    ui->mineValue->setRange(1, ui->rowValue->value() * ui->colValue->value());
    ui->mineValue->setValue(ui->rowValue->value() * ui->colValue->value() / 3);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint
                   | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
}

CustomDialog::~CustomDialog()
{
    delete ui;
}

QSize CustomDialog::getTileSize() const
{
    return QSize(ui->colValue->value(), ui->rowValue->value());
}

int CustomDialog::getMineCount() const
{
    return ui->mineValue->value();
}

void CustomDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CustomDialog::on_rowValue_valueChanged(int)
{
    ui->mineValue->setRange(1, ui->rowValue->value() * ui->colValue->value());
}

void CustomDialog::on_colValue_valueChanged(int)
{
    ui->mineValue->setRange(1, ui->rowValue->value() * ui->colValue->value());
}
