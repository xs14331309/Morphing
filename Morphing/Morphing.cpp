#include <iostream>
#include <fstream>

#include "Morphing.h"
#include "MyMatrix.h"
#include "Delaunay.h"
#include "dlib/image_processing/frontal_face_detector.h"
#include "dlib/image_processing/render_face_detections.h"
#include "dlib/image_processing.h"
#include "dlib/image_io.h"
#include "Point.h"

using namespace dlib;
using namespace std;

// filename:图片路径  out: 特征点保存文件
void wtFeaturesToFile(string filename, string out) {
	// 显示时为了突出效果
	const int pointWidth = 2;

	try {
		frontal_face_detector detector = get_frontal_face_detector(); // 人脸探测器
		CImg<unsigned char> src(filename.c_str());
		array2d<rgb_pixel> img;
		load_image(img, filename);

		// 注意dlib和std的vector用冲突，所以使用STL的vector时必须注明命名空间
		std::vector<rectangle> dets = detector(img);  // 返回探测到的所有人脸

		shape_predictor sp;
		deserialize("shape_predictor_68_face_landmarks.dat") >> sp;

		// 文件用于保存图片人脸特征点
		ofstream outfile(out);

		// 由于我们测试图片只有一张人脸，因此直接取dets[0]即可
		full_object_detection shape = sp(img, dets[0]); 
		for (int i = 0; i < shape.num_parts(); ++i) {
			auto a = shape.part(i);
			outfile << a.x() << " " << a.y() << endl;
			// 显示
			for (int j = a.y() - pointWidth; j != a.y() + pointWidth; j++) {
				for (int i = a.x() - pointWidth; i != a.x() + pointWidth; i++) {
					src(i, j, 0, 0) = 255;
					src(i, j, 0, 1) = 0;
					src(i, j, 0, 2) = 0;
				}
			}
		}
		outfile.flush();
		outfile.close();

		src.display();
		src.clear();
	}
	catch (exception e) {
	}
}


std::vector<Point> getFeaturePointsFromTxt(CImg<unsigned char> &img, string filename) {
	double width = img.width();
	double height = img.height();

	std::vector<Point> points;
	Point p[4];
	p[0] = { 0 ,0, 0 };
	p[1] = { width - 1, 0, 0 };
	p[2] = { width - 1,height - 1, 0 };
	p[3] = { 0 ,height - 1, 0 };
	points.push_back(p[0]);
	points.push_back(p[1]);
	points.push_back(p[2]);
	points.push_back(p[3]);

	fstream infile(filename);
	string temp;
	while (getline(infile, temp)) {
		if (!temp.empty()) {
			int index = temp.find(' ', 0);
			Point p(atof((temp.substr(0, index)).c_str()), atof((temp.substr(index + 1, temp.size())).c_str()), 0);
			points.push_back(p);
		}
	}
	infile.close();
	return points;
}

// -----------------------------------------------------------------------
Morphing::Morphing() {
}

Morphing::Morphing(string _path1, string _path2) : path1(_path1), path2(_path2) {
	img1.load(path1.c_str());
	img2.load(path2.c_str());
	img2.resize(img1);
}


void Morphing::getFeatrues() {
	if (path1.empty() || path2.empty())
		return;

	//wtFeaturesToFile(path1, pfile1);
	//wtFeaturesToFile(path2, pfile2);

    points1 = getFeaturePointsFromTxt(img1, pfile1);
	points2 = getFeaturePointsFromTxt(img2, pfile2);
}

void Morphing::doMorphing(float alpha, string outfile) {
	// 异常排查
	if (path1.empty() || path2.empty() || 
		points1.empty() || points2.empty() ||
		points1.size() != points2.size()) {
		cout << "error!" << endl;
		return;
	}

	// 生成中间图特征点
	std::vector<Point> points;
	CImg<unsigned char> img(img1.width(), img1.height(), 1, 3);
	img.fill(0);
	for (int i = 0; i < points1.size(); i++) {
		float x, y;
		x = (1 - alpha) * points1[i].x + alpha * points2[i].x;
		y = (1 - alpha) * points1[i].y + alpha * points2[i].y;
		Point temp = { x, y, 0 };
		points.push_back(temp);
	}

	// 三角剖分
	Delaunay delaunay = doTriangle(img, points);
	// 预先算出所有三角形的映射矩阵
	for (int i = 0; i < delaunay.m_Tris.size(); i++) {
		int* v = delaunay.m_Tris[i].v;
		double **A = makeAMatrix(points[v[0]], points[v[1]], points[v[2]]);
		double **invA = getInverse(A);
		double **U1 = makeAMatrix(points1[v[0]], points1[v[1]], points1[v[2]]);
		double **U2 = makeAMatrix(points2[v[0]], points2[v[1]], points2[v[2]]);
		delaunay.m_Tris[i].m1 = getMatrixProduct(U1, invA);
		delaunay.m_Tris[i].m2 = getMatrixProduct(U2, invA);
		deleteAMatrix(A);
		deleteAMatrix(invA);
		deleteAMatrix(U1);
		deleteAMatrix(U2);
	}

	cimg_forXY(img, x, y) {
		Point p(x, y, 0);
		//cout << x << " " << y << endl;
		for (int i = 0; i < delaunay.m_Tris.size(); i++) {
			int* v = delaunay.m_Tris[i].v;

			// 确定该点所在的三角形
			bool in = PointinTriangle1(points[v[0]], points[v[1]], points[v[2]], p);
			if (!in) continue;

			double **M1 = delaunay.m_Tris[i].m1;
			double **M2 = delaunay.m_Tris[i].m2;

			// 矩阵乘法
			double u1 = M1[0][0] * x + M1[0][1] * y + M1[0][2];
			double v1 = M1[1][0] * x + M1[1][1] * y + M1[1][2];
			Point *p1 = new Point(u1, v1, 0);
			unsigned char* rgb1 = BilinearFilter(img1, p1);
			double u2 = M2[0][0] * x + M2[0][1] * y + M2[0][2];
			double v2 = M2[1][0] * x + M2[1][1] * y + M2[1][2];
			Point *p2 = new Point(u2, v2, 0);

			unsigned char* rgb2 = BilinearFilter(img2, p2);
			for (int k = 0; k < 3; k++) {
				img(x, y, 0, k) = (1 - alpha) * rgb1[k] + alpha * rgb2[k];
			}

			delete p1, p2;
			delete[] rgb1, rgb2;
			break;
		}
	}

	img.display();
	img.save(outfile.c_str());
}

unsigned char* Morphing::BilinearFilter(CImg<unsigned char> &src, Point * p) {
	int xs = static_cast<int> (p->x);
	int xl = xs + 1 > src.width() ? xs + 1 : xs;
	int ys = static_cast<int> (p->y);
	int yl = ys + 1 > src.height() ? ys + 1 : ys;

	float ax = p->x - xs;
	float by = p->y - ys;

	float Si, Sj;
	unsigned char *rgb = new unsigned char[3];
	for (int i = 0; i < 3; i++) {
		Si = src(xs, ys, 0, i) + ax* (src(xs, yl, 0, i) - src(xs, ys, 0, i));
		Sj = src(xl, ys, 0, i) + ax* (src(xl, yl, 0, i) - src(xl, ys, 0, i));
		rgb[i] = static_cast<unsigned char>(Si + by*(Sj - Si));
	}
	//cout << (int)rgb[0] << (int)rgb[1] << (int)rgb[2] << endl;
	return rgb;
}

Delaunay Morphing::doTriangle(CImg<unsigned char> &img, std::vector<Point> points) {
	double width = img.width();
	double height = img.height();

	// 插入图片四角点
	Point p[4];
	p[0] = { 0 ,0, 0 };
	p[1] = { width - 1, 0, 0 };
	p[2] = { width - 1,height - 1, 0 };
	p[3] = { 0 ,height - 1, 0 };
	Delaunay MyMesh(p[0], p[1], p[2], p[3]);
	for (int i = 4; i < points.size(); i++) {
		MyMesh.AddPoint(points[i].x, points[i].y, points[i].z);
	}
	//MyMesh.Delete_Frame();


	const unsigned char color[3] = { 0, 255, 0 };
	for (int i = 0; i < MyMesh.m_Tris.size(); i++) {
		Triangle temp = MyMesh.m_Tris[i];
		Point p1 = MyMesh.m_Pts[temp.v[0]];
		Point p2 = MyMesh.m_Pts[temp.v[1]];
		Point p3 = MyMesh.m_Pts[temp.v[2]];
		img.draw_triangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, color, 1, ~0U);
	}
	img.display();
	return MyMesh;
}




