#include "datamodel.h"

DataModel::DataModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    for (int i=0; i<12; ++i)
        m_data.append(QPair<double, double>(0, 0));
    updateStatistics();    
}

int DataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.size();
}

int DataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        int row = index.row();
        switch (index.column())
        {
            case 0:
                return m_data.at(row).first;
            case 1:
                return m_data.at(row).second;
            default:
                return QVariant();
        }
    }
    else if (role == Qt::TextColorRole)
    {
        int row = index.row();
        switch (index.column())
        {
            case 0:
                return Qt::black;
            case 1:
                if (m_data.at(row).second > 0)
                    return Qt::red;
                else if (m_data.at(row).second < 0)
                    return Qt::green;
                else
                    return QVariant();
            default:
                return QVariant();
        }
    }
    else if (role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    else
        return QVariant();
}

QVariant DataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
                case 0:
                    return "Value";
                case 1:
                    return "Deviation";
                default:
                    return QVariant();
            }
        }
        else if (orientation == Qt::Vertical)
            return section+1;
        else
            return QVariant();
    }
    else if (role == Qt::TextAlignmentRole)
        return Qt::AlignRight;
    else
        return QVariant();
}

bool DataModel::insertRows(int position, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)
    beginInsertRows(QModelIndex(), position, position+count-1);

    for (int row = 0; row < count; ++row)
        m_data.insert(position, QPair<double, double>(0, 0));
    updateStatistics();

    endInsertRows();

    // Emit signal for data changed
    QModelIndex topLeftIndex = index(0,0);
    QModelIndex bottomRightIndex = index(1, rowCount());
    emit dataChanged(topLeftIndex, bottomRightIndex);

    return true;
}

bool DataModel::removeRows(int position, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)
    beginRemoveRows(QModelIndex(), position, position+count-1);

    for (int row = 0; row < count; ++row)
        m_data.removeAt(position);

    endRemoveRows();
    return true;
}

Qt::ItemFlags DataModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (!index.isValid())
        return defaultFlags;

    switch (index.column())
    {
        case 0:
            return Qt::ItemIsEditable | defaultFlags;
        case 1:
            return defaultFlags;
    }

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool DataModel::setData (const QModelIndex &index_, const QVariant &value, int role)
{
    if (index_.isValid() && role == Qt::EditRole)
    {
        // Conversion to double
        bool isConversionOk;
        double newValue = value.toDouble(&isConversionOk);
        if (!isConversionOk)
            return false;

        // Update data
        m_data[index_.row()].first = newValue;
        updateStatistics();

        // Emit signal for data changed
        QModelIndex topLeftIndex = index(0,0);
        QModelIndex bottomRightIndex = index(1, rowCount());
        emit dataChanged(topLeftIndex, bottomRightIndex);

        return true;
    }
    else
        return false;
}

void DataModel::resetTo(int length)
{
    removeRows(0, rowCount());
    insertRows(0, length);
}

void DataModel::updateStatistics()
{
    // Total, min, max
    m_total = 0;
    m_min = m_data[0].first;
    m_max = m_data[0].first;
    for (int i=0; i<rowCount(); ++i)
    {
        double value = m_data[i].first;
        m_total += value;
        if (value < m_min)
            m_min = value;
        if (value > m_max)
            m_max = value;
    }

    // Range
    m_range = m_max - m_min;

    // Mean
    m_mean = m_total / rowCount();

    // Deviations
    for (int i=0; i<rowCount(); ++i)
        m_data[i].second = m_data[i].first - m_mean;

    // High count and low count
    m_highCount = 0;
    m_lowCount = 0;
    for (int i=0; i<rowCount(); ++i)
    {
        if (m_data[i].second > 0)
            m_highCount++;
        else if (m_data[i].second < 0)
            m_lowCount++;
    }
}
