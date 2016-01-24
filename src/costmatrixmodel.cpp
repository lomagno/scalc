#include "costmatrixmodel.h"

CostMatrixModel::CostMatrixModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    for (int i=1; i<=12; ++i)
    {
        QList<double> row;
        for (int j=1; j<=12; ++j)
            row.append(distance(12, i, j));
        m_data.append(row);
    }

    m_symmetricEditMode = false;
}

int CostMatrixModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.size();
}

int CostMatrixModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (m_data.size() > 0)
        return m_data.at(0).size();
    else
        return 0;
}

double CostMatrixModel::distance(int k, int i, int j)
{
    double absoluteDiff = qAbs(i-j);
    if (absoluteDiff <= k/2)
        return absoluteDiff;
    else
        return k - absoluteDiff;
}

QVariant CostMatrixModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole  || role == Qt::EditRole)
        return m_data.at(index.row()).at(index.column());
    else if (role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    else
        return QVariant();
}

QVariant CostMatrixModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)

    if (role == Qt::DisplayRole)
        return section+1;
    else if (role == Qt::TextAlignmentRole)
        return Qt::AlignRight;
    else
        return QVariant();
}

Qt::ItemFlags CostMatrixModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool CostMatrixModel::setData(const QModelIndex &index_, const QVariant &value, int role)
{
    if (index_.isValid() && role == Qt::EditRole)
    {
        // Conversion to double
        bool isConversionOk;
        double newValue = value.toDouble(&isConversionOk);
        if (!isConversionOk)
            return false;

        // Update data
        m_data[index_.row()][index_.column()] = newValue;
        if (m_symmetricEditMode)
            m_data[index_.column()][index_.row()] = newValue;

        // TODO: valutare se è necessario emettere il segnale per tutte le celle
        // Emit signal for data changed
        QModelIndex topLeftIndex = index(0,0);
        QModelIndex bottomRightIndex = index(rowCount()-1, rowCount()-1);
        emit dataChanged(topLeftIndex, bottomRightIndex);

        return true;
    }
    else
        return false;
}

bool CostMatrixModel::insertRows(int position, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)

    beginInsertRows(QModelIndex(), position, position+count-1);
    for (int row=0; row<count; ++row)
    {
        QList<double> newRow;
        for (int i=0; i<columnCount(); ++i)
            newRow.append(0);
        m_data.insert(position, newRow);
    }
    endInsertRows();

    // Emit signal for data changed
    QModelIndex topLeftIndex = index(0,0);
    QModelIndex bottomRightIndex = index(1, rowCount());
    emit dataChanged(topLeftIndex, bottomRightIndex);

    return true;
}

bool CostMatrixModel::insertColumns(int position, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)

    beginInsertColumns(QModelIndex(), position, position+count-1);
    for (int row=0; row<rowCount(); ++row)
    {
        for (int i=0; i<count; ++i)
            m_data[row].insert(position, 0);
    }
    endInsertColumns();

    // Emit signal for data changed
    QModelIndex topLeftIndex = index(0,0);
    QModelIndex bottomRightIndex = index(rowCount(), columnCount());
    emit dataChanged(topLeftIndex, bottomRightIndex);

    return true;
}

bool CostMatrixModel::removeRows(int position, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)

    beginRemoveRows(QModelIndex(), position, position+count-1);
    for (int row = 0; row < count; ++row)
        m_data.removeAt(position);
    endRemoveRows();

    return true;
}

bool CostMatrixModel::removeColumns(int position, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)

    beginRemoveColumns(QModelIndex(), position, position+count-1);
    for (int row=0; row<rowCount(); ++row)
    {
        for (int i=0; i<count; ++i)
            m_data[row].removeAt(position);
    }
    endRemoveColumns();

    return true;
}

void CostMatrixModel::loadDistanceMatrix()
{
    // Order of distance matrix
    int k = rowCount();

    // Update data
    for (int i=0; i<k; ++i)
        for (int j=0; j<k; ++j)
            m_data[i][j] = distance(k, i, j);

    // Emit signal for data changed
    QModelIndex topLeftIndex = index(0,0);
    QModelIndex bottomRightIndex = index(k-1, k-1);
    emit dataChanged(topLeftIndex, bottomRightIndex);
}

void CostMatrixModel::loadUMatrix()
{
    // Order of distance matrix
    int k = rowCount();

    // Update data
    for (int i=0; i<k; ++i)
        for (int j=0; j<k; ++j)
            m_data[i][j] = (i == j) ? 0 : 1;

    // Emit signal for data changed
    QModelIndex topLeftIndex = index(0,0);
    QModelIndex bottomRightIndex = index(k-1, k-1);
    emit dataChanged(topLeftIndex, bottomRightIndex);
}

void CostMatrixModel::resetTo(int size, Si::CostMatrixType costMatrixType)
{
    removeColumns(0, columnCount());
    removeRows(0, rowCount());
    insertRows(0, size);
    insertColumns(0, size);
    switch (costMatrixType)
    {
        case Si::DistanceMatrix:
            loadDistanceMatrix();
            break;
        case Si::UMatrix:
            loadUMatrix();
            break;
    }
}

void CostMatrixModel::setSymmetricEditMode(bool symmetricEditMode)
{
    m_symmetricEditMode = symmetricEditMode;
}

double CostMatrixModel::maxRowSum()
{
    int k = m_data.size();
    double max = rowSum(0, k);
    for (int i=1; i<k; ++i)
    {
        double sum = rowSum(i, k);
        if (sum > max)
            max = sum;
    }
    return max;
}

double CostMatrixModel::rowSum(int row, int columnCount)
{
    double sum = 0;
    for (int j=0; j<columnCount; ++j)
        sum += m_data.at(row).at(j);
    return sum;
}
