#ifndef EXPORTCOSTMATRIXDIALOG_H
#define EXPORTCOSTMATRIXDIALOG_H

#include <QDialog>

namespace Ui {
class ExportCostMatrixDialog;
}

class ExportCostMatrixDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportCostMatrixDialog(QWidget *parent = 0);
    ~ExportCostMatrixDialog();
    QString getExportFilePath();
    QString getSeparator();

public slots:
    void accept();

private:
    Ui::ExportCostMatrixDialog *ui;

private slots:
    void onChooseFilePath();
};

#endif // EXPORTCOSTMATRIXDIALOG_H
