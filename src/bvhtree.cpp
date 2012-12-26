#include "bvhtree.h"

#include <algorithm>
#include <cfloat>
#include "plane.h"
#include "common.h"

int BVHTree::CCOUNT = 0;
const int BVHTree::BLOCK_SIZE = 32;
BVHBox* BVHTree::_piece_box = new BVHBox[BVHTree::BLOCK_SIZE];

bool BVHTree::is_leaf() const {
	return l == r;
}

void BVHTree::create_tree(int n, Triangle *triangles) {
	if (triangles != NULL) this->triangles = triangles;

	box.calc_box( this->triangles, n);
	_create(0, n - 1);
}

//BVHBox _piece_box [BVHTree::BLOCK_SIZE];
void BVHTree::_create(int l, int r) {
	this->l = l;
	this->r = r;
	if (l == r) {
		left_son = right_son = NULL;
		return;
	}

	float result = FLT_MAX;
	int split_i = 0, type = 0;

	for (int split_type = 0; split_type < 3; ++ split_type) {
		float block_length = _get_block_length(split_type);
				
		if (block_length > 0) {
			for (int i = 0; i < BLOCK_SIZE; ++i)
				_piece_box[i].clear();

			for (int i = l; i <= r; ++i) {
				int t = _get_diff(triangles[i], block_length, split_type);
				if (t >= BLOCK_SIZE) t = BLOCK_SIZE - 1;
				
				_piece_box[t].add(triangles[i]);
			}

			BVHBox bl, br;
			for (int i = 0; i < BLOCK_SIZE; ++i) {
				bl.clear(), br.clear();
				
				for (int j = 0; j < BLOCK_SIZE; ++j)
					if (j <= i) bl.merge( _piece_box[j] );
					else br.merge( _piece_box[j] );
				float value = bl.get_surface_area() + br.get_surface_area();

				if (value < result) {
					result = value;
					type = split_type;
					split_i = i;
				}
			}

		}	
	}


	int left = l, right = r, li, ri;
	float block_length = _get_block_length(type);

	do	{
		do	{
			li = _get_diff(triangles[left], block_length, type); 
			left ++;
		} while (li <= split_i);
		do	{
			ri = _get_diff(triangles[right], block_length, type);
			right --;
		} while (ri > split_i);

		left --;
		right ++;
		if (left < right) {
			std::swap(triangles[left], triangles[right]);
			left ++;
			right --;
		}
	}	while(left < right);

	li = _get_diff(triangles[left], block_length, type); 
	if (li > split_i) left --;

	left_son = new BVHTree();
	right_son = new BVHTree();

	left_son->triangles = right_son->triangles = this->triangles;
	left_son->box.clear();
	right_son->box.clear();
	for (int i = l; i <= r; ++i)
		if (i <= left) left_son->box.add(triangles[i]);
		else right_son->box.add(triangles[i]);
	
	left_son->_create(l, left);
	right_son->_create(left+1, r);
}

int BVHTree::_get_diff(const Triangle& t, float block_length, int type) {
	float result = 0;
	switch (type) {
		case 0:
			result = (t.x - box.x_range.x);
			break;
		case 1:
			result = (t.y - box.y_range.x);
			break;
		case 2:
			result = (t.z - box.z_range.x);
	}
	return std::min(int(result / block_length), BLOCK_SIZE - 1);
}

float BVHTree::_get_block_length(int type) {
	switch (type) {
		case 0 :
			return ( box.x_range.y - box.x_range.x ) / BLOCK_SIZE;
		case 1 :
			return ( box.y_range.y - box.y_range.x ) / BLOCK_SIZE;
		case 2 :
			return ( box.z_range.y - box.z_range.x ) / BLOCK_SIZE;
	}

}

bool BVHTree::intersect(const Line3& line, Vector3& result, int& index) {
	CCOUNT ++;
	if (l == r)
		return triangles[index = l].intersect(line, result);
	
	int r_index;
	Vector3 r_result;
	bool l_flag, r_flag;

	l_flag = this->left_son->box.intersect(line) && this->left_son->intersect(line, result, index);
	r_flag = this->right_son->box.intersect(line) && this->right_son->intersect(line, r_result, r_index);

	if (l_flag && r_flag)
		r_flag = dist(result, line.o) > dist(r_result, line.o);
	
	if (r_flag) {
		index = r_index;
		result = r_result;
	}

	return l_flag || r_flag;
}

BVHTree::~BVHTree() {
	if (!is_leaf()) {
		delete left_son;
		delete right_son;
	}
}

//-------------BVHBox--------------
void BVHBox::calc_box(Triangle *t, int n) {
	clear();
	for (int i = 0; i < n; ++i)
		add(t[i]);
}

void BVHBox::clear() {
	count = 0;
	x_range.x = y_range.x = z_range.x = FLT_MAX;
	x_range.y = y_range.y = z_range.y = -FLT_MAX;
}

bool BVHBox::is_empty() {
	return count;
}

void BVHBox::add(const Triangle& t) {
	count ++;
	add(t.a);
	add(t.b);
	add(t.c);
}

void BVHBox::add(const Vector3& p) {
	x_range.x = std::min(x_range.x, p.x);
	x_range.y = std::max(x_range.y, p.x);
	y_range.x = std::min(y_range.x, p.y);
	y_range.y = std::max(y_range.y, p.y);
	z_range.x = std::min(z_range.x, p.z);
	z_range.y = std::max(z_range.y, p.z);
}

float BVHBox::get_surface_area() {
	if (count)
		return count * ( (x_range.y - x_range.x) * (y_range.y - y_range.x) +
			(z_range.y - z_range.x) * (y_range.y - y_range.x) +
			(x_range.y - x_range.x) * (z_range.y - z_range.x));
	return 0;
}

void BVHBox::merge(const BVHBox& b) {
	count += b.count;
	x_range.x = std::min(x_range.x, b.x_range.x);
	x_range.y = std::max(x_range.y, b.x_range.y);
	y_range.x = std::min(y_range.x, b.y_range.x);
	y_range.y = std::max(y_range.y, b.y_range.y);
	z_range.x = std::min(z_range.x, b.z_range.x);
	z_range.y = std::max(z_range.y, b.z_range.y);
}

bool BVHBox::intersect(const Line3& line) const {
	float t_min = -FLT_MAX, t_max = FLT_MAX;
	bool flag = false;
	Vector3 p_min, p_max;

	for (int i = 0; i < 3; ++i) {
		float lvalue, rvalue;
		Plane::CoordinateType type;
		switch (i) {
			case 0 :
				type = Plane::X;
				lvalue = x_range.x;
				rvalue = x_range.y;
				break;
			case 1 :
				type = Plane::Y;
				lvalue = y_range.x;
				rvalue = y_range.y;
				break;
			case 2 :
				type = Plane::Z;
				lvalue = z_range.x;
				rvalue = z_range.y;
				break;
		}

		Plane lp(lvalue, type), rp(rvalue, type);
		Vector3 lrst, rrst;
		bool l, r;

		l = lp.intersect(line, lrst);
		r = rp.intersect(line, rrst);

		if (l && r) {
			float tl = dist(lrst, line.o);
			float tr = dist(rrst, line.o);
			if (tl > tr) {
				std::swap(tl, tr);
				std::swap(lrst, rrst);
			}

			if (t_min < tl) {
				t_min = tl;
				p_min = lrst;
			}
			if (t_max > tr) {
				t_max = tr;
				p_max = rrst;
			}

			//t_min = std::max(t_min, std::min(tl, tr));
			//t_max = std::min(t_max, std::max(tl, tr));
		} else if (l) {
			float tl = dist(lrst, line.o);
			if (t_max > tl) {
				t_max = tl;
				p_max = lrst;
			}
		} else if (r) {
			//t_max = std::min(t_max, dist(l ? lrst : rrst, line.o));
			float tr = dist(rrst, line.o);
			if (t_max > tr) {
				t_max = tr;
				p_max = rrst;
			}
		}

		flag |= l || r;
	}

	return (t_min < t_max) && flag && 
		x_range.x < p_max.x + EPS && x_range.y + EPS > p_max.x &&
		y_range.x < p_max.y + EPS && y_range.y + EPS > p_max.y &&
		z_range.x < p_max.z + EPS && z_range.y + EPS > p_max.z;
}
