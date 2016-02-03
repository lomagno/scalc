#include "outputtablewidget.h"
#include <QApplication>
#include <QClipboard>

OutputTableWidget::OutputTableWidget(QWidget *parent) :
    QTableWidget(parent)
{    
    // Context menu
    contextMenu = new QMenu;
    copyAction = new QAction(QIcon(":/icons/img/edit-copy.png"), "Copy", this);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(onCopyAction()));
    copyAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    copyAction->setShortcutContext(Qt::WidgetShortcut);
    addAction(copyAction);
    contextMenu->addAction(copyAction);
    contextMenu->addSeparator();
    resizeColumnsToContentsAction = new QAction(
        QIcon(":/icons/img/autofit-width.png"), "Resize columns to contents", this);
    contextMenu->addAction(resizeColumnsToContentsAction);
    connect(resizeColumnsToContentsAction, SIGNAL(triggered()), this, SLOT(resizeColumnsToContents()));
}

void OutputTableWidget::contextMenuEvent(QContextMenuEvent *event)
{
    // Enable or disable copy action
    QItemSelection itemSelection = this->selectionModel()->selection();
    if (itemSelection.isEmpty())
        copyAction->setEnabled(false);
    else
        copyAction->setEnabled(true);

    // Enable or disable resize columns to contents action
    if (rowCount() == 0 || columnCount() == 0)
        resizeColumnsToContentsAction->setEnabled(false);
    else
        resizeColumnsToContentsAction->setEnabled(true);

    // Show context menu
    contextMenu->exec(mapToGlobal(event->pos()));
}

void OutputTableWidget::onCopyAction()
{
    // Selection
    QItemSelection itemSelection = this->selectionModel()->selection();
    if (itemSelection.isEmpty())
        return;
    QItemSelectionRange itemSelectionRange = this->selectionModel()->selection().first();

    int selectionTop = itemSelectionRange.top();
    int selectionLeft = itemSelectionRange.left();
    int selectionBottom = itemSelectionRange.bottom();
    int selectionRight = itemSelectionRange.right();

    // Copy text
    QString copyText;
    for (int i=selectionTop; i<=selectionBottom; ++i)
    {
        if (i != selectionTop)
            copyText += "\r\n";
        for (int j=selectionLeft; j<=selectionRight; ++j)
        {
            if (j != selectionLeft)
                copyText += "\t";
            copyText += model()->data(model()->index(i, j), Qt::EditRole).toString();
        }
    }

    // Copy to clipboard
    qApp->clipboard()->setText(copyText);
}
