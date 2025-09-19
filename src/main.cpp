#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <optional>

using namespace cv;
/*
static std::vector<std::vector<Point>> imgprocess(const Mat& img);
static std::vector<RotatedRect> init_select(const std::vector<std::vector<Point>>& contours);
static std::optional<std::pair<RotatedRect, RotatedRect>> look4pair(const std::vector<RotatedRect>& light_bars);
*/
static std::vector<std::vector<Point>> imgprocess(const Mat& img) {
	Mat grayed, hsved;
	cvtColor(img, hsved, COLOR_BGR2HSV);
	Mat gaussianed, medianed, binary, adaptive;
	GaussianBlur(hsved, gaussianed, Size(5, 5), 1.5);
	Mat final;
	Scalar lower_blue(90, 60, 200);
	Scalar upper_blue(111, 255, 255);
	inRange(gaussianed, lower_blue, upper_blue, final);
	Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	Mat postprocessed_1, postprocessed;
	morphologyEx(final, postprocessed_1, cv::MORPH_OPEN, kernel);
	morphologyEx(postprocessed_1, postprocessed, cv::MORPH_OPEN, kernel);
	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;
	findContours(postprocessed, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	return contours;
}

static std::vector<RotatedRect> init_select(const std::vector<std::vector<Point>>& contours) {
	std::vector<RotatedRect> light_bars;
	for (const auto& contour : contours) {
		double area = contourArea(contour);
		if (area < 100) continue;//这里将来需要随距离更改。
		RotatedRect rotated_bbox = minAreaRect(contour);
		double width = rotated_bbox.size.width;
		double height = rotated_bbox.size.height;
		if (width > height) {
			std::swap(width, height);
		}
		double aspect_ratio = height / width;
		if (aspect_ratio > 1.4 && aspect_ratio < 5.0) light_bars.push_back(rotated_bbox);
	}
	return light_bars;
}

static std::optional<std::pair<RotatedRect, RotatedRect>> look4pair(const std::vector<RotatedRect>& light_bars) {
	for (int i = 0; i < light_bars.size(); i++) {
		for (int j = i + 1; j < light_bars.size(); j++) {
			const RotatedRect& bar1 = light_bars.at(i);
			const RotatedRect& bar2 = light_bars.at(j);
			double angle1 = bar1.angle;
			double angle2 = bar2.angle;
			double differ = angle1 - angle2;
			double height_ratio = bar1.size.height / bar2.size.height;
			if (differ > 10 || differ < -10 || height_ratio >1.25 || height_ratio < 0.8) continue;
			double y_diff = std::abs(bar1.center.y - bar2.center.y);
			if (y_diff > bar1.size.height * 0.5) { // 如果垂直距离差太大
				continue;
			}
			double x_diff = std::abs(bar1.center.x - bar2.center.x);
			double avg_height = (bar1.size.height + bar2.size.height) / 2.0;
			// 小装甲板的间距大约是高度的2倍，大装甲板约3.5倍。
			if (x_diff < avg_height * 1.0 || x_diff > avg_height * 4.0) {
				continue;
			}
			std::pair <RotatedRect, RotatedRect> armor_pair= {bar1, bar2};
			return armor_pair;
		}
	}
	return std::nullopt;
}

int main() {
	std::string path = "../resources/test_image.jpg";
	cv::Mat img = imread(path);
	if (img.empty()) throw std::runtime_error("Could not read the image");
	std::vector<std::vector<Point>> contours = imgprocess(img);
	std::vector<RotatedRect> light_bars = init_select(contours);
	std::optional<std::pair<RotatedRect, RotatedRect>> found_armor_pair = look4pair(light_bars);
	if (found_armor_pair.has_value()) {
		std::pair<RotatedRect, RotatedRect> armor_pair = found_armor_pair.value();
		auto [barf, bars] = armor_pair;
		Rect armor_bbox = barf.boundingRect(); // 获取第一个灯条的包围盒
		armor_bbox |= bars.boundingRect(); // 与第二个灯条的包围盒取并集
		Mat img_result = img.clone();
		rectangle(img_result, armor_bbox, Scalar(0, 0, 225), 10);
		imshow("识别结果", img_result);
		imwrite("../resources/img_result.jpg", img_result);
		waitKey(0);
	}
	
}
