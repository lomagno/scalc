#ifndef EXPORTDATADIALOG_H
#define EXPORTDATADIALOG_H

#include <QDialog>

namespace Ui {
class ExportDataDialog;
}

class ExportDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDataDialog(QWidget *parent = 0);
    ~ExportDataDialog();
    QString getExportFilePath();
    Qt::Orientation getOrientation();
    QString getSeparator();

public slots:
    void accept();

private:
    Ui::ExportDataDialog *ui;

private slots:
    void onChooseFilePath();
    void onOrientationChanged(int currentIndex);
};

#endif // EXPORTDATADIALOG_H
