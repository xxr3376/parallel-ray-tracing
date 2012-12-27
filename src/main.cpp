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
				u.normalize();
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
	lightList.push_back(Vector3(-100, 100, 100));
	//lightList.push_back(Vector3(0, 0, -100));
}

//int cilk_main() {
int main() {
	init();
	read_obj_file("../data/plane.obj", triangles);
	tree.create_tree(triangles.size(), &triangles[0]);
	printf("finished... build tree\n");
	fflush(stdin);
	Camera cam(Vector3(0, 0, 200), Vector3(1, 0, 0), Vector3(0, -1, 0), 800, 600);
//	Camera cam(Vector3(0, 200, 0), Vector3(1, 0, 0), Vector3(0, 0, 1), 800, 600);
	cam.setFocalLen(100);
	int WIDTH = 800, HEIGHT = 600;
	IplImage *im = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 3);//创建一个图像
    Vector3 point;
    for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j ++) {
			//tree.CCOUNT = 0;
			Color3 c = rayTracing(cam.getSight(i, j), MAX_DEPTH);
			im->imageData[j*im->widthStep+3*i+2] = (int) (c.r * 255);
			im->imageData[j*im->widthStep+3*i+1] = (int) (c.g * 255);
			im->imageData[j*im->widthStep+3*i+0] = (int) (c.b * 255);
			//printf("%d\n", tree.CCOUNT);
        }
    }
	cvShowImage("rayTracing", im);
	cvSaveImage("out1.png", im);
	waitKey(0);
	return 0;
}
