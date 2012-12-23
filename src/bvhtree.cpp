#include "bvhtree.h"

#include <algorithm>
#include <cfloat>

const int BVHTree::BLOCK_SIZE = 32;

bool BVHTree::is_leaf() {
	return l == r;
}

void BVHTree::create_tree(int n, Triangle *triangles) {
	if (triangles != NULL) this->triangles = triangles;

	box.calc_box( this->triangles, n);
	_create(0, n - 1);
}

BVHBox _piece_box [BLOCK_SIZE];
void BVHTree::_create(int l, int r) {
	float result = FLT_MAX;
	int split_i = 0, type = 0;

	//x
	
	for (int split_type = 0; split_type < 3; ++ split_type) {
		float block_length;
		switch (split_type) {
			case 0 :
				block_length = ( box.x_range.y - box.x_range.x ) / BLOCK_SIZE;
				break;
			case 1 :
				block_length = ( box.y_range.y - box.y_range.x ) / BLOCK_SIZE;
				break;
			case 2 :
				block_length = ( box.z_range.y - box.z_range.x ) / BLOCK_SIZE;
		}
		
		if (block_length > 0) {
			for (int i = 0; i < BLOCK_SIZE; ++i)
				_piece_box[i].clear();

			for (int i = l; i <= r; ++i) {
				int t = _get_diff(triangles[i], split_type);
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

	do	{
		li = 
		ri = 
	}	while();
}

int BVHTree::_get_diff(const Triangle& t, int type) {
	float result = 0;
	case (type) {
		case 0:
			result = (t.x - box.x_range.x);
			break;
		case 1:
			result = (t.y - box.y_range.x);
			break;
		case 2:
			result = (t.z - box.z_range.x);
	}
	return std::min(int(result / BLOCK_SIZE), BLOCK_SIZE - 1);
}

void BVHTree::_refresh(int& result, int& type, int& split_i, int t) {
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
	add(t.A);
	add(t.B);
	add(t.C);
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

void merge(const BVHBox& b) {
	count += b.count;
	x_range.x = std::min(x_range.x, b.x_range.x);
	x_range.y = std::max(x_range.y, b.x_range.y);
	y_range.x = std::min(y_range.x, b.y_range.x);
	y_range.y = std::max(y_range.y, b.y_range.y);
	z_range.x = std::min(z_range.x, b.z_range.x);
	z_range.y = std::max(z_range.y, b.z_range.y);
}
