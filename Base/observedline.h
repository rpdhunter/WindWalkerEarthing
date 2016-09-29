#ifndef OBSERVEDLINE_H
#define OBSERVEDLINE_H

#include "point.h"
#include <QString>

//观测线类
class ObservedLine
{
public:
    ObservedLine();

    void setPoints(Point p1,Point p2);
    Point P1() const;
    Point P2() const;

    double length() const;

    double gap() const;
    void setGap(double g);
    int subsection() const;
    void setSubsection(int n);
    QString name() const;
    void setName(QString name);

    bool isValid();     //返回观测线是否可用

private:
    Point p1,p2;
    double _gap;         //观测线上点与点之间的间隔
    int _subsection;     //分段数，subsection = this->length() / gap
    QString _name;       //观测线名称
};

#endif // OBSERVEDLINE_H
