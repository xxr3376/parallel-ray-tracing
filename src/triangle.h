#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include "pnt.h"
#include "line.h"

class Triangle {
public:
	Vector3 a, b, c;
	float x, y, z;

	Triangle (const Vector3& a, const Vector3& b, const Vector3& c);

	bool intersect(const Line3& line, Vector3& result) const;
private:
};

#endif //__TRIANGLE_H__
