#ifndef __TRIANGLE__
#define __TRIANGLE__

#include "pnt.h"

class Triangle {
public:
	Vector3 a, b, c;
	float x, y, z;

	Triangle (const Vector3& a, const Vector3& b, const Vector3& c);
private:
};

#endif //__TRIANGLE__
