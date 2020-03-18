#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include<iostream>
#include<math.h>
using namespace cv;
using namespace std;

#define WIDTH 6016
#define HEIGHT 4512
#define ANGLE 1

Point get_centroid(Mat gray)
{
	// namedWindow("gray", 0);
	// imshow("gray", gray);

	Mat binary;
	threshold(gray, binary, 80, 255, THRESH_BINARY);
	namedWindow("Binary", 0);
	imshow("Binary", binary);
	Mat src;
	Canny(binary, src, 3, 60, 3);
	convertScaleAbs(src, src);
	// namedWindow("Canny", 0);
	// imshow("Canny", src);

	int height = src.rows;
	int width = src.cols;

	Mat sum_row;
	Mat sum_col;

	reduce(src, sum_row, 1, 0, CV_32S);
	reduce(src, sum_col, 0, 0, CV_32S);

	Point x1, x2, y1, y2;

	minMaxLoc(sum_row, nullptr, nullptr, nullptr, &y1);
	minMaxLoc(sum_col, nullptr, nullptr, nullptr, &x1);

	sum_row.at<float>(y1.y, 0) = 0;
	sum_col.at<float>(0, x1.x) = 0;


	minMaxLoc(sum_row, nullptr, nullptr, nullptr, &y2);
	minMaxLoc(sum_col, nullptr, nullptr, nullptr, &x2);

	int x = (x1.x + x2.x) / 2;
	int y = (y1.y + y2.y) / 2;
	return Point(x, y);

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
	read_raw("pics/2.raw", WIDTH, HEIGHT, origin);

	// 旋转矩阵
	Mat M = getRotationMatrix2D(Point(WIDTH/2, HEIGHT/2), ANGLE, 1);

	Mat temp;
	warpAffine(origin, temp, M, Size(WIDTH, HEIGHT));

	Point p = get_centroid(temp);  // 旋转后的中心
	circle(temp, p, 5, Scalar(255, 255, 255), -1);
	// namedWindow("Temp", 0);
	// imshow("Temp", temp);
	

	// 求原坐标
	cv::Point centroid;
	p.x -= M.at<double>(0,2);
	p.y -= M.at<double>(1,2);

	// 逆矩阵
	Mat M_inverse(M, Rect(0,0,2,2)); 
	invert(M_inverse, M_inverse);

    centroid.x = M_inverse.at<double>(0,0) * p.x + M_inverse.at<double>(0,1) * p.y;
    centroid.y = M_inverse.at<double>(1,0) * p.x + M_inverse.at<double>(1,1) * p.y;
	cout << centroid.x << " " << centroid.y << endl;

	// 在图中标注中心和坐标
	char x_y[13];
	circle(origin, centroid, 5, Scalar(255, 255, 255), -1);
	sprintf(x_y, "(%4d, %4d)", centroid.x, centroid.y);
	putText(origin, x_y, Point(centroid.x+40, centroid.y-40), FONT_HERSHEY_PLAIN, 5, Scalar(255, 255, 255), 4);
	
	namedWindow("Result", 0);
	imshow("Result", origin);


	while (waitKey() != 27); // Esc退出
	destroyAllWindows();
}
