#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;

int main() {
	cv::Mat img = imread("D:\\RMVcode\\opencv_project\\opencv_project\\test_image.jpg");
	if(img.empty()) throw std::runtime_error("Could not read the image");
	Mat grayed, hsved;
	cvtColor(img, hsved, COLOR_BGR2HSV); 
	Mat gaussianed, medianed, binary, adaptive;
	GaussianBlur(hsved, gaussianed, Size(5, 5), 1.5);
	//adaptiveThreshold(gaussianed, adaptive, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 2 );
	/*Mat mask1, mask2;
	Scalar lower_red(0, 100, 100);
	Scalar upper_red(10, 255, 255);
	inRange(hsved, lower_red, upper_red, mask1);
	Scalar lower_red2(160, 100, 100);
	Scalar upper_red2(179, 255, 255);
	inRange(gaussianed, lower_red2, upper_red2, mask2);
	Mat final = mask1 + mask2;*/
	Mat final;
	Scalar lower_blue(90, 60, 200);
	Scalar upper_blue(111, 255, 255);
	inRange(gaussianed, lower_blue, upper_blue, final);
	Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	Mat postprocessed_1,postprocessed;
	morphologyEx(final, postprocessed_1, cv::MORPH_OPEN, kernel);
	morphologyEx(postprocessed_1, postprocessed, cv::MORPH_OPEN, kernel);
	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;
	findContours(postprocessed, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	if (!contours.empty()) {
		double max_area = 0;
		int max_area_idx = -1;
		/*for (int i = 0; i < contours.size(); i++) {
			double area = contourArea(contours[i]);
			if (area > max_area) {
				max_area = area;
				max_area_idx = i;
			}
		}

		if (max_area_idx != -1) {
			// 计算最大轮廓的矩和 (Moments)
			Moments M = moments(contours[max_area_idx]);

			// 计算质心 (Centroid) -> 这就是我们需要的智能种子点！
			// 确保 M.m00 不为零，避免除零错误
			if (M.m00 > 0) {
				Point seedPoint(M.m10 / M.m00, M.m01 / M.m00);
				Scalar newGrayValue(0);  // 将高光区域填充为黑色
				Scalar loDiff(10);       // 向下容差
				Scalar upDiff(10);       // 向上容差
				int flags = 8 | FLOODFILL_FIXED_RANGE;

				floodFill(highlight, seedPoint, newGrayValue, 0, loDiff, upDiff, flags);
			}
		}*/
	}
	/*Point2f seedPoint();
	Scalar newGrayValue(255);
	Scalar loDiff(30);
	Scalar upDiff(30);
	int flags = 8 | FLOODFILL_FIXED_RANGE;
	floodFill(hl_postprocessed, seedPoint, newGrayValue, 0, loDiff, upDiff, flags);*/
	Mat img_result = img.clone();
	//for (size_t i = 0; i < contours.size(); i++) {
		//drawContours(img_result, contours, (int)i, Scalar(0, 255, 0), 2);
	//}
	/*double max_area = 0;
	int max_area_idx = 0;
	int sec_area_idx = 0;*/
	for (size_t i = 0; i < contours.size(); i++) {
		double area = contourArea(contours[i]);
		if (area < 1200) continue;
		Rect bbox = boundingRect(contours[i]);
		rectangle(img_result, bbox, Scalar(0, 0, 255), 2);
	}

	
	imshow("识别结果", img_result);
	waitKey(0);
}
