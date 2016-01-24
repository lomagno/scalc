#ifndef COSTMATRIXDELEGATE_H
#define COSTMATRIXDELEGATE_H

#include <QStyledItemDelegate>

class CostMatrixDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    CostMatrixDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // COSTMATRIXDELEGATE_H
