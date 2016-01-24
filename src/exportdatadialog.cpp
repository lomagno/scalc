#include "exportdatadialog.h"
#include "ui_exportdatadialog.h"
#include <QFileDialog>
#include <QMessageBox>

ExportDataDialog::ExportDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDataDialog)
{
    ui->setupUi(this);
    connect(ui->chooseFilePathButton, SIGNAL(clicked()), this, SLOT(onChooseFilePath()));
    connect(ui->exportAsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onOrientationChanged(int)));
}

ExportDataDialog::~ExportDataDialog()
{
    delete ui;
}

void ExportDataDialog::onChooseFilePath()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export file",
        QDir::homePath(),
        "Comma-separated values (*.csv)");

    if (fileName.isEmpty())
        return;

    ui->filePathLineEdit->setText(fileName);
}

QString ExportDataDialog::getExportFilePath()
{
    return ui->filePathLineEdit->text();
}

Qt::Orientation ExportDataDialog::getOrientation()
{
    switch (ui->exportAsComboBox->currentIndex())
    {
        case 0:
            return Qt::Vertical;
        case 1:
            return Qt::Horizontal;
        default:
            return Qt::Vertical;
    }
}

void ExportDataDialog::accept()
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

void ExportDataDialog::onOrientationChanged(int currentIndex)
{
    if (currentIndex == 1)
        ui->separatorComboBox->setEnabled(true);
    else
        ui->separatorComboBox->setEnabled(false);
}

QString ExportDataDialog::getSeparator()
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


