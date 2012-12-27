#ifndef __COMMON_H__
#define __COMMON_H__
#include <vector>
#include <cstdio>
#include "triangle.h"
#include "attribute.h"
using std::vector;

#define EPS 1e-5

void read_obj_file(const char *file_name, vector<Triangle>& triangles, Attribute* attr);

inline int Min(int a, int b){
	return a > b ? b : a;
}

#endif //__COMMON_H__
