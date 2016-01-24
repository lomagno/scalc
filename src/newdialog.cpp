#include "newdialog.h"
#include "ui_newdialog.h"

NewDialog::NewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewDialog)
{
    ui->setupUi(this);
}

NewDialog::~NewDialog()
{
    delete ui;
}

int NewDialog::getLenght()
{
    return ui->lengthSpinBox->value();
}

Si::CostMatrixType NewDialog::getCostMatrixType()
{
    switch (ui->costMatrixComboBox->currentIndex())
    {
        case 0:
            return Si::DistanceMatrix;
        case 1:
            return Si::UMatrix;
        default:
            return Si::DistanceMatrix;
    }
}
