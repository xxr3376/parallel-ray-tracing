#ifndef __LINE_H__
#define __LINE_H__

#include "pnt.h"

class Line3 {
public:
	Vector3 o, d;

	Line3();
	Line3(const Vector3& o, const Vector3& d);
private:
};

#endif //__LINE_H__