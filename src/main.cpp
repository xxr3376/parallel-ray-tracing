#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <cstdio>
#include <vector>
#include "pnt.h"
#include "triangle.h"
#include "line.h"
#include "common.h"
#include "bvhtree.h"

using namespace cv;
using namespace std;

//int cilk_main() {
int main() {

	vector<Triangle> triangles;
	read_obj_file("../data/Sphere01.obj", triangles);

	BVHTree bvh;
	printf("begin to build tree");
	bvh.create_tree(triangles.size(), &triangles[0]);
	printf("build success");

	int WIDTH = 800, HEIGHT = 600;
	IplImage *im = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 3);//创建一个图像
	for (int i = 0; i < WIDTH; i++)
	{
		//遍历每行
		for (int j = 0; j < HEIGHT; j ++)
		{
			//遍历每列
			//im->imageData[j*im->widthStep+3*i+2] = (int) r.r;
			//im->imageData[j*im->widthStep+3*i+1] = (int) r.g;
			//im->imageData[j*im->widthStep+3*i+0] = (int) r.b;
			int x = i + WIDTH / 2;
			int y = j + HEIGHT / 2;
			printf("(%d, %d)\n", x, y);
			int index = 0;
			Vector3 point;
			Line3 input(Vector3(x, y, 0), Vector3(0, 0 ,1));
			int k = bvh.intersect(input, point, index) * 255;
			im->imageData[j*im->widthStep+3*i+2] = k;
            im->imageData[j*im->widthStep+3*i+1] = k;
            im->imageData[j*im->widthStep+3*i+0] = k;
		}
	}
	cvShowImage("rayTracing", im);
	waitKey(0);


	//Mat img = imread("lena.jpg"); 
	//if(!img.data) 
	//	return -1; 
	//namedWindow("Lena", CV_WINDOW_AUTOSIZE); 
	//imshow("Lena", img); 
	//waitKey();

	BVHTree tree;
	tree.create_tree(triangles.size(), &triangles[0]);

	Line3 a(Vector3(100, 100, 100), Vector3(-100, -100, -100));

	Vector3 v;
	int index;
	if (tree.intersect(a, v, index)) {
		printf("%f %f %f  |  %d\n", v.x, v.y, v.z, index);
	}

	return 0;
}
