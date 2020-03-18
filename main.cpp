#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <math.h>
#include <numeric>
using namespace cv;
using namespace std;

#define WIDTH 6016
#define HEIGHT 4512
#define ANGLE 1

void get_centroid(Mat gray, Point2f &centroid)
{
	// namedWindow("gray", 0);
	// imshow("gray", gray);

	Mat binary;
	threshold(gray, binary, 80, 255, THRESH_BINARY);
	// namedWindow("Binary", 0);
	// imshow("Binary", binary);
	blur(binary, binary, Size(3, 3));

	vector<float> x1_cluster;
	vector<float> x2_cluster;
	vector<float> y1_cluster;
	vector<float> y2_cluster;

	for (int row = (HEIGHT/4); row < (HEIGHT/3); row += HEIGHT/120) {
		int col = 0;
		while(binary.at<uchar>(row, col++) == 0);
		x1_cluster.push_back(col-1);
		// circle(gray, Point(col-1, row), 5, Scalar(255, 255, 255), -1);
		while(binary.at<uchar>(row, col++) > 0);
		x1_cluster.push_back(col-1);
		// circle(gray, Point(col-1, row), 5, Scalar(255, 255, 255), -1);
	}
	for (int row = (HEIGHT*2/3); row < (HEIGHT*3/4); row += HEIGHT/120) {
		int col = 0;
		while(binary.at<uchar>(row, col++) == 0);
		x2_cluster.push_back(col-1);
		// circle(gray, Point(col-1, row), 5, Scalar(255, 255, 255), -1);
		while(binary.at<uchar>(row, col++) > 0);
		x2_cluster.push_back(col-1);
		// circle(gray, Point(col-1, row), 5, Scalar(255, 255, 255), -1);
	}
	for (int col = (WIDTH/4); col < (WIDTH/3); col += WIDTH/120) {
		int row = 0;
		while(binary.at<uchar>(row++, col) == 0);
		y1_cluster.push_back(row-1);
		// circle(gray, Point(col, row-1), 5, Scalar(255, 255, 255), -1);
		while(binary.at<uchar>(row++, col) > 0);
		y1_cluster.push_back(row-1);
		// circle(gray, Point(col, row-1), 5, Scalar(255, 255, 255), -1);
	}
	for (int col = (WIDTH*2/3); col < (WIDTH*3/4); col += WIDTH/120) {
		int row = 0;
		while(binary.at<uchar>(row++, col) == 0);
		y2_cluster.push_back(row-1);
		// circle(gray, Point(col, row-1), 5, Scalar(255, 255, 255), -1);
		while(binary.at<uchar>(row++, col) > 0);
		y2_cluster.push_back(row-1);
		// circle(gray, Point(col, row-1), 5, Scalar(255, 255, 255), -1);
	}

	float x1 = static_cast<float>(accumulate(x1_cluster.begin(), x1_cluster.end(), 0)) / static_cast<float>(x1_cluster.size());
	float x2 = static_cast<float>(accumulate(x2_cluster.begin(), x2_cluster.end(), 0)) / static_cast<float>(x2_cluster.size());
	float y1 = static_cast<float>(accumulate(y1_cluster.begin(), y1_cluster.end(), 0)) / static_cast<float>(y1_cluster.size());
	float y2 = static_cast<float>(accumulate(y2_cluster.begin(), y2_cluster.end(), 0)) / static_cast<float>(y2_cluster.size());

	centroid = Point2f((x1+x2)/2.0, (y1+y2)/2.0);

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

	raw_pic = Mat(HEIGHT, WIDTH, CV_8UC1, raw);
}

int main()
{
	Mat origin;
	read_raw("pics/3.raw", WIDTH, HEIGHT, origin);

	Point2f centroid;
	get_centroid(origin, centroid);
	cout << centroid << endl;

	// 在图中标注中心和坐标
	char x_y[30];
	circle(origin, centroid, 5, Scalar(255, 255, 255), -1);
	sprintf(x_y, "(%4.3f, %4.3f)", centroid.x, centroid.y);
	putText(origin, x_y, Point(centroid.x+40, centroid.y-40), FONT_HERSHEY_PLAIN, 5, Scalar(255, 255, 255), 4);
	
	namedWindow("Result", 0);
	imshow("Result", origin);


	while (waitKey() != 27); // Esc退出
	destroyAllWindows();
}
