#include <iostream>
#include <string>
using namespace std;

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
using namespace cv;


void use_houghLines()
{
	Mat origin_chart = imread("pics/chart.png");
	Mat gray_chart;
	cvtColor(origin_chart, gray_chart, COLOR_BGR2GRAY);
	int height = gray_chart.cols;
	int width = gray_chart.rows;
	// imshow("Gray chart", gray_chart);

	Mat binary_chart;
	threshold(gray_chart, binary_chart, 80, 255, THRESH_BINARY);
	// imshow("Binary chart", binary_chart);

	Mat canny_chart;
	Canny(binary_chart, canny_chart, 40, 120, 3);
	// imshow("Canny chart", canny_chart);

	vector<Vec4i> lines;
	HoughLinesP(canny_chart, lines, 1, CV_PI/180, 150, 150, 80);

	for (size_t i = 0; i < lines.size(); i++) {
		Vec4i ln = lines[i];
		line(origin_chart, Point(ln[0], ln[1]), Point(ln[2], ln[3]), Scalar(0, 0, 255), 1, LINE_AA);
	}
	// imshow("Hough chart", origin_chart);

	Point2f crosspoint[4];
	int p = 0;
	for (size_t i = 0; i < lines.size(); i++) {
		for (size_t j = i+1; j < lines.size(); j++) {
			double ka, kb;
			ka = (double)(lines[i][3] - lines[i][1]) / (double)(lines[i][2] - lines[i][0]);
			kb = (double)(lines[j][3] - lines[j][1]) / (double)(lines[j][2] - lines[j][0]);
			
			if (-1 < abs(ka - kb) && abs(ka - kb) < 0.5 || ka == kb) {
				continue;
			}
			// printf("ka = %f, kb = %f\n", ka, kb);

			if (ka+1 == ka-1) {
				crosspoint[p].x = lines[i][0];
				crosspoint[p].y = lines[j][1];
			} else if (kb+1 == kb-1) {
				crosspoint[p].x = lines[j][0];
				crosspoint[p].y = lines[i][1];
			} else {
				crosspoint[p].x = (ka*lines[i][0] -lines[i][1] - kb*lines[j][0] + lines[j][1]) / (ka - kb);
				crosspoint[p].y = (ka*kb*(lines[i][0] - lines[j][0]) + ka*lines[j][1] - kb*lines[i][1]) / (ka - kb);
			}
			circle(origin_chart, crosspoint[p], 2, Scalar(255, 0, 0), -1);
			p++;
		}
	}

	float x_sum = 0;
	float y_sum = 0;
	for (int i = 0; i < 4; i++) {
		x_sum += crosspoint[i].x;
		y_sum += crosspoint[i].y;
	}
	int x = x_sum / 4;
	int y = y_sum / 4;
	circle(origin_chart, Point(x, y), 2, Scalar(0, 255, 0), -1);
	char x_y[13];
	sprintf(x_y, "(%4d, %4d)", x, y);
	putText(origin_chart, x_y, Point(x+40, y-40), FONT_HERSHEY_PLAIN, 0.7, Scalar(0, 255, 0));

	imshow("Centre chart", origin_chart);
}

Mat use_Shi_Tomasi(Mat origin_chart)
{
	// Mat bf_chart;
	// bilateralFilter(origin_chart, bf_chart, 5, 10, 10);
	// imshow("BF", bf_chart);
	// Mat enhance_chart;
	// Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, 0, 5, 0, 0 , -1, 0);
	// filter2D(bf_chart, enhance_chart, CV_8UC3, kernel);
	// imshow("enhance_chart", enhance_chart);

	Mat gray_chart;
	cvtColor(origin_chart, gray_chart, COLOR_BGR2GRAY);
	int height = gray_chart.cols;
	int width = gray_chart.rows;
	// imshow("Gray chart", gray_chart);

	Mat binary_chart;
	threshold(gray_chart, binary_chart, 80, 255, THRESH_BINARY);
	// imshow("Binary chart", binary_chart);

	// Mat canny_chart;
	// Canny(binary_chart, canny_chart, 40, 120, 3);
	// imshow("Canny chart", canny_chart);



	vector<Point2f> corners;
	int max_corners = 10;
	double quality_level = 0.05;
	double min_distance = 35;
	int block_size = 3;
	bool use_harris = true;
	double k = 0.04;

	Mat roi = Mat::zeros(width, height, CV_8UC1);
	rectangle(roi, Rect(170, 170, 90, 90), Scalar(255, 255, 255), -1);
	// imshow("roi", roi);

	goodFeaturesToTrack(binary_chart, corners, max_corners, quality_level, 
							min_distance, roi, block_size, use_harris, k);
	
	for (size_t i = 0; i < corners.size(); i++) {
		circle(origin_chart, corners[i], 4, Scalar(255, 0, 0), -1);
	}

	if (corners.size() == 4) {
		float x_sum = 0;
		float y_sum = 0;
		for (int i = 0; i < 4; i++) {
			x_sum += corners[i].x;
			y_sum += corners[i].y;
		}
		int x = x_sum / 4;
		int y = y_sum / 4;
		circle(origin_chart, Point(x, y), 4, Scalar(0, 0, 255), -1);
		char x_y[13];
		sprintf(x_y, "(%4d, %4d)", x, y);
		putText(origin_chart, x_y, Point(x+40, y-40), FONT_HERSHEY_PLAIN, 0.9, Scalar(0, 0, 255));
	}
	
	return origin_chart;
}

int main(int argc, const char **argv)
{
	use_houghLines();

	char pic_path[20];
	for (int i = 1; i < 10; i++) {
		sprintf(pic_path, "pics/%d.png", i);
		Mat origin_chart = imread(pic_path);
		imshow(pic_path, use_Shi_Tomasi(origin_chart));
	}

	while (waitKey() != 27);  // ESC
	return 0;
}

