#ifndef OUTPUTTABLEWIDGET_H
#define OUTPUTTABLEWIDGET_H

#include <QTableWidget>
#include <QMenu>
#include <QContextMenuEvent>

class OutputTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit OutputTableWidget(QWidget *parent = 0);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    QMenu *contextMenu;
    QAction *copyAction;
    QAction *resizeColumnsToContentsAction;

private slots:
    void onCopyAction();
};

#endif // OUTPUTTABLEWIDGET_H
