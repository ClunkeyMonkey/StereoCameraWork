#include "undistortor.h"



un::undistortor::undistortor(char c)
{
	std::string setting_file;
	if (c == 'L') {
		setting_file = "LeftData.xml";
	}
	else if (c == 'R') {
		setting_file = "RightData.xml";
	}
	
	cv::FileStorage fs;
	fs.open(setting_file, cv::FileStorage::READ);
	if (!fs.isOpened()) {
		std::cout << "Could not open file." << std::endl;
	}

	fs["camera_matrix"] >> camera_matrix;
	fs["distortion_coefficients"] >> distortion_coefficients;

	fs.release();

	cv::initUndistortRectifyMap(camera_matrix, distortion_coefficients, cv::Mat(), camera_matrix, cv::Size(1280, 960), CV_32FC1, map_1, map_2);

}


un::undistortor::~undistortor()
{
}


const bool un::undistortor::fix(const cv::Mat & in1, cv::Mat * out1)
{
	
	cv::remap(in1, *out1, map_1, map_2, cv::INTER_LINEAR);
	
	return false;
}
