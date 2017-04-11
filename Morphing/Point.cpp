#include "Point.h"


bool SameSide(const Point &A, const Point &B, const Point &C, const Point &P) {
	Point AB = B - A;
	Point AC = C - A;
	Point AP = P - A;

	Point v1 = AB.Cross(AC);
	Point v2 = AB.Cross(AP);

	// v1 and v2 should point to the same direction
	return v1.Dot(v2) >= 0;
}

// Same side method
// Determine whether point P in triangle ABC
bool PointinTriangle1(const Point &A, const Point &B, const Point &C, const Point &P) {
	return SameSide(A, B, C, P) &&
		SameSide(B, C, A, P) &&
		SameSide(C, A, B, P);
}