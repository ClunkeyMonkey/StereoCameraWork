#include <opencv2\opencv.hpp>
#include <cmath>
#include "undistortor.h"

cv::RNG rng(cv::getTickCount());

int main() {
	cv::Mat A, A_Left, A_Right;
	cv::Mat B, B_Left, B_Right;
	cv::Mat A_Disp_16S, A_Disp_8U;
	cv::Mat B_Disp_16S, B_Disp_8U;

	A = cv::imread("images/A.jpg", CV_LOAD_IMAGE_COLOR);
	B = cv::imread("images/B.jpg", CV_LOAD_IMAGE_COLOR);

	std::cout << "Images Opened" << std::endl;

	A_Left = A(cv::Range::all(), cv::Range(0, A.cols / 2)).clone();
	A_Right = A(cv::Range::all(), cv::Range(A.cols / 2, A.cols)).clone();

	B_Left = B(cv::Range::all(), cv::Range(0, B.cols / 2)).clone();
	B_Right = B(cv::Range::all(), cv::Range(B.cols / 2, B.cols)).clone();
	
	cv::imwrite("output/Split/A_Left.jpg", A_Left);
	cv::imwrite("output/Split/A_Right.jpg", A_Right);

	cv::imwrite("output/Split/B_Left.jpg", B_Left);
	cv::imwrite("output/Split/B_Right.jpg", B_Right);

	std::cout << "Images Split" << std::endl;
	
	un::undistortor leftD('L');
	un::undistortor rightD('R');

	leftD.fix(A_Left, &A_Left);
	rightD.fix(A_Right, &A_Right);

	std::cout << "A Corrected" << std::endl;
	
	cv::imwrite("output/Corrected/A_Left.jpg", A_Left);
	cv::imwrite("output/Corrected/A_Right.jpg", A_Right);

	leftD.fix(B_Left, &B_Left);
	rightD.fix(B_Right, &B_Right);

	std::cout << "B Corrected" << std::endl;

	cv::imwrite("output/Corrected/B_Left.jpg", B_Left);
	cv::imwrite("output/Corrected/B_Right.jpg", B_Right);

	A_Disp_16S = cv::Mat(A_Left.rows, A_Left.cols, CV_16S);
	A_Disp_8U = cv::Mat(A_Left.rows, A_Left.cols, CV_8UC1);

	B_Disp_16S = cv::Mat(B_Left.rows, B_Left.cols, CV_16S);
	B_Disp_8U = cv::Mat(B_Left.rows, B_Left.cols, CV_8UC1);

	int minDisparity = 0, numDisparities = 16 * 10, SADWindowSize = 5;

	cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(minDisparity, numDisparities, SADWindowSize, 8 * A_Left.channels() * SADWindowSize * SADWindowSize, 32 * A_Left.channels() * SADWindowSize * SADWindowSize, floor(sqrt(numDisparities)), 0, 9, 200, 2);

	std::cout << "Calculating Disparity of A" << std::endl;

	sgbm->compute(A_Left, A_Right, A_Disp_16S);
	A_Disp_16S = A_Disp_16S / 16;
	A_Disp_16S.convertTo(A_Disp_8U, CV_8UC1);

	cv::imwrite("output/DisparityMaps/A_Disp.jpg", A_Disp_8U);
	
	std::cout << "A_Disp completed" << std::endl;

	std::cout << "Calculating Disparity of B" << std::endl;

	sgbm->compute(B_Left, B_Right, B_Disp_16S);
	B_Disp_16S = B_Disp_16S / 16;
	B_Disp_16S.convertTo(B_Disp_8U, CV_8UC1);

	cv::imwrite("output/DisparityMaps/B_Disp.jpg", B_Disp_8U);

	std::cout << "B_Disp completed" << std::endl;

	std::system("PAUSE");
	cv::destroyAllWindows();
	return 0;
}