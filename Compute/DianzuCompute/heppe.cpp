#include "heppe.h"
#include "Base/compute.h"
#include "Base/point.h"
#include "math.h"
#include "qmath.h"

#include <QtDebug>

#include "armadillo"

using namespace arma;

const double pi = 3.1415926;        //圆周率
const double MINVALUE = 0.00000001;   //最小值（用于比较实数和零，如果小于这个数明则认为相等）
const double u0 = 4*pi*1e-7;        //磁导率


Heppe::Heppe(Conductor c)
    :JiFenBase(c)
{
}

//几何法求解均匀介质中两导体的互阻系数
//其中rou为土壤电阻率
double Heppe::huzu(Conductor c2)
{
    Compute::Relation aaaaa = Compute::relationship(c1,c2);     //判断线段之间的关系

    Point A(c1.x0,c1.y0,c1.z0);
    Point B(c1.x1,c1.y1,c1.z1);
    Point E(c2.x0,c2.y0,c2.z0);
    Point F(c2.x1,c2.y1,c2.z1);

    Point _A = Point::Shadow(A,E,F);
    Point _B = Point::Shadow(B,E,F);
    Point _E = Point::Shadow(E,A,B);
    Point _F = Point::Shadow(F,A,B);

    double AB = Point::length(A,B);
    double EF = Point::length(E,F);
    double AE = Point::length(A,E);
    double AF = Point::length(A,F);
    double BE = Point::length(B,E);
    double BF = Point::length(B,F);


    double _BF = Point::_length(_B,F,E,F);
    double _BE = Point::_length(_B,E,E,F);
    double _AF = Point::_length(_A,F,E,F);
    double _AE = Point::_length(_A,E,E,F);
    double _FB = Point::_length(_F,B,A,B);
    double _FA = Point::_length(_F,A,A,B);
    double _EB = Point::_length(_E,B,A,B);
    double _EA = Point::_length(_E,A,A,B);

    double CB,CA,GF,GE,CG;

    double KK;

    double jiaodu = Compute::jiaoDu(c1,c2);

    if(aaaaa == Compute::collinear || aaaaa == Compute::parallel || qAbs(jiaodu) < MINVALUE){ //平行共线或近似平行
        Point C = A;
        Point G = _A;

        CB = Point::_length(C,B,A,B);
        CA = Point::_length(C,A,A,B);
        GF = Point::_length(G,F,E,F);
        GE = Point::_length(G,E,E,F);
        CG = Point::length(C,G);

        double l1 = A.x-B.x;
        double m1 = A.y-B.y;
        double n1 = A.z-B.z;
        double l2 = E.x-F.x;
        double m2 = E.y-F.y;
        double n2 = E.z-F.z;
        if(l1*l2>=0 && m1*m2>=0 && n1*n2>=0){   //判断两线段是否同向

            KK = BE +AF - BF -AE;

        }
        else{
            KK = -(BE +AF - BF -AE);
        }
    }
    else{   //相交或异面
        Conductor c3 = Compute::getCrossPoint(c1,c2);

        Point C(c3.x0,c3.y0,c3.z0);
        Point G(c3.x1,c3.y1,c3.z1);

        CB = Point::_length(C,B,A,B);
        CA = Point::_length(C,A,A,B);
        GF = Point::_length(G,F,E,F);
        GE = Point::_length(G,E,E,F);
        CG = Point::length(C,G);

        if(qAbs(CG) < MINVALUE ){   //此种情况可认为两线段（延长线）相交
            KK = 0;
        }
        else{                       //此种情况就是异面了，异面时BF,BE,AF,AE理论上应该都不为零（或近似为零）
            double Omega = atan(CG/BF/tan(jiaodu) + CB*GF*sin(jiaodu)/CG/BF)
                    -atan(CG/BE/tan(jiaodu) + CB*GE*sin(jiaodu)/CG/BE)
                    -atan(CG/AF/tan(jiaodu) + CA*GF*sin(jiaodu)/CG/AF)
                    +atan(CG/AE/tan(jiaodu) + CA*GE*sin(jiaodu)/CG/AE);

            KK = CG*Omega/sin(jiaodu);
        }
    }

    double AA,BB,CC,DD;
    if(qAbs(CB) < MINVALUE || qAbs(BE+_BE) < MINVALUE)
        AA = 0;
    else
        AA = CB*log((BF+_BF)/(BE+_BE));
    if(qAbs(CA) < MINVALUE || qAbs(AE+_AE) < MINVALUE)
        BB = 0;
    else
        BB = CA*log((AF+_AF)/(AE+_AE));
    if(qAbs(GF) < MINVALUE || qAbs(AF+_FA) < MINVALUE)
        CC = 0;
    else
        CC = GF*log((BF+_FB)/(AF+_FA));
    if(qAbs(GE) < MINVALUE || qAbs(AE+_EA) < MINVALUE)
        DD = 0;
    else
        DD = GE*log((BE+_EB)/(AE+_EA));

    double M = AA - BB + CC - DD -KK;

    return M /4 /pi /AB /EF;

}

double Heppe::zizu()
{
    double a = c1.r;       //导体半径

    double l = c1.length(); //导体长度

    double M = 2*l*log(a/(qSqrt(l*l+a*a)-l)) + 2*(a-qSqrt(l*l+a*a));
    return M /(4 * l * l *pi);
}

double Heppe::hugan(Conductor c2)
{
    double jiaodu = Compute::jiaoDu(c1,c2);
    double l1 = c1.length();
    double l2 = c1.length();

    return huzu(c2)*u0*l1*l2*qAbs(cos(jiaodu));
}


double Heppe::waizigan()
{
    double a = c1.r;       //导体半径

    double l = c1.length(); //导体长度

    double M = 2*l*log(a/(qSqrt(l*l+a*a)-l)) + 2*(a-qSqrt(l*l+a*a));
    return M*u0 /(4 *pi);
}
