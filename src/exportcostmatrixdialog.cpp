#include "exportcostmatrixdialog.h"
#include "ui_exportcostmatrixdialog.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

ExportCostMatrixDialog::ExportCostMatrixDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportCostMatrixDialog)
{
    ui->setupUi(this);
    connect(ui->chooseFilePathButton, SIGNAL(clicked()), this, SLOT(onChooseFilePath()));
}

ExportCostMatrixDialog::~ExportCostMatrixDialog()
{
    delete ui;
}

void ExportCostMatrixDialog::onChooseFilePath()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export file",
        QDir::homePath(),
        "Comma-separated values (*.csv)");

    if (fileName.isEmpty())
        return;

    ui->exportFilePathLineEdit->setText(fileName);
}

QString ExportCostMatrixDialog::getExportFilePath()
{
    return ui->exportFilePathLineEdit->text();
}

QString ExportCostMatrixDialog::getSeparator()
{
    switch (ui->separatorComboBox->currentIndex())
    {
        case 0:
            return ",";
        case 1:
            return ";";
        case 2:
            return ":";
        case 3:
            return "\t";
        case 4:
            return " ";
        default:
            return ",";
    }
}

void ExportCostMatrixDialog::accept()
{
    // Check file path
    QString exportFilePath = getExportFilePath();
    QFileInfo fileInfo(exportFilePath);
    if (fileInfo.isRelative())
    {
        QMessageBox::critical(this, "Seasonal index calculator",
            "Only absolute file paths are allowed");
        return;
    }
    QDialog::accept();
}
