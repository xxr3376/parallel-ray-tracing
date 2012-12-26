
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

using namespace cv;
using namespace std;

//int cilk_main() {
int main() {

	vector<Triangle> triangles;
	read_obj_file("../data/Sphere01.obj", triangles);

	BVHTree bvh;
	bvh.create_tree(triangles.size(), &triangles[0]);

	int WIDTH = 320, HEIGHT = 240;
	IplImage *im = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 3);//创建一个图像
	Vector3 point;
	int index = 0;
	Camera cam(Vector3(10, 0, -1000), Vector3(1, 0, 0), Vector3(0, 1, 0), WIDTH, HEIGHT);
	for (int i = 0; i < WIDTH; i++)
	{
		//遍历每行
		for (int j = 0; j < HEIGHT; j ++)
		{
			//遍历每列
			//im->imageData[j*im->widthStep+3*i+2] = (int) r.r;
			//im->imageData[j*im->widthStep+3*i+1] = (int) r.g;
			//im->imageData[j*im->widthStep+3*i+0] = (int) r.b;
			Line3 input = cam.getSight(i, j);
			int k = bvh.intersect(input, point, index) * 255;
			im->imageData[j*im->widthStep+3*i+2] = k;
            im->imageData[j*im->widthStep+3*i+1] = k;
			im->imageData[j*im->widthStep+3*i+0] = k;
		}
	}
	cvShowImage("rayTracing", im);
	waitKey(0);
	return 0;
}
