#ifndef DATATABLEVIEW_H
#define DATATABLEVIEW_H

#include <QTableView>
#include <QMenu>
#include "datadelegate.h"

class DataTableView : public QTableView
{
    Q_OBJECT
public:
    explicit DataTableView(QWidget *parent = 0);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    QMenu *contextMenu;
    QAction *copyAction;
    QAction *pasteAction;
    void keyPressEvent(QKeyEvent *keyEvent);
    void selectDataCellInNextRow();

private slots:
    void onCopyAction();
    void onPasteAction();
};

#endif // DATATABLEVIEW_H
