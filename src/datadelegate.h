#ifndef DATADELEGATE_H
#define DATADELEGATE_H

#include <QStyledItemDelegate>

class DataDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    DataDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // DATADELEGATE_H
