#ifndef __COLOR_H__
#define __COLOR_H__
class Color3{
public:
	float r, g, b;
	Color3(){ r=0; g=0; b=0;}
	Color3(float R, float G, float B){
		r = R;
		g = G;
		b = B;
	}
	void normalize();
	bool over_max();
	friend Color3 operator+ (const Color3& u, const Color3& v);
	friend Color3 operator* (const Color3& u, float x);
	Color3& operator+= (const Color3& u);
	Color3& operator*= (float x);
};

Color3 operator+ (const Color3& u, const Color3& v) {
	return Color3(u.r + v.r, u.g + v.g, u.b + v.b);
}
Color3 operator* (const Color3& u, float x) {
	return Color3(u.r * x, u.g * x, u.b * x);
}
Color3& Color3::operator+= (const Color3& u){
	this->r += u.r;
	this->g += u.g;
	this->b += u.b;
}
Color3& Color3::operator*= (float x){
	this->r *= x;
	this->g *= x;
	this->b *= x;
}
void Color3::normalize(){
	this->r = this->r > 1 ? 1 : this->r;
	this->g = this->g > 1 ? 1 : this->g;
	this->b = this->b > 1 ? 1 : this->b;
}
bool Color3::over_max(){
	return (r > 1) && (g > 1) && (b > 1);
}

#endif

