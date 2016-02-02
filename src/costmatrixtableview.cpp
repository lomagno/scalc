#include "costmatrixtableview.h"
#include "costmatrixdelegate.h"
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QDebug>

CostMatrixTableView::CostMatrixTableView(QWidget *parent) :
    QTableView(parent)
{
    setItemDelegate(new CostMatrixDelegate(this));

    // Context menu
    contextMenu = new QMenu;
    copyAction = new QAction(QIcon(":/icons/img/edit-copy.png"), "Copy", this);
    copyAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    copyAction->setShortcutContext(Qt::WidgetShortcut);
    addAction(copyAction);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(onCopyAction()));
    contextMenu->addAction(copyAction);
    pasteAction = new QAction(QIcon(":/icons/img/edit-paste.png"), "Paste", this);
    pasteAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    pasteAction->setShortcutContext(Qt::WidgetShortcut);
    addAction(pasteAction);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(onPasteAction()));
    contextMenu->addAction(pasteAction);
    contextMenu->addSeparator();
    contextMenu->addAction(
        QIcon(":/icons/img/autofit-width-16x16.png"),
        "Resize columns to contents",
        this, SLOT(resizeColumnsToContents()));
}

void CostMatrixTableView::setColumnsWidth(int width)
{
    int rowCount = model()->rowCount();
    for (int i=0; i<rowCount; ++i)
        setColumnWidth(i, width);
}

void CostMatrixTableView::contextMenuEvent(QContextMenuEvent *event)
{
    // Enable or disable copy action
    QItemSelection itemSelection = this->selectionModel()->selection();
    if (itemSelection.isEmpty())
        copyAction->setEnabled(false);
    else
        copyAction->setEnabled(true);

    // Enable or disable paste action
    if (currentIndex().isValid() && qApp->clipboard()->mimeData()->hasText())
        pasteAction->setEnabled(true);
    else
        pasteAction->setEnabled(false);

    // Show context menu
    contextMenu->exec(mapToGlobal(event->pos()));
}

void CostMatrixTableView::selectDataCellInNextRow()
{
    QModelIndex index = currentIndex();
    if (!index.isValid())
        return;
    int currentColumn = index.column();
    int nextRow = index.row() + 1;
    if (nextRow < model()->rowCount())
        setCurrentIndex(model()->index(nextRow, currentColumn));
}

void CostMatrixTableView::keyPressEvent(QKeyEvent *keyEvent)
{
    // TODO: gestire tasto enter su tastierino numerico
    if (keyEvent->key() == Qt::Key_Return)
        selectDataCellInNextRow();
    else
        QTableView::keyPressEvent(keyEvent);
}

void CostMatrixTableView::onCopyAction()
{
    qDebug() << "onCopyAction()";

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

void CostMatrixTableView::onPasteAction()
{
    // Check if clipboard has text
    if (qApp->clipboard()->mimeData()->hasText() == false)
        return;

    // Get text from clipboard
    QString text = qApp->clipboard()->text();
    QStringList textRows = text.split(QRegExp("(\\r\\n)|(\\n)"));

    // Rows and columns count
    int nRows = textRows.size();
    int nColumns = textRows.at(0).split("\t").size();

    // Convert text to double precision numeric data
    QList<QList<double> > data;
    bool isConversionOk;
    for (int i=0; i<nRows; ++i)
    {
        QString textRow = textRows.at(i);

        // Check if last row is an empty string.
        // For example the last row copied from LibreOffice Calc is an empty string.
        if (textRow.isEmpty() && i==nRows-1)
            break;

        QStringList splittedTextRow = textRow.split("\t");

        // Check number of columns in the row
        if (splittedTextRow.size() != nColumns)
        {
            QMessageBox::warning(this, "SCalc",
                "There are rows with a different number of columns");
            return;
        }

        // Convert to double precision numbers
        QList<double> row;
        for (int j=0; j<nColumns; ++j)
        {
            double value = splittedTextRow.at(j).toDouble(&isConversionOk);
            if (!isConversionOk)
            {
                QMessageBox::warning(this, "SCalc",
                    "Error converting clipboard data to double precision numbers");
                return;
            }
            row.append(value);
        }
        data.append(row);
    }

    // Correct number of rows (this may be different from the first assigned value to nRows)
    nRows = data.size();

    // Check if there is enough space to paste data
    int startRow = currentIndex().row();
    int startColumn = currentIndex().column();
    int availableRows = model()->rowCount() - startRow;
    int availableColumns = model()->columnCount() - startColumn;
    if (availableRows < nRows || availableColumns < nColumns)
    {
        QMessageBox::warning(this, "SCalc",
            "There is not enough space to paste data here");
        return;
    }

    // Paste data
    for (int i=0; i<nRows; ++i)
        for (int j=0; j<nColumns; ++j)
            model()->setData(model()->index(startRow + i, startColumn + j), data.at(i).at(j));
}

