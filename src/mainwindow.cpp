#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vector>
#include <glpk.h>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include "newdialog.h"
#include "exportdatadialog.h"
#include "exportcostmatrixdialog.h"
#include "importcostmatrixdialog.h"
#include "aboutdialog.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Horizontal plitter
    QList<int> horizontalSplitterSizes;
    horizontalSplitterSizes << 150 << 510 << 300;
    ui->horizontalSplitter->setSizes(horizontalSplitterSizes);

    // Vertical splitter
    QList<int> verticalSplitterSizes;
    verticalSplitterSizes << 500 << 300;
    ui->verticalSplitter->setSizes(verticalSplitterSizes);

    // Data
    m_dataModel = new DataModel;
    ui->dataTable->setModel(m_dataModel);
    ui->dataTable->resizeColumnsToContents();

    // Cost matrix
    m_costMatrixModel = new CostMatrixModel;
    ui->costMatrixTable->setModel(m_costMatrixModel);
    ui->costMatrixTable->resizeColumnsToContents();

    // Output
    ui->output->setContextMenuPolicy(Qt::CustomContextMenu);

    // Update status bar
    updateStatusBar();

    // Connections
    connect(ui->loadDistanceMatrixAction, SIGNAL(triggered()), this, SLOT(onLoadDistanceMatrixActionTriggered()));
    connect(ui->loadUMatrixAction, SIGNAL(triggered()), this, SLOT(onLoadUMatrixActionTriggered()));
    connect(ui->symmetricEditModeAction, SIGNAL(triggered(bool)), this, SLOT(onSymmetricEditModeActionTriggered(bool)));
    connect(ui->calculateAction, SIGNAL(triggered()), this, SLOT(onCalculateActionTriggered()));
    connect(m_dataModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged()));
    connect(ui->output, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onOutputContextMenuRequested(QPoint)));
    connect(ui->clearOutputAction, SIGNAL(triggered()), this, SLOT(onClearOutputActionTriggered()));
    connect(ui->newAction, SIGNAL(triggered()), this, SLOT(onNewAction()));
    connect(ui->exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->importDataAction, SIGNAL(triggered()), this, SLOT(onImportDataActionTriggered()));
    connect(ui->exportDataAction, SIGNAL(triggered()), this, SLOT(onExportDataActionTriggered()));
    connect(ui->importCostMatrixAction, SIGNAL(triggered()), this, SLOT(onImportCostMatrixActionTriggered()));
    connect(ui->exportCostMatrixAction, SIGNAL(triggered()), this, SLOT(onExportCostMatrixActionTriggered()));
    connect(ui->exportProblemToGnuMathProgAction, SIGNAL(triggered()), this, SLOT(onExportProblemToGnuMathProgActionTriggered()));
    connect(ui->aboutAction, SIGNAL(triggered()), this, SLOT(onAboutActionTriggered()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLoadDistanceMatrixActionTriggered()
{
    m_costMatrixModel->loadDistanceMatrix();
}

void MainWindow::onLoadUMatrixActionTriggered()
{
    m_costMatrixModel->loadUMatrix();
}

void MainWindow::onDataChanged()
{
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    const char *format = "%.3f";
    QString total = QString().sprintf(format, m_dataModel->total());
    QString mean = QString().sprintf(format, m_dataModel->mean());
    QString min = QString().sprintf(format, m_dataModel->min());
    QString max = QString().sprintf(format, m_dataModel->max());
    QString range = QString().sprintf(format, m_dataModel->range());
    QString nOfHigh = QString("%1").arg(m_dataModel->highCount());
    QString nOfLow = QString("%1").arg(m_dataModel->lowCount());
    ui->statusBar->showMessage(
        QString("Total: %1 | Mean: %2 | Min: %3 | Max: %4 | Range: %5 | N. of high: %6 | N. of low: %7")
            .arg(total)
            .arg(mean)
            .arg(min)
            .arg(max)
            .arg(range)
            .arg(nOfHigh)
            .arg(nOfLow)
    );
}

void MainWindow::onCalculateActionTriggered()
{
    // Output values
    double absoluteSeasonalityIndex;
    double relativeSeasonalityIndex;
    double gini;

    // Supplies and demands
    std::vector<double> s, d;
    std::vector<int> sTime, dTime;
    QList<QPair<double, double> > data = m_dataModel->getData();
    int t=0;
    for (int i=0; i<data.size(); ++i)
    {
        t++;
        double deviation = data.at(i).second;
        if (deviation > 0)
        {
            s.push_back(deviation);
            sTime.push_back(t);
        }
        else if (deviation < 0)
        {
            d.push_back(-deviation);
            dTime.push_back(t);
        }
    }

    // Number of constraints
    size_t nS = s.size();
    size_t nD = d.size();

    if (nS == 0 || nD == 0)
    {
        absoluteSeasonalityIndex = 0;
        relativeSeasonalityIndex = 0;
        gini = 0;
        ui->movementsTable->clear();
        ui->movementsTable->setRowCount(0);
        ui->movementsTable->setColumnCount(0);
    }
    else
    {

        // Create the problem
        glp_prob *lp = glp_create_prob();
        glp_set_obj_dir(lp, GLP_MIN);

        // Objective function
        glp_add_cols(lp, nS*nD);
        {
            int w = 0;
            for (size_t i=0; i<nS; ++i)
            {
                for (size_t j=0; j<nD; ++j)
                {
                    w++;
                    glp_set_col_bnds(lp, w, GLP_LO, 0.0, 0.0); // note: the last argument is ignored
                    double costCoefficient =
                        m_costMatrixModel->data(m_costMatrixModel->index(
                            sTime.at(i)-1, dTime.at(j)-1
                        )).toDouble();
                    glp_set_obj_coef(lp, w, costCoefficient);
                }
            }
        }

        // Constraints
        glp_add_rows(lp, nS + nD);
        for (size_t i=0; i<nS; ++i)
            glp_set_row_bnds(lp, i+1, GLP_FX, s.at(i), 0); // note: the last argument is ignored
        for (size_t i=0; i<nD; ++i)
            glp_set_row_bnds(lp, i+nS+1, GLP_FX, d.at(i), 0); // note: the last argument is ignored

        // Coefficient matrix
        int iCoeff[1+1000], jCoeff[1+1000];
        double coeff[1+1000];
        {
            int w=0;
            for (size_t i=1; i<=nS; ++i)
            {
                for (size_t j=1; j<=nD; ++j)
                {
                    w++;
                    iCoeff[w] = i;
                    jCoeff[w] = (i-1)*nD + j;
                    coeff[w] = 1;
                }
            }
            for (size_t i=1; i<=nD; ++i)
            {
                for (size_t j=1; j<=nS; ++j)
                {
                    w++;
                    iCoeff[w] = i + nS;
                    jCoeff[w] = (j-1)*nD + i;
                    coeff[w] = 1;
                }
            }
            glp_load_matrix(lp, w, iCoeff, jCoeff, coeff);
        }

        // Solve        
        glp_simplex(lp, NULL); // "NULL" means "default control parameters"
        double z = glp_get_obj_val(lp);

        // Update movements table
        ui->movementsTable->clear();
        ui->movementsTable->setRowCount(nS);
        ui->movementsTable->setColumnCount(nD);
        QStringList verticalHeaderLabels;
        for (size_t i=0; i<nS; ++i)
            verticalHeaderLabels.append(QString("%1").arg(sTime.at(i)));
        ui->movementsTable->setVerticalHeaderLabels(verticalHeaderLabels);
        QStringList horizontalHeaderLabels;
        for (size_t i=0; i<nD; ++i)
            horizontalHeaderLabels.append(QString("%1").arg(dTime.at(i)));
        ui->movementsTable->setHorizontalHeaderLabels(horizontalHeaderLabels);
        for (size_t i=0; i<nS; ++i)
            ui->movementsTable->verticalHeaderItem(i)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        for (size_t i=0; i<nD; ++i)
            ui->movementsTable->horizontalHeaderItem(i)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        for (int i=0; i<ui->movementsTable->columnCount(); ++i)
            ui->movementsTable->setColumnWidth(i, 40);
        {
            int w = 0;
            for (size_t i=0; i<nS; ++i)
            {
                for (size_t j=0; j<nD; ++j)
                {
                    w++;
                    QString movement = QString("%1").arg(glp_get_col_prim(lp, w));
                    QTableWidgetItem *item = new QTableWidgetItem(movement);
                    item->setText(movement);
                    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                    ui->movementsTable->setItem(i, j, item);
                }
            }
        }

        // Absolute seasonality index
        absoluteSeasonalityIndex = z;

        // Relative seasonality index
        double sMax = (m_dataModel->total() / m_dataModel->rowCount()) * m_costMatrixModel->maxRowSum();
        relativeSeasonalityIndex = absoluteSeasonalityIndex / sMax;

        // Gini index
        gini = giniIndex();
    }

    // Output    
    ui->output->append(QString("Absolute seasonality index: %1").arg(absoluteSeasonalityIndex));
    ui->output->append(QString("Relative seasonality index: %1").arg(relativeSeasonalityIndex));
    ui->output->append(QString("Gini index: %1").arg(gini));
    ui->output->append("<p></p>");
}

void MainWindow::onSymmetricEditModeActionTriggered(bool mode)
{
    m_costMatrixModel->setSymmetricEditMode(mode);
}

void MainWindow::onOutputContextMenuRequested(QPoint point)
{
    // Output context menu
    QMenu *menu = ui->output->createStandardContextMenu();
    menu->addAction(ui->clearOutputAction);
    menu->exec(ui->output->mapToGlobal(point));
    delete menu;
}

void MainWindow::onClearOutputActionTriggered()
{
    ui->output->clear();
}

void MainWindow::onNewAction()
{
    NewDialog newDialog;
    if (newDialog.exec() == QDialog::Rejected)
        return;
    int k = newDialog.getLenght();
    m_dataModel->resetTo(k);
    m_costMatrixModel->resetTo(k, newDialog.getCostMatrixType());
    ui->costMatrixTable->setColumnsWidth(40);
}

void MainWindow::onImportDataActionTriggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Import data",
        QDir::homePath(),
        "Comma-separated values (*.csv)");

    if (fileName.isEmpty())
        return;

    // Open file
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Seasonal index calculator",
            "Error reading the file");
        return;
    }

    // Import data
    QList<double> data;
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        bool isConversionOk;
        double value = line.toDouble(&isConversionOk);
        if (isConversionOk)
            data << value;
        else
        {
            QMessageBox::critical(this, "Seasonal index calculator",
                "Error converting to double precision number");
            return;
        }

    }
    m_dataModel->resetTo(data.size());
    for (int i=0; i<data.size(); ++i)
        m_dataModel->setData(m_dataModel->index(i, 0), data.at(i));

    // TODO: verificare che il numero di righe non superi il numero massimo di righe
}

void MainWindow::onExportDataActionTriggered()
{
    // Show dialog
    ExportDataDialog exportDataDialog;
    if (exportDataDialog.exec() == QDialog::Rejected)
        return;

    // Open file for writing
    QFile file(exportDataDialog.getExportFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Seasonal index calculator", "Write error");
        return;
    }

    // Write file
    QTextStream out(&file);
    int k = m_dataModel->rowCount();
    QString separator = exportDataDialog.getOrientation() == Qt::Horizontal ?
        exportDataDialog.getSeparator() : "\r\n";
    for (int i=0; i<k; ++i)
    {
        if (i != 0)
            out << separator;
        out << m_dataModel->data(m_dataModel->index(i, 0)).toString();
    }
}

void MainWindow::onImportCostMatrixActionTriggered()
{
    int seasonPatternLength = m_dataModel->rowCount();

    // Show dialog
    ImportCostMatrixDialog importCostMatrixDialog(seasonPatternLength);
    if (importCostMatrixDialog.exec() == QDialog::Rejected)
        return;

    QList<QList<double> > costMatrix = importCostMatrixDialog.getData();

    for (int i=0; i<seasonPatternLength; ++i)
    {
        for (int j=0; j<seasonPatternLength; ++j)
            m_costMatrixModel->setData(m_costMatrixModel->index(i, j), costMatrix.at(i).at(j));
    }
}

void MainWindow::onExportCostMatrixActionTriggered()
{
    // Show dialog
    ExportCostMatrixDialog exportCostMatrixDialog;
    if (exportCostMatrixDialog.exec() == QDialog::Rejected)
        return;

    // Open file for writing
    QFile file(exportCostMatrixDialog.getExportFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Seasonal index calculator", "Write error");
        return;
    }

    // Separator
    QString separator = exportCostMatrixDialog.getSeparator();

    // Write file
    QTextStream out(&file);
    int k = m_costMatrixModel->rowCount();
    for (int i=0; i<k; ++i)
    {
        for (int j=0; j<k; ++j)
        {
            if (j != 0)
                out << separator;
            out << m_costMatrixModel->data(m_costMatrixModel->index(i, j)).toString();
        }
        if (i != k-1)
            out << "\r\n";
    }
}

void MainWindow::onExportProblemToGnuMathProgActionTriggered()
{
    // Supplies and demands
    QList<double> s, d;
    QList<int> sTime, dTime;
    QList<QPair<double, double> > data = m_dataModel->getData();
    int t=0;
    for (int i=0; i<data.size(); ++i)
    {
        t++;
        double deviation = data.at(i).second;
        if (deviation > 0)
        {
            s.append(deviation);
            sTime.append(t);
        }
        else if (deviation < 0)
        {
            d.append(-deviation);
            dTime.append(t);
        }
    }

    // Number of constraints
    int nS = s.size();
    int nD = d.size();

    // Check if problem can be set
    if (nS == 0 || nD == 0)
    {
        QMessageBox::warning(this, "SCalc",
            "Problem can not be set");
        return;
    }

    // File name
    QString fileName = QFileDialog::getSaveFileName(
        this, "Export problem", ".", "GNU MathProg (*.mod)");
    if (fileName.isEmpty())
        return;

    // Open file
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "SCalc", "Error opening file for writing");
        return;
    }

    // Write file
    QTextStream textStream(&file);
    textStream.setRealNumberPrecision(16);
    textStream
        << "set A;" << "\r\n"
        << "set B;" << "\r\n"
        << "\r\n"
        << "param a{i in A};" << "\r\n"
        << "param b{j in B};" << "\r\n"
        << "param c{i in A, j in B};" << "\r\n"
        << "\r\n"
        << "var x{i in A, j in B} >= 0;" << "\r\n"
        << "\r\n"
        << "minimize cost: sum{i in A, j in B} c[i,j] * x[i,j];" << "\r\n"
        << "s.t. surplus{i in A}: sum{j in B} x[i,j] = a[i];" << "\r\n"
        << "s.t. shortage{j in B}: sum{i in A} x[i,j] = b[j];" << "\r\n"
        << "\r\n"
        << "data;" << "\r\n"
        << "" << "\r\n";

    // Set A
    textStream << "set A :=";
    for (int i=0; i<nS; ++i)
        textStream << " " << sTime.at(i);
    textStream << ";" << "\r\n\r\n";

    // Set B
    textStream << "set B :=";
    for (int i=0; i<nD; ++i)
        textStream << " " << dTime.at(i);
    textStream << ";" << "\r\n\r\n";

    // Surpluses
    textStream << "param a :=";
    for (int i=0; i<nS; ++i)
        textStream << "\r\n" << sTime.at(i) << " " << s.at(i);
    textStream << ";" << "\r\n\r\n";

    // Shortages
    textStream << "param b :=";
    for (int i=0; i<nD; ++i)
        textStream << "\r\n" << dTime.at(i) << " " << d.at(i);
    textStream << ";" << "\r\n\r\n";

    // Costs
    textStream << "param c :";
    for (int i=0; i<nD; ++i)
        textStream << " " << dTime.at(i);
    textStream << " :=";
    for (int i=0; i<nS; ++i)
    {
        textStream << "\r\n" << sTime.at(i);
        for (int j=0; j<nD; ++j)
            textStream << " " <<  m_costMatrixModel->data(m_costMatrixModel->index(
                sTime.at(i)-1, dTime.at(j)-1)).toDouble();
    }
    textStream << ";\r\n\r\n";

    // End
    textStream << "end;";
}

void MainWindow::onAboutActionTriggered()
{
    AboutDialog aboutDialog;
    aboutDialog.exec();
}

double MainWindow::giniIndex()
{
    int n = m_dataModel->rowCount();

    // Numerator
    double numerator = 0;
    for (int i=0; i<n; ++i)
    {
        for (int j=0; j<n; ++j)
        {
            numerator += qAbs(
                m_dataModel->data(m_dataModel->index(i, 0)).toDouble()
                - m_dataModel->data(m_dataModel->index(j, 0)).toDouble());
        }
    }

    // Total
    double total = m_dataModel->total();

    if (total != 0)
        return numerator / (2*n*total);
    else
        return 0;
}
