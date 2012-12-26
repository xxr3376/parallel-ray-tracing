#include <cstdio>
#include <vector>
#include "pnt.h"
#include "triangle.h"
#include "line.h"
#include "common.h"
#include "bvhtree.h"

using std::vector;

int main() {
	
	vector<Triangle> triangles;
	read_obj_file("../data/Sphere01.obj", triangles);

	for (int i = 0; i < triangles.size(); ++i) {
		Triangle &t = triangles[i];
		printf("(%f,%f,%f) , (%f,%f,%f) , (%f,%f,%f) : (%f,%f,%f)\n", t.a.x, t.a.y, t.a.z, t.b.x, t.b.y, t.b.z, t.c.x, t.c.y, t.c.z, t.x, t.y, t.z);
	}

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
