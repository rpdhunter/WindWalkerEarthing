#ifndef POWERLISTMODEL_H
#define POWERLISTMODEL_H

#include <QAbstractTableModel>
#include "Base/currentsource.h"

class PowerListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PowerListModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    
signals:
    void doSomeChange();

public slots:
    void modelInit(QList<CurrentSource> list);

private:
    QList<CurrentSource> powerList;     //电源数据
    
};

#endif // POWERLISTMODEL_H
