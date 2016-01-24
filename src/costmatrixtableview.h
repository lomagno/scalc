#ifndef COSTMATRIXTABLEVIEW_H
#define COSTMATRIXTABLEVIEW_H

#include <QTableView>
#include <QMenu>
#include <QContextMenuEvent>

class CostMatrixTableView : public QTableView
{
    Q_OBJECT
public:
    explicit CostMatrixTableView(QWidget *parent = 0);
    void setColumnsWidth(int width);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    QMenu *contextMenu;
    QAction *copyAction;
    QAction *pasteAction;
    void selectDataCellInNextRow();
    void keyPressEvent(QKeyEvent *keyEvent);

private slots:
    void onCopyAction();
    void onPasteAction();
};

#endif // COSTMATRIXTABLEVIEW_H
