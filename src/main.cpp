#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <cstdio>
#include <vector>
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

vector<Triangle> triangles;
vector<Vector3> lightList;
BVHTree tree;
const int MAX_DEPTH = 0;
Color3 rayTracing(const Line3& input, int depth){
	Color3 result;
	int index;
	Vector3 intersection;
	if (tree.intersect(input, intersection, index)){
		Triangle& theOne = triangles[index];
		Vector3 n = det(theOne.b - theOne.a, theOne.c - theOne.a).normalize();
		if (dot(n, input.d) > 0){
			n = n * -1;
		}
		for (int i = 0; i < (int) lightList.size(); i++){
			Vector3 l = lightList[i] - intersection;
			if (dot(l, n) > 0){
				l = l * -1;
				l.normalize();
				Vector3 u = input.d;
				Vector3 h = (u + l) * 0.5;
				float tmp = pow(dot(h, n), 2);
				tmp = fabs(tmp) * 0.5;
				result += Color3(tmp, tmp, tmp);
			}
		}
		if (depth > 0){
			Vector3 RayIn = input.d;
			RayIn.normalize();
			Vector3 RayOut = RayIn - (n * (dot(n, RayIn) * 2));
			intersection += (RayOut * EPS);
			Line3 reflect(intersection, RayOut);
			result += (rayTracing(reflect, depth - 1) * 0.5);
		}
		return result;
	}
	return Color3(0.05, 0.05, 0.05);
}


void init(){
	lightList.push_back(Vector3(100, 100, 100));
	lightList.push_back(Vector3(-100, 0, 100));
	lightList.push_back(Vector3(0, 0, -100));
}

void render(IplImage* im, const Camera& cam, int WIDTH, int HEIGHT){
    for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j ++) {
			//tree.CCOUNT = 0;
			Color3 c = rayTracing(cam.getSight(i, j), MAX_DEPTH);
			c.normalize();
			im->imageData[j*im->widthStep+3*i+2] = (int) (c.r * 255);
			im->imageData[j*im->widthStep+3*i+1] = (int) (c.g * 255);
			im->imageData[j*im->widthStep+3*i+0] = (int) (c.b * 255);
			//printf("%d\n", tree.CCOUNT);
        }
    }
}
//int cilk_main() {
int main() {
	init();
	read_obj_file("../data/yaya.obj", triangles);
	tree.create_tree(triangles.size(), &triangles[0]);
	printf("finished... build tree\n");
	fflush(stdin);
	Camera cam(Vector3(0, 0, 100), Vector3(1, 0, 0), Vector3(0, -1, 0), 800, 600);
	//cam.rotate(0, -45.0 / 180 * 3.14);
	float focal = 20;
	cam.setFocalLen(20);
	int WIDTH = 400, HEIGHT = 300;
	int k = 0;
	int loop_flag;
	while(true){
		loop_flag = 1;
		IplImage *im = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 3);//创建一个图像
		render(im, cam, WIDTH, HEIGHT);
		cvShowImage("rayTracing", im);
		while(loop_flag){
			k = waitKey(0);
			printf("%d\n", k);
			loop_flag = 0;
			if (k == 65364){ cam.rotate(0, 0.01); }
			else if (k == 65361){ cam.rotate(-0.01, 0); }
			else if (k == 65362){ cam.rotate(0, -0.01); }
			else if (k == 65363){ cam.rotate(0.01, 0); }
			else if (k == 45) { focal-=10;cam.setFocalLen(focal);}
			else if (k == 61) { focal+=10;cam.setFocalLen(focal);}
			else if (k == 27){return 0;}
			else{
				loop_flag = 1;
			}
		}
	}
	return 0;
}
