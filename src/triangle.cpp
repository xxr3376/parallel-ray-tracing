#include "triangle.h"

#include <cmath>
#include "common.h"

Triangle::Triangle (const Vector3& a, const Vector3& b, const Vector3& c)
	: a(a), b(b), c(c){
	x = (a.x + b.x + c.x) / 3;
	y = (a.y + b.y + c.y) / 3;
	z = (a.z + b.z + c.z) / 3;
}

bool Triangle::intersect(const Line3& line, Vector3& result) const {
	Vector3 e1 = a - b, e2 = a - c, s = a - line.o;
	float t = det(line.d, e1, e2);
	if (fabs(t) < EPS) return false;

	float beta = det(line.d, s, e2) / t;
	if (beta < 0 || beta > 1) return false;
	float gamma = det(line.d, e1, s) / t;
	if (gamma < 0 || gamma > 1 || beta + gamma > 1) return false;
	if (det(s, e1, e2) / t <= 0) return false;

	result = a * (1 - beta - gamma) + b * beta + c * gamma;

	return true;
}