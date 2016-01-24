#ifndef COSTMATRIXMODEL_H
#define COSTMATRIXMODEL_H

#include <QAbstractTableModel>
#include "common.h"

class CostMatrixModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit CostMatrixModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData (const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool insertRows(int position, int count, const QModelIndex &parent = QModelIndex());
    bool insertColumns(int position, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int position, int count, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int position, int count, const QModelIndex &parent = QModelIndex());
    void loadDistanceMatrix();
    void loadUMatrix();
    void resetTo(int size, Si::CostMatrixType costMatrixType);
    void setSymmetricEditMode(bool symmetricEditMode);
    double maxRowSum();

private:
    // Data
    QList<QList<double> > m_data;

    bool m_symmetricEditMode;

    double distance(int k, int i, int j);
    double rowSum(int row, int columnCount);
};

#endif // COSTMATRIXMODEL_H
