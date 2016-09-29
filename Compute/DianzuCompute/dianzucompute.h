#ifndef DIANZUCOMPUTE_H
#define DIANZUCOMPUTE_H

class Conductor;
class Point;

class DianZuCompute
{
public:
    DianZuCompute();

    //几何法
    static double HuZu_JiHe_Totle(Conductor c1, Conductor c2, double rou);  //全空间几何法计算
    static double HuZu_JiHe_Half(Conductor c1,Conductor c2, double rou);   //半空间几何法计算（可直接使用）
    static double ZiZu_JiHe_Totle(Conductor c1, double rou);   //全空间几何法计算
    static double ZiZu_JiHe_Half(Conductor c1, double rou);   //半空间几何法计算（可直接使用）

    //积分法
    static double HuZu_JiFen_Totle(Conductor c1, Conductor c2, double rou, int n);   //全空间积分法计算
    static double HuZu_JiFen_Half(Conductor c1, Conductor c2, double rou, int n);   //半空间积分法计算,n点采样（可直接使用）
    static double ZiZu_JiFen_Totle(Conductor c1, double rou, int n);   //全空间积分法计算
    static double ZiZu_JiFen_Half(Conductor c1, double rou, int n);    //半空间积分法计算（可直接使用）

    //Dawalibi法
    static double HuZu_Dawalibi_Totle(Conductor c1, Conductor c2, double rou, int n);   //全空间Dawalibi法计算
    static double HuZu_Dawalibi_Half(Conductor c1, Conductor c2, double rou, int n);   //半空间Dawalibi法计算,n点采样（可直接使用）
    static double ZiZu_Dawalibi_Totle(Conductor c1, double rou, int n);   //全空间Dawalibi法计算
    static double ZiZu_Dawalibi_Half(Conductor c1, double rou, int n);    //半空间Dawalibi法计算（可直接使用）

    //Kouteynikoff法(主要使用了Kouteynikoff的近距离点位公式)
    static double HuZu_Kouteynikoff_Totle(Conductor c1, Conductor c2, double rou, int n);   //全空间Kouteynikoff计算
    static double HuZu_Kouteynikoff_Half(Conductor c1, Conductor c2, double rou, int n);   //半空间Kouteynikoff计算,n点采样（可直接使用）
    static double ZiZu_Kouteynikoff_Totle(Conductor c1, double rou, int n);   //全空间Kouteynikoff计算
    static double ZiZu_Kouteynikoff_Half(Conductor c1, double rou, int n);    //半空间Kouteynikoff计算（可直接使用）

    //二次分域法(应当只有半空间自阻公式可以用，其他的都没有定义)
//    static double HuZu_SecSubdivision_Totle(Conductor c1, Conductor c2, double rou, int n);   //全空间二次分域计算
//    static double HuZu_SecSubdivision_Half(Conductor c1, Conductor c2, double rou, int n);   //半空间二次分域计算,n点采样（可直接使用）
//    static double ZiZu_SecSubdivision_Totle(Conductor c1, double rou, int n);   //全空间二次分域计算
    static double ZiZu_SecSubdivision_Half(Conductor c1, double rou, int n);    //半空间二次分域计算（可直接使用）


    static double HuZu_JiFen_Half_V3(Conductor c1,Conductor c2, double rou);   //半空间积分法法计算，三点采样（可直接使用）

    static double HuZu_JiFen_Totle_v2(Conductor c1, Conductor c2, double rou, int n);   //全空间积分法计算（陈先璐）
    static double Segment_Point_Potential_v2(Conductor c1, Point P, double rou);   //单根导体在某点产生的电位

private:
    static void Conductor_YuChuLi(Conductor &c1, Conductor &c2);      //预处理导体



};

#endif // DIANZUCOMPUTE_H
