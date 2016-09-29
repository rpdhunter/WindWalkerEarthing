#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QAbstractTableModel>
#include "Base/conductor.h"

class DataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DataModel( QObject *parent = 0);


    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    
signals:
    void doSomeChange();
public slots:
    void modelInit(QList<Conductor> list);

private:
    QList<Conductor> dataList;
    
};

#endif // DATAMODEL_H
