#include "observedline.h"
#include <qmath.h>

#include "point.h"

ObservedLine::ObservedLine()
{
    this->_gap =0;
    this->_subsection=0;
    this->p1 = Point(0,0,0);
    this->p2 = Point(0,0,0);
}

void ObservedLine::setPoints(Point p1, Point p2)
{
    this->p1 = p1;
    this->p2 = p2;
}

Point ObservedLine::P1() const
{
    return p1;
}

Point ObservedLine::P2() const
{
    return p2;
}

double ObservedLine::length() const
{
    return qSqrt(qPow((p1.x-p2.x),2)+qPow((p1.y-p2.y),2)+qPow((p1.z-p2.z),2));
}

double ObservedLine::gap() const
{
    return this->_gap;
}

//为了使设置的gap值可以平分整个观测线，需要做一个小小的修正
void ObservedLine::setGap(double g)
{
    if(g == 0){
        return;
    }
    this->_subsection = this->length() / g;
    this->_gap = this->length() / this->_subsection;
}

int ObservedLine::subsection() const
{
    return this->_subsection;
}

void ObservedLine::setSubsection(int n)
{
    if(n == 0)
        return;
    this->_subsection = n;
    this->_gap = this->length() / n;
}

QString ObservedLine::name() const
{
    return this->_name;
}

void ObservedLine::setName(QString name)
{
    this->_name = name;
}

bool ObservedLine::isValid()
{
    if(this->_gap >0 && this->_subsection >0 && this->length() >0)
        return true;
    else
        return false;
}
