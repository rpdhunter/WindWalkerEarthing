#include "conductorpair.h"

ConductorPair::ConductorPair(Conductor c1, Conductor c2)
{
    this->c1 = c1;
    this->c2 = c2;
    l1 = c1.length();
    l2 = c2.length();
    myMode = Huzu;
}

ConductorPair::ConductorPair(Conductor c1)
{
    this->c1 = c1;
    l1 = c1.length();
    myMode = Zizu;
}

bool ConductorPair::operator ==(const ConductorPair &other) const
{
    if(myMode == Huzu){
        if(this->c1 == other.c1 && this->c2 == other.c2){
            return true;
        }
        else{
            return false;
        }
    }
    else{       //myMode == Zizu
        if(this->c1 == other.c1){
            return true;
        }
        else{
            return false;
        }
    }
}

bool ConductorPair::operator >(const ConductorPair &other) const
{
    if(myMode == Huzu){
        if(this->l1 > other.l1){
            return true;
        }
        else if(this->l1 < other.l1){
            return false;
        }
        else{      //this->l1 == other.l1
            if(this->l2 > other.l2){
                return true;
            }
            else if(this->l2 < other.l2){
                return false;
            }
            else{   //this->l2 == other.l2
                if(this->c1 > other.c1){
                    return true;
                }
                else if(this->c1 < other.c1){
                    return false;
                }
                else{       //this->c1 == other.c1
                    if(this->c2 > other.c2){
                        return true;
                    }
                    else if(this->c2 < other.c2){
                        return false;
                    }
                    else{   //this->c2 == other.c2
                        return false;
                    }
                }
            }
        }
    }
    else{       //myMode == Zizu
        if(this->l1 > other.l1){
            return true;
        }
        else if(this->l1 < other.l1){
            return false;
        }
        else{      //this->l1 == other.l1
            if(this->c1 > other.c1){
                return true;
            }
            else if(this->c1 < other.c1){
                return false;
            }
            else{       //this->c1 == other.c1
                return false;
            }
        }
    }
}

bool ConductorPair::operator <(const ConductorPair &other) const
{
    if(*this > other || *this == other){
        return false;
    }
    else{
        return true;
    }
}
