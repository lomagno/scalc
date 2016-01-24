#ifndef IMPORTCOSTMATRIXDIALOG_H
#define IMPORTCOSTMATRIXDIALOG_H

#include <QDialog>

namespace Ui {
class ImportCostMatrixDialog;
}

class ImportCostMatrixDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportCostMatrixDialog(int seasonalPatternLength, QWidget *parent = 0);
    ~ImportCostMatrixDialog();
    QList<QList<double> > getData();

public slots:
    void accept();

private:
    Ui::ImportCostMatrixDialog *ui;
    int m_seasonalPatternLength;
    QList<QList<double> > m_data;

private slots:
    void onChooseImportFileButtonClicked();
};

#endif // IMPORTCOSTMATRIXDIALOG_H
