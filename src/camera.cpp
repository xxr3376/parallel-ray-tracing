#include "camera.h"
#include <cmath>
#include <cstdio>

Camera::Camera(const Vector3 start, const Vector3 directX, const Vector3 directY, const int w, const int h):
	startPoint(start), dX(directX), dY(directY), dZ(0, 0, 0), width(w), height(h)
{
	dX.normalize();
	dY.normalize();
	dZ = det(dX, dY);
	dZ.normalize();
	dX = dX * 0.01;
	dY = dY * 0.01;
	focalLength = 100;
}
Line3 Camera::getSight(int x, int y) const{
	Vector3 destX = dX * float(x - this->width / 2);
	Vector3 destY = dY * float(y - this->height / 2);
	Vector3 destZ = dZ * focalLength;
	Vector3 to = destX + destY + destZ;
	return Line3(startPoint, to);
}
void Camera::setFocalLen(float len){
	focalLength = len;
}
void Camera::rotate(float x, float y){
	this->_rotate(dX, y, x, 0);
	this->_rotate(dY, y, x, 0);
	this->_rotate(dZ, y, x, 0);
}
void Camera::_rotate(Vector3& target, float x, float y, float z){
	Vector3 tmp = target;
	target.x = cos(y)*cos(z) * tmp.x + cos(y)*sin(z) * tmp.y - sin(y) * tmp.z;
	target.y = (sin(x)*sin(y)*cos(z)-cos(x)*sin(z)) * tmp.x +
				(sin(x)*sin(y)*sin(z)+cos(x)*cos(z)) * tmp.y +
				sin(x)*cos(y) * tmp.z;
	target.z = (cos(x)*sin(y)*cos(z)+sin(x)*sin(z)) * tmp.x +
				(cos(x)*sin(y)*sin(z)-sin(x)*cos(z)) * tmp.y +
				cos(x)*cos(y) * tmp.z;
}

