#include <opencv2\opencv.hpp>
#include <cmath>

using namespace cv;
using namespace std;

int main() {
	Mat input1, left1, right1, input2, left2, right2, disp1;

	// Entrance		Kurt	Room	Wall
	input1 = imread("Wall.jpg", CV_LOAD_IMAGE_COLOR);
	left1 = input1(Range::all(), Range(0, input1.cols / 2)).clone();
	right1 = input1(Range::all(), Range(input1.cols / 2, input1.cols)).clone();

	//imshow("Input", input1);
	//imshow("Left", left1);
	//imshow("Right", right1);

	/*
	VideoCapture camera1(0);// , camera2(2);
	camera1 >> input1;
	camera1 >> input1;
	

	camera2 >> input2;
	camera2 >> input2;

	while (true) {
		camera1 >> input1;
		//camera2 >> input2;

		left1 = input1(Range::all(), Range(0, input1.cols / 2)).clone();
		right1 = input1(Range::all(), Range(input1.cols / 2, input1.cols)).clone();
		/*left2 = input2(Range::all(), Range(0, input2.cols / 2)).clone();
		right2 = input2(Range::all(), Range(input2.cols / 2, input2.cols)).clone();

		if (!input1.empty()){
			imshow("Camera1", input1);
		}
		if (!left1.empty()) {
			imshow("Left1", left1);
		}
		if (!right1.empty()) {
			imshow("Right1", right1);
		}


		if (!input2.empty()) {
			imshow("Camera2", input2);
		}*
		if (!left2.empty()) {
			imshow("Left2", left2);
		}
		if (!right2.empty()) {
			imshow("Right2", right2);
		}

		if (waitKey(33) == 32) {
			break;
		}
	}
	*/
	
	
	//First Algorithm

	Mat imgLeft1, imgRight1;
	cvtColor(left1, imgLeft1, COLOR_BGR2GRAY);
	cvtColor(right1, imgRight1, COLOR_BGR2GRAY);


	//-- And create the image in which we will save our disparities
	Mat imgDisparity16S1 = Mat(imgLeft1.rows, imgLeft1.cols, CV_16S);
	Mat imgDisparity8U1 = Mat(imgLeft1.rows, imgLeft1.cols, CV_8UC1);

	if (imgLeft1.empty() || imgRight1.empty())
	{ std::cout << " --(!) Error reading images" << std::endl; return -1; }

	//-- 2. Call the constructor for StereoBM
	int ndisparities = 16 * 5;
	int SADWindowSize = 5;
	Ptr<StereoBM> sbm1 = StereoBM::create(ndisparities, SADWindowSize);

	while (waitKey(1) != 27) {
		//Refined Settings for first algorithm

		sbm1->setDisp12MaxDiff(1);
		sbm1->setSpeckleRange(8);
		sbm1->setSpeckleWindowSize(0);
		sbm1->setUniquenessRatio(0);
		sbm1->setTextureThreshold(507);
		sbm1->setMinDisparity(-39);
		sbm1->setPreFilterCap(61);
		sbm1->setPreFilterSize(5);

		//-- 3. Calculate the disparity image
		sbm1->compute(imgLeft1, imgRight1, imgDisparity16S1);

		//-- Check its extreme values
		double minVal1, maxVal1;

		minMaxLoc(imgDisparity16S1, &minVal1, &maxVal1);

		cout << "Min disp 1: " << minVal1 << " Max value1: " << maxVal1 << endl;

		//-- 4. Display it as a CV_8UC1 image
		imgDisparity16S1.convertTo(imgDisparity8U1, CV_8UC1, 255 / (maxVal1 - minVal1));

		//namedWindow("windowDisparity", WINDOW_NORMAL);
		imshow("disparity_LeftOnRight.jpg", imgDisparity8U1);
	}

	/*
	//Subtraction Checking

	Mat Difference1 = imgDisparity8U1 - imgDisparity8U2;
	Mat Difference2 = imgDisparity8U2 - imgDisparity8U1;

	imwrite("Diff1-2.jpg", Difference1);
	imwrite("Diff2-1.jpg", Difference2);
	*/

	/*
	//Point Confirmation with two passes
	int threshold = 100;
	Point place;
	Mat check = Mat(imgDisparity8U1.rows, imgDisparity8U1.cols, CV_8UC1, Scalar(0));
	for (int col = 0; col < imgDisparity8U1.cols; col++) {
		for (int row = 0; row < imgDisparity8U1.rows; row++) {
			place.x = col;
			place.y = row;
			if ((imgDisparity8U1.at<uchar>(place) > 0) && (imgDisparity8U2.at<uchar>(place) > 0)) {
				if (abs(imgDisparity8U1.at<uchar>(place) - imgDisparity8U2.at<uchar>(place)) < threshold) {
					check.at<uchar>(place) = imgDisparity8U1.at<uchar>(place);
				}
			}
		}
	}
	imwrite("check.jpg", check);
	*/

	/*
	Second Algorithm
	
	Mat left_img1, right_img1, left_img2, right_img2;

	cvtColor(left1, left_img1, COLOR_BGR2GRAY);
	cvtColor(right1, right_img1, COLOR_BGR2GRAY);

    int disparity_max = 256;

    std::vector<cv::Mat> cost_maps1, cost_maps2;


	//left on right
    for (int disparity = 0; disparity < disparity_max; disparity++){
        cv::Mat cost_map = cv::Mat(left_img1.size().height, left_img1.size().width, CV_8UC1);
        for (int row = 0; row < left_img1.size().height; row++){
            for (int col = 0; col < left_img1.size().width; col++){
                int col_disp = (col - disparity < 0) ? 0 : col - disparity;
                            //Absolute Difference
                cost_map.at<uchar>(row, col) = abs(left_img1.at<uchar>(row, col) - right_img1.at<uchar>(row, col_disp));
            }
        }
        cost_maps1.push_back(cost_map);
        imshow("Cost Map 1", cost_map);
		waitKey(50);
    }


    //Pick the smallest disparity
    cv::Mat final_cost_map1 = cv::Mat(left_img1.size().height, left_img1.size().width, CV_8UC1);
    for (int row = 0; row < final_cost_map1.size().height; row++){
        for (int col = 0; col < final_cost_map1.size().width; col++){

            int min = 65536;
            for (int disparity = 0; disparity < disparity_max; disparity++){
                min = (cost_maps1[disparity].at<uchar>(row, col) < min ? cost_maps1[disparity].at<uchar>(row, col) : min);
            }

            final_cost_map1.at<uchar>(row, col) = min;

        }
    }

	//right on left
	cvtColor(right1, left_img2, COLOR_BGR2GRAY);
	cvtColor(left1, right_img2, COLOR_BGR2GRAY);

	for (int disparity = 0; disparity < disparity_max; disparity++) {
		cv::Mat cost_map = cv::Mat(left_img2.size().height, left_img2.size().width, CV_8UC1);
		for (int row = 0; row < left_img2.size().height; row++) {
			for (int col = 0; col < left_img2.size().width; col++) {
				int col_disp = (col - disparity < 0) ? 0 : col - disparity;
				//Absolute Difference
				cost_map.at<uchar>(row, col) = abs(left_img2.at<uchar>(row, col) - right_img2.at<uchar>(row, col_disp));
			}
		}
		cost_maps2.push_back(cost_map);
		imshow("Cost Map 2", cost_map);
		waitKey(50);
	}


	//Pick the smallest disparity
	cv::Mat final_cost_map2 = cv::Mat(left_img2.size().height, left_img2.size().width, CV_8UC1);
	for (int row = 0; row < final_cost_map2.size().height; row++) {
		for (int col = 0; col < final_cost_map2.size().width; col++) {

			int min = 65536;
			for (int disparity = 0; disparity < disparity_max; disparity++) {
				min = (cost_maps2[disparity].at<uchar>(row, col) < min ? cost_maps2[disparity].at<uchar>(row, col) : min);
			}

			final_cost_map2.at<uchar>(row, col) = min;

		}
	}

    //Show the final cost map, multiplied by 32 to show pixels with low value
    imshow("Final Map 1", final_cost_map1);
	imshow("Final Map 2", final_cost_map2);
	cv::Mat finalMap = final_cost_map1 - final_cost_map2;
	imshow("Final Map", finalMap);
	*/
	waitKey(0);

	destroyAllWindows();
	//camera1.release();
	//camera2.release();
	return 0;
}