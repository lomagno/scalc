#include "costmatrixdelegate.h"
#include <QDoubleSpinBox>

CostMatrixDelegate::CostMatrixDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *CostMatrixDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    Q_UNUSED(parent)
    Q_UNUSED(option)
    Q_UNUSED(index)
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setMinimum(0);
    editor->setMaximum(999999999);
    return editor;
}

void CostMatrixDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    double value = index.model()->data(index, Qt::EditRole).toDouble();
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}

void CostMatrixDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
    const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    double value = spinBox->value();
    model->setData(index, value, Qt::EditRole);
}

void CostMatrixDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}
