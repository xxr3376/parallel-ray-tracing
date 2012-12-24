#ifndef __BVHTREE__
#define __BVHTREE__

#include <cstdlib>

#include "triangle.h"
#include "pnt.h"

class BVHBox {
public:
	Vector2 x_range, y_range, z_range;
	int count;

	void calc_box(Triangle *t, int n);
	void clear();
	bool is_empty();
	void add(const Triangle& t);
	void add(const Vector3& p);
	float get_surface_area();
	void merge(const BVHBox& b);
private:
};

class BVHTree {
public:
	//用于存储这棵树表示的所有三角形
	Triangle *triangles;
	
	//包围和
	//Vector2 x_range, y_range, z_range;
	BVHBox box;

	//左右儿子
	BVHTree *left_son, *right_son;
	
	//表示了这个子树下面区间[l, r]的三角形
	int l, r;
	
	
	//判断是否是叶子
	bool is_leaf();
	//建树
	void create_tree(int n, Triangle *triangles = NULL);

	//析构
	~BVHTree();

private:
	
	void _create(int l, int r);
	int _get_diff(const Triangle& t, int type);


	static const int BLOCK_SIZE; 
	static BVHBox *_piece_box;
};

#endif // __BVHTREE__