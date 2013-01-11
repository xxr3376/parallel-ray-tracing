#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <cstdio>
#include <cmath>

using namespace cv;
using namespace std;

int main(int argc, const char *argv[])
{
	if (argc !=3){
		printf("Wrong parameter, Please run in ./test pic1 pic2\n");
		exit(0);
	}
	Mat img1 = imread(argv[1]); 
	Mat img2 = imread(argv[2]); 
	if (img1.size().width != img2.size().width || img1.size().height != img2.size().height){
		printf("diffrent size, please check input files\n");
		exit(0);
	}
	int diff = 0;
	printf("img1.size.width = %d\n", img1.size().width);
	printf("img1.size.height = %d\n", img1.size().height);
	for (int i = 0; i < img1.size().width; i++){
		for (int j = 0; j < img1.size().height; j++){
			Vec3b c1 = img1.at<Vec3b>(j, i);
			Vec3b c2 = img2.at<Vec3b>(j, i);
			diff += abs(c1[0] - c2[0]);
			diff += abs(c1[1] - c2[1]);
			diff += abs(c1[2] - c2[2]);

		}
	}
	printf("Diff:%d\n", diff);
	return 0;
}
