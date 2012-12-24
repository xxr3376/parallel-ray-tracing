#include "triangle.h"

Triangle::Triangle (const Vector3& a, const Vector3& b, const Vector3& c)
	: a(a), b(b), c(c){
	x = (a.x + b.x + c.x) / 3;
	y = (a.y + b.y + c.y) / 3;
	z = (a.z + b.z + c.z) / 3;
}
