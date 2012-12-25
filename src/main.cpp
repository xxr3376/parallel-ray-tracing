#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <cstdio>
#include <vector>
#include "pnt.h"
#include "triangle.h"
#include "common.h"

using namespace cv;
using namespace std;

//int cilk_main() {
int main() {

	vector<Triangle> triangles;
	read_obj_file("../data/Sphere01.obj", triangles);

	for (int i = 0; i < triangles.size(); ++i) {
		Triangle &t = triangles[i];
		printf("(%f,%f,%f) , (%f,%f,%f) , (%f,%f,%f) : (%f,%f,%f)\n", t.a.x, t.a.y, t.a.z, t.b.x, t.b.y, t.b.z, t.c.x, t.c.y, t.c.z, t.x, t.y, t.z);
	}

	Mat img = imread("lena.jpg"); 
	if(!img.data) 
		return -1; 
	namedWindow("Lena", CV_WINDOW_AUTOSIZE); 
	imshow("Lena", img); 
	waitKey();

	return 0;
}
