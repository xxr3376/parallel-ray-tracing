#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <cstdio>
#include <vector>
#include <cmath>
#include "pnt.h"
#include "triangle.h"
#include "line.h"
#include "common.h"
#include "bvhtree.h"
#include "camera.h"
#include "color.h"
#include "camera.h"

using namespace cv;
using namespace std;

Line3 getRefractRay(Vector3 inRay, Vector3 n, Vector3 intersection, float refractiveIndex){
	Vector3 tmp1 = (inRay + n * fabs(dot(inRay, n)));
	float fixed = det(inRay, n).length() * (1 / refractiveIndex - 1);
	Vector3 refractDirection = tmp1 * fixed + inRay;
	return Line3(intersection + refractDirection * EPS, refractDirection);
}

vector<Triangle> triangles;
vector<Vector3> lightList;
vector<Mat> textures;
BVHTree tree;
const int MAX_DEPTH = 10;
Color3 rayTracing(const Line3& input, int depth){
	Color3 result;
	int index;
	Vector4 interPack;
	if (tree.intersect(input, interPack, index)){
		Vector3 intersection = input.get_intersect(interPack.w);
		Triangle& theOne = triangles[index];
		Vector3 n = det(theOne.b - theOne.a, theOne.c - theOne.a).normalize();
		Vector3 texture_point = triangles[index].get_texture_point(interPack);
		Mat texture = textures[triangles[index].attr->textureNumber];
		int imgSize = texture.size.p[0];
		float reflectivity = triangles[index].attr->reflectivity;
		float refractivity = triangles[index].attr->refractivity;
		Vec3b k = texture.at<Vec3b>(Min((int)floor((1 - texture_point.y) * imgSize), imgSize - 1), Min((int)(( texture_point.x) * imgSize), imgSize - 1));
		if (dot(n, input.d) < 0){
			for (int i = 0; i < (int) lightList.size(); i++){
				Vector3 l = lightList[i] - intersection;
				if (dot(l, n) > 0){
					float lightLen = l.length();
					Line3 toLight = Line3(intersection, l);
					Vector4 lightPack;
					int lightIndex;
					int blocked = tree.intersect(toLight, lightPack, lightIndex);
					if (!blocked || lightLen < lightPack.w + EPS){
						l = l * -1;
						l.normalize();
						Vector3 u = input.d;
						Vector3 h = (u + l) * 0.5;
						float tmp = pow(dot(h, n), 2);
						tmp = fabs(tmp) * (1 - reflectivity - refractivity);
						Color3 self_color = Color3(k[2], k[1], k[0]) * (tmp / 255);
						result += self_color;
					}
				}
			}
			if (depth > 0 && reflectivity > 0){
				Vector3 RayIn = input.d;
				RayIn.normalize();
				Vector3 RayOut = RayIn - (n * (dot(n, RayIn) * 2));
				intersection += (RayOut * EPS);
				Line3 reflect(intersection, RayOut);
				result += (rayTracing(reflect, depth - 1)* reflectivity);
			}
		}
		if (depth > 0 && refractivity > 0){
			float refractiveIndex = triangles[index].attr->refractiveIndex;
			if (dot(n, input.d) > 0){
				refractiveIndex = 1 / refractiveIndex;
				n = n * -1;
			}
			Line3 next = getRefractRay(input.d, n, intersection, refractiveIndex);
			result += (rayTracing(next, depth - 1)* refractivity);
		}
		return result;
	}
	return Color3(0.05, 0.05, 0.05);
}


void init(){
	lightList.push_back(Vector3(100, 100, 100));
	lightList.push_back(Vector3(-100, 0, 100));
	lightList.push_back(Vector3(0, 0, -100));
	Mat img  = imread("../data/BabyCrocodileGreen.png"); 
	if(!img.data) exit(-1);
	textures.push_back(img);
	img  = imread("../data/pure.png"); 
	textures.push_back(img);
	Attribute* a = new Attribute();
	a->textureNumber = 0;
	a->reflectivity = 0.4;
	read_obj_file("../data/yaya.obj", triangles, a);
	a = new Attribute();
	a->textureNumber = 1;
	a->reflectivity = 0.3;
	a->refractivity = 0.7;
	a->refractiveIndex = 1.2;
	read_obj_file("../data/Sphere02.obj", triangles, a);

	tree.create_tree(triangles.size(), &triangles[0]);
}

void render(IplImage* im, const Camera& cam, int WIDTH, int HEIGHT){
	//#pragma omp parallel for schedule(dynamic, 10)
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j ++) {
			Color3 c = rayTracing(cam.getSight(i, j), MAX_DEPTH);
			c.normalize();
			im->imageData[j*im->widthStep+3*i+2] = (int) (c.r * 255);
			im->imageData[j*im->widthStep+3*i+1] = (int) (c.g * 255);
			im->imageData[j*im->widthStep+3*i+0] = (int) (c.b * 255);
		}
	}
}
//int cilk_main() {
int main() {
	init();
	int WIDTH = 400, HEIGHT = 300;
	Camera cam(Vector3(80, 0, 00), Vector3(0, 0, -1), Vector3(0, -1, 0), WIDTH, HEIGHT);
	float focal = 50;
	cam.setFocalLen(focal);
	int k = 0;
	int loop_flag;
	while(true){
		loop_flag = 1;
		IplImage *im = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 3);//创建一个图像
		render(im, cam, WIDTH, HEIGHT);
		cvShowImage("rayTracing", im);
		cam.print();
		while(loop_flag){
			k = waitKey(0);
			printf("%d\n", k);
			loop_flag = 0;
			if (k == 65364){ cam.rotate(0, 0.03); }
			else if (k == 65361){ cam.rotate(-0.03, 0); }
			else if (k == 65362){ cam.rotate(0, -0.03); }
			else if (k == 65363){ cam.rotate(0.03, 0); }
			else if (k == 45) { focal-=10;cam.setFocalLen(focal);}
			else if (k == 61) { focal+=10;cam.setFocalLen(focal);}
			else if (k == 119) { cam.move(0, -150);}
			else if (k == 115) { cam.move(0, 150);}
			else if (k == 97) { cam.move(-150, 0);}
			else if (k == 100) { cam.move(150, 0);}
			else if (k == 27){return 0;}
			else{
				loop_flag = 1;
			}
		}
	}
	return 0;
}
