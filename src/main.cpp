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
Vector3 light(100, 100, 100);
BVHTree tree;

Color3 rayTracing(const Line3& input, int depth, int x, int y){
	/*
	if (depth <= 0)
		return Color3(0, 0, 0);
	*/
	int index;
	Vector3 intersection;
	if (tree.intersect(input, intersection, index)){

		/*
		Triangle& theOne = triangles[index];
		Vector3 n = det(theOne.b - theOne.a, theOne.c - theOne.a).normalize();
		if (dot(n, input.d) > 0){
			n = n * -1;
		}
		Vector3 l = light - intersection;
		if (dot(light
		*/

		return Color3(1, 1, 1);
	}
	if (x == 400 && (y > 150 && y < 450)){
		printf("%f, %f, %f \t", input.o.x, input.o.y, input.o.z);
		printf("%f, %f, %f \n", input.d.x, input.d.y, input.d.z);
	}
	return Color3(0, 0, 0);
}
//
//
//int cilk_main() {
int main() {
	read_obj_file("../data/Sphere01.obj", triangles);
	tree.create_tree(triangles.size(), &triangles[0]);

	Camera cam(Vector3(0, 0, -200), Vector3(1, 0, 0), Vector3(0, 1, 0), 800, 600);
	cam.setFocalLen(30);
	int WIDTH = 800, HEIGHT = 600;
	IplImage *im = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 3);//创建一个图像
    Vector3 point;
    for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j ++) {
			Color3 c = rayTracing(cam.getSight(i, j), 10, i, j);
			im->imageData[j*im->widthStep+3*i+2] = (int) c.r * 255;
			im->imageData[j*im->widthStep+3*i+1] = (int) c.g * 255;
			im->imageData[j*im->widthStep+3*i+0] = (int) c.b * 255;
        }
    }
	cvShowImage("rayTracing", im);
	waitKey(0);
	return 0;
}
