#include "importcostmatrixdialog.h"
#include "ui_importcostmatrixdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>

ImportCostMatrixDialog::ImportCostMatrixDialog(int seasonalPatternLength, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportCostMatrixDialog)
{
    ui->setupUi(this);
    m_seasonalPatternLength = seasonalPatternLength;
    connect(ui->chooseImportFileButton, SIGNAL(clicked()), this, SLOT(onChooseImportFileButtonClicked()));
}

ImportCostMatrixDialog::~ImportCostMatrixDialog()
{
    delete ui;
}

void ImportCostMatrixDialog::onChooseImportFileButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Import file",
        QDir::homePath(),
        "Comma-separated values (*.csv)");

    if (fileName.isEmpty())
        return;

    ui->importFileLineEdit->setText(fileName);
}

void ImportCostMatrixDialog::accept()
{
    QString filePath = ui->importFileLineEdit->text();
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists())
    {
        QMessageBox::critical(this, "Seasonal index calculator",
            "File does not exist");
        return;
    }
    if (fileInfo.isRelative())
    {
        QMessageBox::critical(this, "Seasonal index calculator",
            "Only absolute file paths are allowed");
        return;
    }

    // Separator
    QString separator;
    switch (ui->separatorComboBox->currentIndex())
    {
        case 0:
            separator = ",";
            break;
        case 1:
            separator = ";";
            break;
        case 2:
            separator = ":";
            break;
        case 3:
            separator = "\t";
            break;
        case 4:
            separator = " ";
            break;
        default:
            separator = ",";
            break;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Seasonal index calculator",
            "Error reading the file");
        return;
    }

    QTextStream in(&file);
    int rowCount = 0;
    while (!in.atEnd())
    {
        rowCount++;
        QString line = in.readLine();
        QStringList splittedLine = line.split(separator);
        if (splittedLine.size() != m_seasonalPatternLength)
        {
            QMessageBox::critical(this, "Seasonal index calculator",
                "There is a row with a number of elements that is different from the seasonal pattern length");
            m_data.clear();
            return;
        }
        QList<double> row;
        bool isConversionOk;
        for (int i=0; i<m_seasonalPatternLength; ++i)
        {
            double value = splittedLine.at(i).toDouble(&isConversionOk);
            if (!isConversionOk)
            {
                QMessageBox::critical(this, "Seasonal index calculator",
                    "Error converting to double precision number");
                m_data.clear();
                return;
            }
            row << value;
        }
        m_data << row;
    }

    // Check number of rows
    if (rowCount != m_seasonalPatternLength)
    {
        QMessageBox::critical(this, "Seasonal index calculator",
            "The number of rows is different from the seasonal pattern length");
        m_data.clear();
        return;
    }

    QDialog::accept();
}

QList<QList<double> > ImportCostMatrixDialog::getData()
{
    return m_data;
}
