#ifndef CUSTOMDIALOG_H
#define CUSTOMDIALOG_H

#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>

namespace Ui {
class CustomDialog;
}

class CustomDialog : public QDialog
{
    Q_OBJECT

public:
    CustomDialog(QPoint rowRange, QPoint colRange, QWidget *parent = 0);
    ~CustomDialog();

    QSize getTileSize() const;
    int getMineCount() const;

protected:
    void changeEvent(QEvent *e);

private:
    Q_SLOT void on_rowValue_valueChanged(int value);
    Q_SLOT void on_colValue_valueChanged(int value);

    Ui::CustomDialog *ui;
};

#endif // CUSTOMDIALOG_H
