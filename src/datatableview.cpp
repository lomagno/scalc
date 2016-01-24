#include "datatableview.h"
#include <QContextMenuEvent>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QDebug>

DataTableView::DataTableView(QWidget *parent) :
    QTableView(parent)
{        
    // Context menu
    contextMenu = new QMenu;
    copyAction = new QAction(QIcon(":/icons/img/edit-copy.png"), "Copy", this);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(onCopyAction()));
    contextMenu->addAction(copyAction);
    pasteAction = new QAction(QIcon(":/icons/img/edit-paste.png"), "Paste", this);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(onPasteAction()));
    contextMenu->addAction(pasteAction);
    contextMenu->addSeparator();
    contextMenu->addAction(
        QIcon(":/icons/img/autofit-width-16x16.png"),
        "Resize columns to contents",
        this, SLOT(resizeColumnsToContents()));

    // Delegate
    DataDelegate *dataDelegate = new DataDelegate;
    setItemDelegateForColumn(0, dataDelegate);
}


void DataTableView::contextMenuEvent(QContextMenuEvent *event)
{
    // Enable or disable copy action
    QItemSelection itemSelection = this->selectionModel()->selection();
    if (itemSelection.isEmpty())
        copyAction->setEnabled(false);
    else
        copyAction->setEnabled(true);

    // Enable or disable paste action
    if (currentIndex().isValid() && qApp->clipboard()->mimeData()->hasText())
    {
        if (currentIndex().column() == 0)
            pasteAction->setEnabled(true);
        else
            pasteAction->setEnabled(false);
    }
    else
        pasteAction->setEnabled(false);

    // Show context menu
    contextMenu->exec(mapToGlobal(event->pos()));
}

void DataTableView::onCopyAction()
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

void DataTableView::onPasteAction()
{
    // Check if clipboard has text
    if (qApp->clipboard()->mimeData()->hasText() == false)
        return;

    // Get text from clipboard
    QString text = qApp->clipboard()->text();
    QStringList splittedText = text.split(QRegExp("(\\r\\n)|(\\n)"));

    // Convert text to list of double precision numbers
    QList<double> data;
    int nTokens = splittedText.size();
    bool isConversionOk;
    for (int i=0; i<nTokens; ++i)
    {
        QString token = splittedText.at(i);

        // Check if last token is an empty string.
        // For example the last data copied from LibreOffice Calc is an empty string.
        if (token.isEmpty() && i==nTokens-1)
            break;

        double value = token.toDouble(&isConversionOk);
        if (!isConversionOk)
        {
            QMessageBox::warning(this, "SCalc",
                "Error converting clipboard data to double precision numbers");
            return;
        }
        data.append(value);
    }

    // Size of data
    int dataSize = data.size();

    // Check if there is enough space to paste data
    int startRow = currentIndex().row();
    int availableRows = model()->rowCount() - startRow;
    if (availableRows < dataSize)
    {
        QMessageBox::warning(this, "SCalc",
            "There is not enough space to paste data here");
        return;
    }

    // Paste data
    for (int i=0; i<dataSize; ++i)
        model()->setData(model()->index(startRow + i, 0), data.at(i));
}

void DataTableView::selectDataCellInNextRow()
{
    QModelIndex index = currentIndex();
    if (!index.isValid())
        return;
    int nextRow = index.row() + 1;
    if (nextRow < model()->rowCount())
        setCurrentIndex(model()->index(nextRow, 0));
}

void DataTableView::keyPressEvent(QKeyEvent *keyEvent)
{
    // TODO: gestire tasto enter su tastierino numerico
    if (keyEvent->key() == Qt::Key_Return)
        selectDataCellInNextRow();
    else
        QTableView::keyPressEvent(keyEvent);
}

