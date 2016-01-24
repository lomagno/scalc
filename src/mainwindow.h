#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "datamodel.h"
#include "costmatrixmodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    DataModel *m_dataModel;
    CostMatrixModel *m_costMatrixModel;
    QMenu *m_outputContextMenu;
    void updateStatusBar();    
    double giniIndex();

private slots:
    void onLoadDistanceMatrixActionTriggered();
    void onLoadUMatrixActionTriggered();
    void onDataChanged();
    void onCalculateActionTriggered();
    void onSymmetricEditModeActionTriggered(bool mode);
    void onOutputContextMenuRequested(QPoint point);
    void onClearOutputActionTriggered();
    void onNewAction();
    void onImportDataActionTriggered();
    void onExportDataActionTriggered();
    void onImportCostMatrixActionTriggered();
    void onExportCostMatrixActionTriggered();
    void onExportProblemToGnuMathProgActionTriggered();
    void onAboutActionTriggered();
};

#endif // MAINWINDOW_H
