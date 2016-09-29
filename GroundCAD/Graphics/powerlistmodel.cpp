#include "powerlistmodel.h"


PowerListModel::PowerListModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    connect(this,SIGNAL(doSomeChange()),this,SIGNAL(modelReset()));
}

int PowerListModel::rowCount(const QModelIndex &parent) const
{
    return powerList.count();
}

int PowerListModel::columnCount(const QModelIndex &parent) const
{
    return 6;
}

QVariant PowerListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();
    int row=index.row();
    int column=index.column();
    if(row>=powerList.count())
        return QVariant();

    switch(column){
    case 0:
        return powerList.at(row).P.x;
    case 1:
        return powerList.at(row).P.y;
    case 2:
        return powerList.at(row).P.z;
    case 3:
        return QString("%1∠%2°").arg(powerList.value(row).norm()).arg(powerList.value(row).arg());
    case 4:
        return powerList.at(row).f;
    case 5:
        return powerList.at(row).remarks;
    default:
        return QVariant();
    }
}

QVariant PowerListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role!=Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Vertical)
        return QVariant();
    switch(section){
    case 0:
        return "X";
    case 1:
        return "Y";
    case 2:
        return "Z";
    case 3:
        return tr("电流(A)");
    case 4:
        return tr("频率(Hz)");
    case 5:
        return tr("标识");
    default:
        return QVariant();
    }
}


void PowerListModel::modelInit(QList<CurrentSource> list)
{
    this->powerList = list;
    emit doSomeChange();
}
