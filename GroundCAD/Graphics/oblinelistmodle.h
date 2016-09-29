#ifndef OBLINELISTMODLE_H
#define OBLINELISTMODLE_H

#include <QAbstractTableModel>
#include "Base/observedline.h"

class ObLineListModle : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ObLineListModle(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    
signals:
    void doSomeChange();

public slots:
    void modelInit(QList<ObservedLine> list);

private:
    QList<ObservedLine> obLineList;     //观测线数据
    
};

#endif // OBLINELISTMODLE_H
