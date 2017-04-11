#ifndef DELAUNAY_H_INCLUDED  
#define DELAUNAY_H_INCLUDED  
#include <cstdlib>  
#include <iostream>  
#include <cstring>  
#include <string>  
#include <fstream>  
#include <math.h>  
#include <vector>
#include "Point.h"

using namespace std;
/*Bowyer-Watson算法
的基本步骤是：
1、构造一个超级三角形，包含所有散点，放入三角形链表。
2、将点集中的散点依次插入，在三角形链表中找出外接圆包含插入点的三角形（称为该点的影响三角形），删除影响三角形的公共边，将插入点同影响三角形的全部顶点连接起来，完成一个点在Delaunay三角形链表中的插入。
3、根据优化准则对局部新形成的三角形优化。将形成的三角形放入Delaunay三角形链表。
4、循环执行上述第2步，直到所有散点插入完毕。*/

typedef std::vector<Point> PointArray;//定义点类的vector容器  
 
typedef struct  
{  
    int left;  
    int right;  
    int count;//边的计数，如果计数为0，则删除此边  
}Edge;//定义边类  
typedef std::vector<Edge> EdgeArray;//定义边类的vector容器  
  
typedef struct  
{  
    int v[3];//三角形的三个顶点  
    Edge s[3];//三角形的三条边  
    double xc;//三角形外接圆圆心的x坐标  
    double yc;//三角形外接圆圆心的y坐标  
    double r;//三角形外接圆的半径
	double** m1; //到原图的映射矩阵
	double** m2; //到目标图的映射矩阵
}Triangle;//定义三角形类  
typedef std::vector<Triangle> TriangleArray;//定义三角形类的vector容器  
  
typedef std::vector<int> intArray;//定义int类的vector容器  
  
class Delaunay//定义Delaunay类  
{  
public:  
    Delaunay(Point p1,Point p2,Point p3,Point p4);//Delaunay类的构造函数，创建外边框  
    ~Delaunay();//Delaunay类的析构函数  
  
    bool AddPoint(double xx,double yy,double zz);//向已有剖分图形中加点的函数  
    void Delete_Frame();//删除外边框  
    void Boundary_Recover(int fromPoint,int toPoint);//边界恢复  
    void output();//输出ANSYS命令流文件  

	PointArray m_Pts;//m_Pts用于存储所有点  
	EdgeArray m_Edges;//m_Edges用于存储所有边  
	TriangleArray m_Tris;//m_Tris用于存储所有三角形

private:  
    void Cal_Centre(double &x_centre,double &y_centre,double &radius,int n1,int n2,int n3);//计算三角形的外接圆圆心坐标和半径  
    void MakeTriangle(int n1,int n2,int n3);//生成指定顶点的三角形  
    bool inCircle(double xx,double yy,Triangle currentTris);//判断点是否在圆内  
    void DelTriangle(int n,EdgeArray &BoundEdges);//删除指定的三角形  
  
  
};  
#endif // DELAUNAY_H_INCLUDED