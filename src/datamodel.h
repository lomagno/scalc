#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QAbstractTableModel>
#include <QPair>

class DataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DataModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool insertRows(int position, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int position, int count, const QModelIndex &parent = QModelIndex());
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData (const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    void resetTo(int length);

    QList<QPair<double, double> > getData() {return m_data;}

    // Statistics
    double total() {return m_total;}
    double mean() {return m_mean;}
    double min() {return m_min;}
    double max() {return m_max;}
    double range() {return m_range;}
    double highCount() {return m_highCount;}
    double lowCount() {return m_lowCount;}


private:

    // Data
    QList<QPair<double, double> > m_data;
    double m_total;
    double m_mean;
    double m_min;
    double m_max;
    double m_range;
    double m_highCount;
    double m_lowCount;    

    void updateStatistics();
};

#endif // DATAMODEL_H
