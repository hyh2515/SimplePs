#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core/hal/intrin.hpp>
using cv::Mat;

class ToolUtility
{
public:
	static void Mat2CImage(const Mat& mat, CImage& cImage);
	static Mat ToolUtility::CImage2Mat(CImage cimg);
};

