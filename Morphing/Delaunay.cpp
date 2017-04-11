#include "Delaunay.h"
#include "Point.h"

Delaunay::Delaunay(Point p1, Point p2, Point p3, Point p4)
{
	m_Pts.resize(4);
	m_Pts[0] = p1;
	m_Pts[1] = p2;
	m_Pts[2] = p3;
	m_Pts[3] = p4;
	m_Edges.resize(4);
	Edge l1 = { 0,1,-1 };
	Edge l2 = { 1,2,-1 };
	Edge l3 = { 0,3,-1 };
	Edge l4 = { 2,3,-1 };
	m_Edges[0] = l1;
	m_Edges[1] = l2;
	m_Edges[2] = l3;
	m_Edges[3] = l4;
	MakeTriangle(0, 1, 2);
	MakeTriangle(0, 2, 3);
}

Delaunay::~Delaunay()
{
	m_Pts.resize(0);
	m_Edges.resize(0);
	m_Tris.resize(0);
}

void Delaunay::MakeTriangle(int n1, int n2, int n3)
{
	double x_centre, y_centre, radius;
	Cal_Centre(x_centre, y_centre, radius, n1, n2, n3);
	Triangle newTriangle = { { n1,n2,n3 },{ { n1,n2,1 },{ n2,n3,1 },{ n1,n3,1 } },x_centre,y_centre,radius, NULL, NULL };
	m_Tris.push_back(newTriangle);
	int EdgeSzie = (int)m_Edges.size();
	int flag;
	for (int i = 0; i<3; i++)
	{
		flag = 1;
		for (int j = 0; j<EdgeSzie; j++)
		{
			if (newTriangle.s[i].left == m_Edges[j].left&&newTriangle.s[i].right == m_Edges[j].right&&m_Edges[j].count != -1) { flag = 0; m_Edges[j].count += 1; break; }
			else if (newTriangle.s[i].left == m_Edges[j].left&&newTriangle.s[i].right == m_Edges[j].right&&m_Edges[j].count == -1) { flag = 0; break; }
		}
		if (flag == 1) m_Edges.push_back(newTriangle.s[i]);
	}
}

void Delaunay::Cal_Centre(double &x_centre, double &y_centre, double &radius, int n1, int n2, int n3)
{
	double x1, x2, x3, y1, y2, y3;
	x1 = m_Pts[n1].x;
	y1 = m_Pts[n1].y;
	x2 = m_Pts[n2].x;
	y2 = m_Pts[n2].y;
	x3 = m_Pts[n3].x;
	y3 = m_Pts[n3].y;
	x_centre = ((y2 - y1)*(y3*y3 - y1*y1 + x3*x3 - x1*x1) - (y3 - y1)*(y2*y2 - y1*y1 + x2*x2 - x1*x1)) / (2 * (x3 - x1)*(y2 - y1) - 2 * ((x2 - x1)*(y3 - y1)));
	y_centre = ((x2 - x1)*(x3*x3 - x1*x1 + y3*y3 - y1*y1) - (x3 - x1)*(x2*x2 - x1*x1 + y2*y2 - y1*y1)) / (2 * (y3 - y1)*(x2 - x1) - 2 * ((y2 - y1)*(x3 - x1)));
	radius = sqrt((x1 - x_centre)*(x1 - x_centre) + (y1 - y_centre)*(y1 - y_centre));
}

bool Delaunay::AddPoint(double xx, double yy, double zz)
{
	EdgeArray BoundEdges;
	Point newPoint(xx, yy, zz);
	m_Pts.push_back(newPoint);
	intArray badTriangle;
	int TriSize = (int)m_Tris.size();
	for (int i = 0; i<TriSize; i++)
	{
		if (inCircle(xx, yy, m_Tris[i]) == true) badTriangle.push_back(i);
	}
	for (int i = 0; i<(int)badTriangle.size(); i++)
	{
		DelTriangle(badTriangle[i], BoundEdges);
		for (int j = i + 1; j<(int)badTriangle.size(); j++) badTriangle[j] -= 1;
	}
	int PtSize = (int)m_Pts.size();
	for (int i = 0; i<(int)BoundEdges.size(); i++)
	{
		if (PtSize - 1<BoundEdges[i].left) MakeTriangle(PtSize - 1, BoundEdges[i].left, BoundEdges[i].right);
		else if (PtSize - 1>BoundEdges[i].left && PtSize - 1<BoundEdges[i].right) MakeTriangle(BoundEdges[i].left, PtSize - 1, BoundEdges[i].right);
		else MakeTriangle(BoundEdges[i].left, BoundEdges[i].right, PtSize - 1);
	}
	return true;
}

bool Delaunay::inCircle(double xx, double yy, Triangle currentTris)
{
	double dis = sqrt((currentTris.xc - xx)*(currentTris.xc - xx) + (currentTris.yc - yy)*(currentTris.yc - yy));
	if (dis>currentTris.r) return false;
	else return true;
}

void Delaunay::DelTriangle(int n, EdgeArray &BoundEdges)
{
	for (int i = 0; i<3; i++)
	{
		for (int j = 0; j<(int)m_Edges.size(); j++)
		{
			if (m_Edges[j].left == m_Tris[n].s[i].left&&m_Edges[j].right == m_Tris[n].s[i].right)
			{
				if (m_Edges[j].count == 2)
				{
					m_Edges[j].count = 1;
					BoundEdges.push_back(m_Edges[j]);
				}
				else if (m_Edges[j].count == -1) BoundEdges.push_back(m_Edges[j]);
				else if (m_Edges[j].count == 1)
				{
					for (int k = 0; k<(int)BoundEdges.size(); k++)
					{
						if (BoundEdges[k].left == m_Edges[j].left&&BoundEdges[k].right == m_Edges[j].right)
						{
							BoundEdges.erase(BoundEdges.begin() + k);
							break;
						}
					}
					m_Edges.erase(m_Edges.begin() + j);
					j--;
				}
				break;
			}
		}
	}
	m_Tris.erase(m_Tris.begin() + n);
}

void Delaunay::output()
{
	ofstream outfile("output.log");
	if (!outfile)
	{
		cout << "Unable to output nodes!";
		exit(1);
	}
	outfile << "/PREP7" << endl;
	for (int i = 0; i<(int)m_Pts.size(); i++)
	{
		outfile << "K," << i + 1 << "," << m_Pts[i].x << "," << m_Pts[i].y << "," << m_Pts[i].z << endl;
	}
	for (int i = 0; i<(int)m_Edges.size(); i++)
	{
		outfile << "L," << m_Edges[i].left + 1 << "," << m_Edges[i].right + 1 << endl;
	}
	outfile.close();
}


void Delaunay::Delete_Frame()
{
	EdgeArray BoundEdges;
	for (int i = 0; i<4; i++) m_Pts.erase(m_Pts.begin());
	for (int i = 0; i<(int)m_Tris.size(); i++)
	{
		if (m_Tris[i].v[0] == 0 || m_Tris[i].v[0] == 1 || m_Tris[i].v[0] == 2 || m_Tris[i].v[0] == 3)
		{
			DelTriangle(i, BoundEdges);
			BoundEdges.resize(0);
			i--;
		}
		else
		{
			for (int j = 0; j<3; j++)
			{
				m_Tris[i].v[j] -= 4;
				m_Tris[i].s[j].left -= 4;
				m_Tris[i].s[j].right -= 4;
			}
		}
	}
	for (int i = 0; i<4; i++) m_Edges.erase(m_Edges.begin());
	for (int i = 0; i<(int)m_Edges.size(); i++)
	{
		m_Edges[i].left -= 4;
		m_Edges[i].right -= 4;
	}
}

void Delaunay::Boundary_Recover(int fromPoint, int toPoint)
{
	EdgeArray BoundEdges;
	for (int i = 0; i<(int)m_Tris.size(); i++)
	{
		if (m_Tris[i].v[0] >= (fromPoint - 1) && m_Tris[i].v[2] <= (toPoint - 1))
		{
			DelTriangle(i, BoundEdges);
			BoundEdges.resize(0);
			i--;
		}
	}
}
