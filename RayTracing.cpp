// RayTracing.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#define EPSILON 0.00001f
#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>
using namespace std;
using namespace cv;


struct UGColor{
	double r, g, b;
	UGColor(double p1, double p2, double p3): r(p1), g(p2), b(p3){}
	void check()
	{
		if (r > 255)
			r = 255;
		if (g > 255)
			g = 255;
		if (b > 255)
			b = 255;
		if (r < 0 )
			r = 0;
		if (g < 0)
			g = 0;
		if (b < 0)
			b = 0;
	}
	UGColor operator+(UGColor e)
	{
		return UGColor(r + e.r, g + e.g, b + e.b);
	}
	UGColor operator*(double e)
	{
		return UGColor(r * e, g * e,  b * e);
	}
	UGColor& operator=(const UGColor& e)
	{
		r = e.r;
		g = e.g;
		b = e.b;
		return *this;
	}
};
struct UGVector{
	double x,y,z;
	UGVector(double p1 = 0, double p2 = 0, double p3 = 0): x(p1), y (p2), z(p3){}
	double length()
	{
		return sqrt(x*x + y*y + z*z);
	}
	double square() const 
	{
		return x*x + y*y + z*z;
	}
	double dot(const UGVector& e) const 
	{
		return e.x * x + e.y *y + e.z * z;
	}
	UGVector cross(const UGVector& e)const
	{
		return UGVector(y * e.z - e.y * z, z * e.x - x * e.z, x * e.y - e.x * y);
	}
	void normalize()
	{
		double l = length();
		x /= l;
		y /= l;
		z /= l;
	}
	UGVector operator*(double t)
	{
		return UGVector(t * x ,t * y, t * z);
	}
	UGVector operator+(const UGVector& e)
	{
		return UGVector(x + e.x, y + e.y, z + e.z);
	}
	UGVector operator-(const UGVector& e)
	{
		return UGVector(x - e.x, y - e.y, z - e.z);
	}
	friend ostream& operator<<(ostream& os, const UGVector& e);
};
ostream& operator<<(ostream& os, const UGVector& e)
{
	os << "(" << e.x  << ","  << e.y << "," << e.z <<") ";
	return os;
}
struct UGLine{
	UGVector base;
	UGVector delta;
	UGLine(double b1 = 0, double b2 = 0, double b3 = 0, double d1 = 1, double d2 = 1, double d3 = 1) : base(b1, b2, b3){
		double v = sqrt(d1 * d1 + d2 * d2 + d3 * d3);
		delta.x = d1 / v;
		delta.y = d2 / v;
		delta.z = d3 / v;
	}
	UGLine(UGVector a, UGVector b) : base(a){
		double v = sqrt((b.x - a.x) * (b.x - a.x) +  (b.y - a.y) * (b.y - a.y) + (b.z - a.z) * (b.z - a.z));
		delta.x = (b.x - a.x) / v;
		delta.y = (b.y - a.y) / v;
		delta.z = (b.z - a.z) / v;
	}
};

class Shape{	//物体抽象类
public:
	UGColor self_color;	//自身颜色
	double s;			//Phong模型高光系数
	double refelection;	//反射比例
	double refraction;	//折射比例
	double refraction_index;	//折射率
	bool transmission;		//是否透明
	UGLine lastn;		//存储法线
	Shape(UGColor cd, double ss, double refe ,double refr, bool trans) :self_color(cd), s(ss), refelection(refe), refraction(refr), transmission(trans){}
	//virtual ~Shape();
	virtual bool judgeInterseetion(UGLine incoming, double& dis) = 0;		//判交纯虚函数
	virtual UGColor phongColor(UGVector u, UGVector light, UGColor color)	//phong模型上色，可被覆盖重写纹理函数
	{
		UGColor r(0, 0, 0);
		light = light * -1;
		UGVector h = (u + light) * 0.5;
		double tmp = pow(h.dot(lastn.delta), s);
		tmp = (tmp > 0) ? tmp : -tmp;
		r.r = color.r * (self_color.r / 255 + self_color.r / 255 * tmp);
		r.g = color.g * (self_color.g / 255 + self_color.g / 255 * tmp);
		r.b = color.b * (self_color.b / 255 + self_color.b / 255 * tmp);
		return r;
	}
	virtual UGLine getOutRefractionRay(UGLine incoming, double& attenuation)	//计算折射光线，可被重载，默认为理想面
	{
		attenuation = 1;
		return UGLine(lastn.base.x, lastn.base.y, lastn.base.z,incoming.delta.x, incoming.delta.y, incoming.delta.z);
	}
	UGLine getLastN(){return lastn;}	//返回最后求交的法线
	//virtual UGLine getReflectionRay(UGLine in) = 0;
protected:
	UGLine refract(UGVector incoming, double n1, double n2)	//计算内部折射光线
	{
		incoming.normalize();
		double cosi = fabs(incoming.dot(lastn.delta));
		double P = sqrt(n2 * n2 - n1 * n1 + n1 * n1 * cosi *cosi) - n1 * cosi;
		UGLine t;
		t.base = lastn.base;
		t.delta = incoming - lastn.delta * P;
		t.delta.normalize();
		return t;
	}
};

class RealShape : public Shape{	//面片集合的真实物体，可折射
public:
	vector<Shape*> surface;
	void addSurface(Shape* t){
		surface.push_back(t);
	}
	bool judgeInterseetion(UGLine incoming, double& dis)
	{
		int c = -1;
		dis = 1e50;
		double dis_temp;
		UGLine lastn_temp;
		for (int i = 0; i < (int)surface.size(); i++)
		{
			if (surface[i]->judgeInterseetion(incoming, dis_temp))
				if (dis_temp + EPSILON < dis)
				{
					dis = dis_temp;
					c = i;
				}
		}
		if (c == -1)
			return false;
		lastn = surface[c]->getLastN();
		return true;
	}
	UGLine getOutRefractionRay(UGLine incoming, double& attenuation);
};


class Ball : public Shape{	//球，可折射
	UGVector center;
	double radius;
public:
	Ball(double x, double y, double z, double r, UGColor D, double ss, double re) : Shape(D, ss, re, 0, 0), center(x, y, z) {radius = r;}
	Ball(double x, double y, double z, double r, UGColor D, double ss, double re, double rec, double index) : Shape(D, ss, re, rec, 1), center(x, y, z) {radius = r;refraction_index = index;}
	//~Ball(){}
	bool judgeInterseetion(UGLine incoming, double& dis)
	{
		UGVector delta = center - incoming.base;
		double l = delta.dot(incoming.delta);
		double far = sqrt(delta.square() - l * l);
		if (radius - far > EPSILON)
		{
			dis = l - sqrt(radius * radius - far * far);
			if (dis < EPSILON) return false;
			UGVector p = incoming.delta * dis + incoming.base;
			lastn = UGLine(center, p);
			lastn.base = p;
			return true;
		}
		return false;
	}
	UGLine getOutRefractionRay(UGLine incoming, double& attenuation)
	{
		//cout << "Overload function is called"<< endl;
		UGLine inside = refract(incoming.delta, 1, refraction_index);
		//cout << incoming.delta << "\t " << inside.delta << endl;
		double distant;
		UGVector delta = center - inside.base;
		distant = 2 * delta.dot(inside.delta);
		attenuation = exp(-distant);
		inside.base = inside.base + inside.delta * distant;
		lastn = UGLine(center, inside.base);
		lastn.base = inside.base;
		return refract(inside.delta, refraction_index, 1);
	}
};

class Triangle : public Shape{	//三角面片，可拓展为任意物体
	UGVector v0, v1, v2;
	UGVector n;	
public:
	Triangle(UGVector aa, UGVector bb, UGVector cc, UGColor D, double ss, double re) : Shape(D, ss, re, 0, 0), v0(aa), v1(bb), v2(cc) {n = ((v0 - v1).cross(v1 - v2)) * -1; n.normalize();}
	//~Ball(){}
	bool judgeInterseetion(UGLine incoming, double& dis)
	{
		double t, u, v;
		UGVector dir = incoming.delta;
		dir.normalize();
		// E1
		UGVector E1 = v1 - v0;
		// E2
		UGVector E2 = v2 - v0;
		// P
		UGVector P = dir.cross(E2);
		// determinant
		double det = E1.dot(P);
		// keep det > 0, modify T accordingly
		UGVector T;
		if(det > 0)
		{	
			T = incoming.base - v0;
		}
		else
		{
			T = v0 - incoming.base;
			det = - det;
		}
		// If determinant is near zero, ray lies in plane of triangle
		if( det <0.0001f )
			return false;
		// Calculate u and make sure u <= 1
		u = T.dot(P);
		if( u <0.0f ||u > det )
			return false;
		// Q
		UGVector Q = T.cross(E1);
		// Calculate v and make sure u + v <= 1
		v = dir.dot(Q);
		if( v < 0.0f || u + v > det )
			return false;
		// Calculate t, scale parameters, ray intersects triangle
		t = E2.dot(Q);
		double fInvDet =1.0f/ det;
		t *= fInvDet;
		u *= fInvDet;
		v *= fInvDet;
		dis = t;
		if (dis < EPSILON) return false;
		lastn.base= v0 * (1 - u - v) + v1 * u + v2 * v;
		lastn.delta = n;
		//cout << "s\t" << dis << endl;
		return true;
	}
};

class Parallelogram : public Shape{	//平行四边形，简化计算
	UGVector v0, v1, v2;
	UGVector n;	
	double _u, _v;
	bool picture;	//是否有贴图
	bool texture;	//是否有纹理
	int width, height;	//贴图长宽
	IplImage* img;	//贴图文件映射
public:
	Parallelogram(UGVector aa, UGVector bb, UGVector cc, UGColor D, double ss, double re, bool text) : Shape(D, ss, re, 0, 0), v0(aa), v1(bb), v2(cc) {n = ((v0 - v1).cross(v1 - v2)) * -1; n.normalize();picture = 0;texture = text;}//使用三个点确定四边形位置
	//~Ball(){}
	void addPicture(string path)	//添加纹理
	{
		img = cvLoadImage(path.c_str(), 1);
		if (!img) return;
		/*width = input->width;
		height = input->height;
		img = new short**[width];
		for (int i = 0; i < width; i++)
		{
			img[i] = new short*[height];
			for(int j = 0; j < height; j++)
				img[i][j] = new short[3];
		}
		for (int y = 0; y < height; y++)
		{
			for( int x = 0; x < width; x++)
			{
				CvScalar s = cvGet2D(input, x, y);
				img[x][y][0] = (short)s.val[2];
				img[x][y][1] = (short)s.val[1];
				img[x][y][2] = (short)s.val[0];
			}
		}*/
		picture = 1;
		//cvReleaseImage(&input);
	}
	bool judgeInterseetion(UGLine incoming, double& dis)
	{
		double t, u, v;
		UGVector dir = incoming.delta;
		dir.normalize();
		// E1
		UGVector E1 = v1 - v0;
		// E2
		UGVector E2 = v2 - v0;
		// P
		UGVector P = dir.cross(E2);
		// determinant
		double det = E1.dot(P);
		// keep det > 0, modify T accordingly
		UGVector T;
		if(det > 0)
			T = incoming.base - v0;
		else
		{
			T = v0 - incoming.base;
			det = - det;
		}
		// If determinant is near zero, ray lies in plane of triangle
		if( det <0.0001f )
			return false;
		// Calculate u and make sure u <= 1
		u = T.dot(P);
		if( u <0.0f ||u > det )
			return false;
		// Q
		UGVector Q = T.cross(E1);
		// Calculate v and make sure u + v <= 1
		v = dir.dot(Q);
		if( v < 0.0f || v > det )
			return false;
		// Calculate t, scale parameters, ray intersects triangle
		t = E2.dot(Q);
		double fInvDet =double(1.0)/ det;
		t *= fInvDet;
		u *= fInvDet;
		v *= fInvDet;
		_u = u;
		_v = v;
		dis = t;
		if (dis < EPSILON) return false;
		lastn.base= v0 * (1 - u - v) + v1 * u + v2 * v;
		lastn.delta = n;
		return true;
	}
	UGColor phongColor(UGVector u, UGVector light, UGColor color)
	{
		UGColor cp = self_color;
		if (picture)		//如果有贴图	
		{
			int x = (int)(_u * img->width);
			int y = (int)(_v * img->height);
			CvScalar s = cvGet2D(img, x, y);
			//cout << "open (" << x << ", " << y << ")" <<endl;
			double imgr = s.val[2];
			double imgg = s.val[1];
			double imgb = s.val[0];
			cp = UGColor(imgr, imgg, imgb);
		}
		else if (texture)	//如果有纹理
		{
			if ((int(_u*20) & 1) == 1 &&  (int(_v*20) & 1) == 1)
				cp = UGColor(0, 0, 0);			
		}
		UGColor r(0, 0, 0);
		light = light * -1;				//Phong模型
		UGVector h = (u + light) * 0.5;
		double tmp = pow(h.dot(lastn.delta), s);
		tmp = (tmp > 0) ? tmp : -tmp;
		r.r = color.r * (cp.r / 255 + cp.r / 255 * tmp);
		r.g = color.g * (cp.g / 255 + cp.g / 255 * tmp);
		r.b = color.b * (cp.b / 255 + cp.b / 255 * tmp);
		return r;
	}
};

struct UGLight
{
	UGVector place;
	UGColor color;
	UGLight(UGVector p, UGColor c) : place(p), color(c){}
};

vector<Shape*> Room;	//物体集合
vector<UGLight*> Light;	//光源集合
UGLine getRefectRay(UGVector l, UGLine n)
{
	UGLine re(0,0,0,1,0,0);
	n.delta.normalize();
	l.normalize();
	re.base = n.base;
	re.delta = l - n.delta * ((n.delta.dot(l)) * 2);
	re.delta.normalize();
	return re;
}
UGColor rayTracing(UGLine ray, int times)
{
	UGColor r(0, 0, 0);
	int c = -1;
	double d, minD = 10000000;
	for(int i = 0; i < (int)Room.size(); i++)	//一一求交，并找出最小值
		if (Room[i]->judgeInterseetion(ray, d))
			if (d < minD)
			{
				c = i;
				minD = d;
			}
	if (c != -1)	//如果相交
	{
		double self = 1 - Room[c]->refelection - Room[c]->refraction;
		const UGLine n = Room[c]->getLastN();	//取出法向量
		if (self > 0.01f)	//如果有自身颜色
		{
			int light_counter;
			double Maxd;
			for(int i = 0; i < (int)Light.size(); i++)	//对于每条光线
			{
				UGLine light = UGLine(n.base, Light[i]->place);
				light_counter = 0;
				UGVector distant = (Light[i]->place - n.base);
				Maxd = distant.length() / light.delta.length();
				UGVector l = light.delta;
				if (n.delta.dot(l) < -EPSILON)	//如果和实现是钝角，则无光
					continue;
				for(int j = 0; j < (int)Room.size(); j++)	//和物体求交
				{
					if ( j == c)
						continue;
					if (Room[j]->judgeInterseetion(light, d))
						if (d < Maxd)
						{
							light_counter = 1;
							break;
						}
				}
				if (light_counter)		//若被遮挡则无光
					continue;
				r  = r + Room[c]->phongColor(ray.delta, light.delta, Light[i]->color);	//否则返回Phong模型颜色
			}
		}
		if (times < 10)	//如果迭代次数未达到
		{
			UGColor color_refelect(0,0,0);
			UGColor color_refract(0,0,0);
			if (Room[c]->refelection > 0.01f)	//如果有反射光
			{
				UGLine refelect = getRefectRay(ray.delta, n);
				color_refelect = rayTracing(refelect, times + 1);
			}
			if (Room[c]->transmission)		//如果有折射光
			{
				double a;
				UGLine refract = Room[c]->getOutRefractionRay(ray, a);
				color_refract = rayTracing(refract, times + 1);
			}
			r = r * self + color_refelect * Room[c]->refelection + color_refract * Room[c]->refraction;	//合并颜色
		}
	}
	r.check();
	return r;
}


struct Camera{	//空间内相机类
	UGVector point;
	UGVector x, y, z;
	double zoom;
	int width, height;
	Camera(UGVector p, UGVector xx, UGVector yy, double zz, int w, int h) : point(p), x(xx), y(yy), zoom(zz), width(w), height(h)
	{
		x.normalize();
		y.normalize();
		z = x.cross(y);
	}
	UGLine getRay(int xx, int yy)
	{
		UGVector to = point + z * zoom + x * (xx - width / 2) + y * (yy - height / 2);
		return UGLine(point, to);
	}
};
Camera camera(UGVector(300, 300, -200), UGVector(1, 0, 0), UGVector(0, 1, 0), 300, 600, 600);

void prepare()	//构造房间物体
{
	Room.push_back(new Ball(100,100, 300, 50, UGColor(255, 151, 145), 6, 0.1));
	Room.push_back(new Ball(400,300, 100, 50, UGColor(150, 150, 50), 3, 0.2));
	Room.push_back(new Ball(100,400, 200, 100, UGColor(50, 50, 50), 3, 0.5));
	Room.push_back(new Ball(400,400, 30, 30, UGColor(150, 50, 150), 3, 0.1));
	Room.push_back(new Ball(250,250, 50, 70, UGColor(255, 255, 255), 10, 0, 0.75, 1.33));
	int x = 0,y = 0,z = 0;
	int DEPTH = 600;
	int HEIGHT = 600;
	int WIDTH = 600;
	//Left Wall
	Room.push_back(new Parallelogram(UGVector(x,y + HEIGHT,z), UGVector(x,y,z), UGVector(x,y + HEIGHT ,z + DEPTH), UGColor(13, 117, 6), 4, 0, 0));
	//Right Wall
	Room.push_back(new Parallelogram(UGVector(x + WIDTH,y + HEIGHT,z), UGVector(x + WIDTH,y + HEIGHT,z + DEPTH), UGVector(x + WIDTH,y,z), UGColor(13, 117, 6), 4, 1, 0));
	//Roof
	Room.push_back(new Parallelogram(UGVector(x,y,z), UGVector(x + WIDTH,y,z), UGVector(x,y,z + DEPTH), UGColor(0, 122, 179), 3, 0, 0));
	//Back
	Room.push_back(new Parallelogram(UGVector(x-5,y-5,z + DEPTH -5), UGVector(x + WIDTH + 5,y -5 ,z + DEPTH - 5), UGVector(x - 5,y + 5 + HEIGHT,z + DEPTH -5 ), UGColor(67, 161, 2), 3, 0, 0));
	//((Parallelogram*)Room[5])->addPicture("2.jpg");
	//Floor
	Room.push_back(new Parallelogram(UGVector(x,y + HEIGHT,z), UGVector(x,y  + HEIGHT,z + DEPTH), UGVector(x + WIDTH,y + HEIGHT,z), UGColor(94, 76, 41), 3, 0, 1));
	
	Room.push_back(new Parallelogram(UGVector(x-600,y-600,-600), UGVector(x - 600,y + 600 + HEIGHT,-600 ), UGVector(x + WIDTH + 600,y -600 ,-600), UGColor(255, 0, 255), 3, 0, 0));

	//Light.push_back(new UGLight(UGVector(450,300,-100), UGColor(150, 200, 150)));
	Light.push_back(new UGLight(UGVector(150,300,-199), UGColor(150, 150, 150)));
	Light.push_back(new UGLight(UGVector(150,300,199), UGColor(150, 150, 150)));
}
int _tmain(int argc, _TCHAR* argv[])
{
	char* writeFilename = "test.avi";
	 CvVideoWriter* videoWriter = cvCreateVideoWriter(writeFilename,-1, 25, cvSize(600,600), 1);

	int WIDTH = 600, HEIGHT = 600;
	IplImage *im = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 3);//创建一个图像

	prepare();
	for( int cc = -400; cc < 0; cc+= 3)	//每次更改相机z坐标
	{
		cout << cc << endl;
		camera.point.z = cc;
		for (int i = 0; i < WIDTH; i++)
		{
		   //遍历每行
			for (int j = 0; j < HEIGHT; j ++)
			{
			  //遍历每列
				UGLine t = camera.getRay(i, j);
				UGColor r = rayTracing(t, 0);
				im->imageData[j*im->widthStep+3*i+2] = (int) r.r;
				im->imageData[j*im->widthStep+3*i+1] = (int) r.g;
				im->imageData[j*im->widthStep+3*i+0] = (int) r.b;
			}
		}
		cvWriteFrame(videoWriter,im);
	}
	camera.point.z = -100;
	for( int cc = -100; cc < 100; cc+= 10)//转动视角
	{
		camera.x = UGVector(1, 0, double(cc) / 100.0);	
		camera.x.normalize();
		camera.z = camera.x.cross(camera.y);
		for (int i = 0; i < WIDTH; i++)
		{
		   //遍历每行
			for (int j = 0; j < HEIGHT; j ++)
			{
			  //遍历每列
				UGLine t = camera.getRay(i, j);
				UGColor r = rayTracing(t, 0);
				im->imageData[j*im->widthStep+3*i+2] = (int) r.r;
				im->imageData[j*im->widthStep+3*i+1] = (int) r.g;
				im->imageData[j*im->widthStep+3*i+0] = (int) r.b;
			}
		}
		cvWriteFrame(videoWriter,im);
	}
	//cvShowImage("rayTracing", im);
	//显示图像img在一个名叫show的窗口上
	//cvSaveImage("out.png", im);
	cvReleaseVideoWriter(&videoWriter);
	//waitKey(0);
	cvReleaseImage(&im);
	//等待键盘消息再退出
	return 0;
}

