#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "line.h"
#include "pnt.h"
#include "common.h"


class Camera{
private:
	Vector3 startPoint;
	Vector3 dX;
	Vector3 dY;
	Vector3 dZ;
	float focalLength;
	int width;
	int height;
public:
	Camera(const Vector3 start, const Vector3 directX, const Vector3 directY, const int w, const int h);
	Line3 getSight(int x, int y) const;
	void setFocalLen(float len);
	void rotate(float x, float y);
	void move(float x, float y);
	void print();
private:
	void _rotate(Vector3& target, float x, float y, float z); 
};

#endif
