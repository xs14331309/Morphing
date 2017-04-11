#ifndef POINT_H
#define POINT_H

// 3D vector
class Point {
public:
	Point() {}

	Point(double fx, double fy, double fz)
		:x(fx), y(fy), z(fz) {
	}

	// Subtract
	Point operator - (const Point& v) const {
		return Point(x - v.x, y - v.y, z - v.z);
	}

	// Dot product
	double Dot(const Point& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	// Cross product
	Point Cross(const Point& v) const {
		return Point(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x);
	}

public:
	double x, y, z;
};

// Determine whether two vectors v1 and v2 point to the same direction
// v1 = Cross(AB, AC)
// v2 = Cross(AB, AP)
bool SameSide(const Point &A, const Point &B, const Point &C, const Point &P);

// Same side method
// Determine whether point P in triangle ABC
bool PointinTriangle1(const Point &A, const Point &B, const Point &C, const Point &P);

#endif
