#include "common.h"

void read_obj_file(const char *file_name, vector<Triangle>& triangles) {
	static const int BUFFER_SIZE = 10000;

	FILE *fin = fopen(file_name, "r");
	char buffer[BUFFER_SIZE];
	vector<Vector3> points;
	vector<Vector3> texture;
	triangles.clear();
		
	while (fgets(buffer, BUFFER_SIZE - 1 , fin))
		if (buffer[1] == ' ') {
			if (buffer[0] == 'v') {
				Vector3 v;
				sscanf(buffer + 1, "%f%f%f", &v.x, &v.y, &v.z);
				points.push_back(v);
			} else if (buffer[0] == 'f') {
				int id[6];
				for (int i = 0, j = 1; i < 3; ++i) {
					while (buffer[j] != ' ') ++j;
					if (texture.size())
						sscanf(buffer + j, "%d/%d", id + i, id + i + 3);
					else
						sscanf(buffer + j, "%d", id + i);
					++j;
				}
				if (texture.size())
					triangles.push_back( Triangle(points[ id[0] - 1 ], points[ id[1] - 1 ], points[ id[2] - 1 ], texture[id[3] - 1], texture[id[4] - 1], texture[id[5] - 1]));
				else
					triangles.push_back( Triangle(points[ id[0] - 1 ], points[ id[1] - 1 ], points[ id[2] - 1 ]) );
			}
		}
		else if(buffer[1] == 't' || buffer[0] == 'v'){
			Vector3 vt;
			sscanf(buffer + 2, "%f%f%f", &vt.x, &vt.y, &vt.z);
			texture.push_back(vt);
		}

	
	fclose(fin);
}
