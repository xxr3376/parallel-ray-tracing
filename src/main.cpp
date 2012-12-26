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

//using namespace cv;
using namespace std;

//int cilk_main() {
int main() {

	vector<Triangle> triangles;
	read_obj_file("../data/Sphere01.obj", triangles);
	BVHTree tree;
	tree.create_tree(triangles.size(), &triangles[0]);

	Line3 a(Vector3(100, 100, 100), Vector3(-100, -100, -100));

	Vector3 v;
	int index;

	int WIDTH = 800, HEIGHT = 600;
    Vector3 point;
    for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j ++) {
			tree.CCOUNT = 0;
			float x = (i - WIDTH / 2) / 20.0;
			float y = (j - HEIGHT / 2) / 20.0;
				int index = 0;
				Line3 input(Vector3(x, y, 0), Vector3(0, 0 ,1));
				int k = tree.intersect(input, point, index) * 255;
				printf("%d %f %f\n", tree.CCOUNT, input.o.x, input.o.y);
        }
    }

	tree.CCOUNT = 0;
	if (tree.intersect(a, v, index)) {
		printf("%f %f %f  |  %d\n", v.x, v.y, v.z, index);
		printf("%d\n", tree.CCOUNT);
	}
	return 0;
}
