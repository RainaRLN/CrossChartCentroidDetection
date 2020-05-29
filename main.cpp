#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <math.h>
#include <numeric>
using namespace cv;
using namespace std;

#define WIDTH  4000 // 6016
#define HEIGHT 3000  // s4512
#define THRESH 200


void get_cross(Point2f a1, Point2f a2, Point2f b1, Point2f b2, Point2f &cross)
{
		float ka, kb;
		ka = (a1.y - a2.y) / (a1.x - a2.x);
		kb = (b1.y - b2.y) / (b1.x - b2.x);

		
		if (-1 < abs(ka - kb) && abs(ka - kb) < 0.5 || ka == kb) {		
			cross.x = -1;
			cross.y = -1;
			return;
		}


		if (ka+1 == ka-1) {  // a垂直
			cross.x = a1.x;
			cross.y = b1.y;
		} else if (kb+1 == kb-1) {  // b垂直
			cross.x = b1.x;
			cross.y = a1.y;
		} else {
			cross.x = (ka*a1.x -a1.y - kb*b1.x + b1.y) / (ka - kb);
			cross.y = (ka*kb*(a1.x - b1.x) + ka*b1.y - kb*a1.y) / (ka - kb);
		}
		return;
}

void get_centroid(Mat gray, Point2f &centroid)
{
	namedWindow("gray", 0);
	imshow("gray", gray);

	Mat binary_chart;
	blur(gray, binary_chart, Size(3, 3));
	threshold(gray, binary_chart, THRESH, 255, THRESH_BINARY);
	namedWindow("binary", 0);
	imshow("binary", binary_chart);

	Point2f point[8];
	{
		int row = HEIGHT/3;
		int col = 0;
		while(gray.at<uchar>(row, col++) <= THRESH);
		point[0].x = col-1;
		point[0].y = row;
		circle(gray, Point(col-1, row), 5, Scalar(255, 255, 255), -1);

		while(gray.at<uchar>(row, col++) > THRESH);
		point[1].x = col-1;
		point[1].y = row;
		circle(gray, Point(col-1, row), 5, Scalar(255, 255, 255), -1);
	}
	{
		int row = HEIGHT*2/3;
		int col = 0;
		while(gray.at<uchar>(row, col++) <= THRESH);
		point[2].x = col-1;
		point[2].y = row;
		circle(gray, Point(col-1, row), 5, Scalar(255, 255, 255), -1);
		while(gray.at<uchar>(row, col++) > THRESH);
		point[3].x = col-1;
		point[3].y = row;
		circle(gray, Point(col-1, row), 5, Scalar(255, 255, 255), -1);
	}
	{
		int col = WIDTH/4;
		int row = 0;
		while(gray.at<uchar>(row++, col) <= THRESH);
		point[4].x = col;
		point[4].y = row-1;
		circle(gray, Point(col, row-1), 5, Scalar(255, 255, 255), -1);
		while(gray.at<uchar>(row++, col) > THRESH);
		point[5].x = col;
		point[5].y = row-1;
		circle(gray, Point(col, row-1), 5, Scalar(255, 255, 255), -1);
	}
	{
		int col = WIDTH*3/4;
		int row = 0;
		while(gray.at<uchar>(row++, col) <= THRESH);
		point[6].x = col;
		point[6].y = row-1;
		circle(gray, Point(col, row-1), 5, Scalar(255, 255, 255), -1);
		while(gray.at<uchar>(row++, col) > THRESH);
		point[7].x = col;
		point[7].y = row-1;
		circle(gray, Point(col, row-1), 5, Scalar(255, 255, 255), -1);
	}

	Point2f p1, p2;
	get_cross(point[0], point[2], point[4], point[6], p1);
	circle(gray, p1, 5, Scalar(0, 0, 0), -1);
	

	get_cross(point[1], point[3], point[5], point[7], p2);
	circle(gray, p2, 5, Scalar(0, 0, 0), -1);

	centroid.x = (p1.x + p2.x) / 2;
	centroid.y = (p1.y + p2.y) / 2;
}

void read_raw(const char * filename, int width, int height, Mat &raw_pic)
{
	unsigned int len = width * height;
	unsigned char *raw = new unsigned char[len];
	if(NULL == raw)
    {
        perror("malloc error");
    }

	FILE *fp = fopen(filename, "rb");
    if (fp < 0)
    {
        printf("Open frame data file failed\n");
        return;
    }
	fread(raw, 1, len, fp);
	fclose(fp);

	raw_pic = Mat(height, width, CV_8UC1, raw);
}

int main()
{
	Mat origin;
	
	// char pic_path[20];
	// for (int i = 1; i <= 15; i++) {
	// 	sprintf(pic_path, "pics/%d.raw", i);
	// 	read_raw(pic_path, WIDTH, HEIGHT, origin);

	// 	Point2f centroid;
	// 	get_centroid(origin, centroid);
	// 	cout << centroid << endl;

	// 	// 在图中标注中心和坐标
	// 	char x_y[30];
	// 	circle(origin, centroid, 5, Scalar(255, 255, 255), -1);
	// 	sprintf(x_y, "(%4.3f, %4.3f)", centroid.x, centroid.y);
	// 	putText(origin, x_y, Point(centroid.x+40, centroid.y-40), FONT_HERSHEY_PLAIN, 5, Scalar(255, 255, 255), 4);
		
	// 	namedWindow(pic_path, 0);
	// 	imshow(pic_path, origin);
	// }

	// read_raw("pics/17.raw", WIDTH, HEIGHT, origin);

	// Point2f centroid;
	// get_centroid(origin, centroid);
	// cout << centroid << endl;

	// // 在图中标注中心和坐标
	// char x_y[30];
	// circle(origin, centroid, 10, Scalar(0, 0, 0), -1);
	// sprintf(x_y, "(%4.3f, %4.3f)", centroid.x, centroid.y);
	// putText(origin, x_y, Point(centroid.x+40, centroid.y-40), FONT_HERSHEY_PLAIN, 5, Scalar(0, 0, 0), 4);
	
	// namedWindow("xxx.raw", 0);
	// imshow("xxx.raw", origin);

	char pic_path[100];
	for (int i = 0; i <= 6; i++) {
		sprintf(pic_path, "/media/raina/8G/Raw/4000X3000_TYPE102-x-%d.raw", i);
		read_raw(pic_path, WIDTH, HEIGHT, origin);

		Point2f centroid;
		get_centroid(origin, centroid);
		cout << centroid << endl;

		// 在图中标注中心和坐标
		char x_y[30];
		circle(origin, centroid, 5, Scalar(0, 0, 0), -1);
		sprintf(x_y, "(%4.3f, %4.3f)", centroid.x, centroid.y);
		putText(origin, x_y, Point(centroid.x+40, centroid.y-40), FONT_HERSHEY_PLAIN, 5, Scalar(0, 0, 0), 4);
		
		namedWindow(pic_path, 0);
		imshow(pic_path, origin);
	}


	while (waitKey() != 27); // Esc退出
	destroyAllWindows();
}
