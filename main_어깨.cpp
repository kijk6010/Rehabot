#include <iostream>
#include "opencv2/opencv.hpp"

#define CV_TERMCRIT_ITER    1
#define CV_TERMCRIT_EPS     2

using namespace std;
using namespace cv;


int prev_height = 0;
int max_height = 0;
int min_height = 0;
int object_loc = 0;
int detected_count = 0;
bool count_determiner1 = true;
bool count_determiner2 = true;
bool count_determiner3 = true;
bool determiner_holder = false;
int changed = 0;
int frame_index = 0;

int main()
{
	system("mode con:cols=14 lines=14");

	VideoCapture cap;
	Mat img_frame, img_frame2, img_hsv, img_hsv2, img_hsv3;

	cap.open(-1);
	//OR advance usage : select any API backend
	int deviceID = 0;              //0 = open default camera
	int apiID = cv::CAP_ANY;      // 0 = autodetect default API
						   //open selected camera using selected API


	cap.open(deviceID + apiID);

	//cap.open("C:\\Users\\xogh9\\Desktop\\healthcare1\\COLOR_DETECTION_PYTHON\\test3.mp4");

	if (!cap.isOpened()) {
		cerr << "ERROR! Unable to open camera\n";
		cin.get();
		return -1;
	}

	VideoWriter outputVideo;
	outputVideo.open("ouput.avi", VideoWriter::fourcc('X', 'V', 'I', 'D'), 30,
		Size(2400, 1000),
		true);
	if (!outputVideo.isOpened())
	{
		cout << "동영상을 저장하기 위한 초기화 작업 중 에러 발생" << endl;
		return 1;
	}
	// 이미지 높이 구하기
	cap.read(img_frame);
	Size s = img_frame.size();
	min_height = s.height;


	// check if we succeeded
	if (img_frame.empty() && img_frame2.empty()) {
		cerr << "ERROR! blank frame grabbed\n";
		return 0;
	}


	for (;;)
	{
		// wait for a new frame from camera and store it into 'frame'
		cap.read(img_frame);
		cap.read(img_frame2);

		//rotate(img_frame, img_frame, ROTATE_90_CLOCKWISE);

		// check if we succeeded
		if (img_frame.empty() && img_frame2.empty()) {
			cerr << "ERROR! blank frame grabbed\n";
			break;
		}


		frame_index++;


		//HSV로 변환
		cvtColor(img_frame, img_hsv, COLOR_BGR2HSV);
		cvtColor(img_frame2, img_hsv2, COLOR_BGR2HSV);

		//지정한 HSV 범위를 이용하여 영상을 이진화
		Mat img_mask1, img_mask2, img_mask3;

		inRange(img_hsv, Scalar(160, 100, 129), Scalar(255, 255, 255), img_mask1);
		inRange(img_hsv2, Scalar(22, 62, 125), Scalar(161, 255, 255), img_mask2);



		// 모폴로지 연산 침식 / 팽창

		//morphological opening 작은 점들을 제거 
		erode(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));


		//morphological closing 영역의 구멍 메우기 
		dilate(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		vector<vector<Point> > contours;
		// 윤곽선 찾기
		findContours(img_mask1, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		vector<Point2f> approx;
		Mat img_result = img_frame.clone();
		determiner_holder = count_determiner1;

		// 찾은 윤곽선 개수만큼 이미지에 그리기

		for (size_t i = 0; i < contours.size(); i++)
		{
			approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

			//printf("%f \n", fabs(contourArea(Mat(approx))));
			if (fabs(contourArea(Mat(approx))) > 1000)  /// 면적이 일정크기 이상이어야 한다. 
			{
				if (fabs(contourArea(Mat(approx))) < 30000)   /// 면적이 일정크기 미만이어야 한다.
				{
					rectangle(img_result, boundingRect(contours[i]), Scalar(0, 0, 255), 2, 8, 0);
					Rect Result = boundingRect(contours[i]);


					//printf("pink: %d,%d\n", Result.x, Result.y);
					line(img_result, Point(0, 30), Point(img_result.rows, 30), Scalar(0, 0, 255), 3);
					line(img_result, Point(0, 350), Point(img_result.rows, 350), Scalar(0, 0, 255), 3);
					circle(img_result, Point(Result.x, Result.y), 1, Scalar(255, 255, 255), 2);
					imshow("pink인식", img_result);
					moveWindow("pink인식", 900, 500);
					if (count_determiner1)
					{
						/// 위인지 아래인지 결정하는 바운더리.

						// 바운더리 넘어가면 찾고 내려갈때까지 기다립니다
						if (Result.y > 350)
						{
							printf("UP!1");
							count_determiner1 = false;
						}
					}
					else
					{
						// 반대.
						if (Result.y < 30)
						{
							printf("Down!1");

							min_height = Result.y;
							count_determiner1 = true;
						}

					}

				}

			}
		}
		erode(img_mask2, img_mask2, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(img_mask2, img_mask2, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));


		//morphological closing 영역의 구멍 메우기 
		dilate(img_mask2, img_mask2, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(img_mask2, img_mask2, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		vector<vector<Point> > contours2;
		// 윤곽선 찾기
		findContours(img_mask2, contours2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		vector<Point2f> approx2;
		Mat img_result2 = img_frame2.clone();
		determiner_holder = count_determiner2;
		// 찾은 윤곽선 개수만큼 이미지에 그리기

		for (size_t i = 0; i < contours2.size(); i++)
		{
			approxPolyDP(Mat(contours2[i]), approx2, arcLength(Mat(contours2[i]), true)*0.02, true);

			//printf("%f \n", fabs(contourArea(Mat(approx))));
			if (fabs(contourArea(Mat(approx2))) > 1000)  /// 면적이 일정크기 이상이어야 한다. 
			{
				if (fabs(contourArea(Mat(approx2))) < 30000)   /// 면적이 일정크기 미만이어야 한다.
				{
					rectangle(img_result2, boundingRect(contours2[i]), Scalar(0, 0, 255), 2, 8, 0);
					Rect Result = boundingRect(contours2[i]);


					// printf("green: %d, %d\n", Result.x, Result.y);

					//rectangle(img_result, Point(295, 80), Point(15, 146), Scalar(255, 200, 200), 3);
					circle(img_result2, Point(Result.x, Result.y), 1, Scalar(255, 255, 255), 2);
					line(img_result2, Point(0, 355), Point(img_result.rows, 355), Scalar(0, 0, 255), 3);
					line(img_result2, Point(0, 100), Point(img_result.rows, 100), Scalar(0, 0, 255), 3);
					imshow("green인식", img_result2);
					moveWindow("green인식", 300, 500);
					if (count_determiner2)
					{
						/// 위인지 아래인지 결정하는 바운더리.

						// 바운더리 넘어가면 찾고 내려갈때까지 기다립니다
						if (Result.y > 355)
						{
							printf("UP!2");
							count_determiner2 = false;
						}
					}
					else
					{
						// 반대.
						if (Result.y < 100)
						{
							printf("Down!2");

							min_height = Result.y;
							count_determiner2 = true;
						}

					}

				}

			}
		}


		putText(img_frame, "Count : " + std::to_string(detected_count), Point(300, 100), cv::QT_FONT_NORMAL, 2, cv::Scalar(255, 0, 0), 3, 1);
		imshow("원본", img_frame);
		moveWindow("재활운동", 500, 0);
		// 높이가 아래에서 위로 바뀌면 카운트, 위에서 아래바뀌면 카운트
		//printf("%d \n", changed);

		if (determiner_holder != count_determiner1 && determiner_holder != count_determiner2)
		{
			changed += 1;
			// 아래에서 위, 위에서 아래로 왔을때 디텍션 됨.
			if (changed % 2 == 0)
			{
				detected_count += 1;
				printf("%d\n", detected_count);
				count_determiner1 = count_determiner2 = false;
			}

		}



		if (waitKey(1) >= 0)
			break;

	}

	return 0;

}