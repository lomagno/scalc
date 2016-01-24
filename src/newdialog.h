#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QDialog>
#include "common.h"

namespace Ui {
class NewDialog;
}

class NewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewDialog(QWidget *parent = 0);
    ~NewDialog();
    int getLenght();
    Si::CostMatrixType getCostMatrixType();

private:
    Ui::NewDialog *ui;
};

#endif // NEWDIALOG_H
