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
		cout << "�������� �����ϱ� ���� �ʱ�ȭ �۾� �� ���� �߻�" << endl;
		return 1;
	}
	// �̹��� ���� ���ϱ�
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


		//HSV�� ��ȯ
		cvtColor(img_frame, img_hsv, COLOR_BGR2HSV);
		cvtColor(img_frame2, img_hsv2, COLOR_BGR2HSV);

		//������ HSV ������ �̿��Ͽ� ������ ����ȭ
		Mat img_mask1, img_mask2, img_mask3;

		inRange(img_hsv, Scalar(160, 100, 129), Scalar(255, 255, 255), img_mask1);
		inRange(img_hsv2, Scalar(22, 62, 125), Scalar(161, 255, 255), img_mask2);



		// �������� ���� ħ�� / ��â

		//morphological opening ���� ������ ���� 
		erode(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));


		//morphological closing ������ ���� �޿�� 
		dilate(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		vector<vector<Point> > contours;
		// ������ ã��
		findContours(img_mask1, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		vector<Point2f> approx;
		Mat img_result = img_frame.clone();
		determiner_holder = count_determiner1;

		// ã�� ������ ������ŭ �̹����� �׸���

		for (size_t i = 0; i < contours.size(); i++)
		{
			approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

			//printf("%f \n", fabs(contourArea(Mat(approx))));
			if (fabs(contourArea(Mat(approx))) > 1000)  /// ������ ����ũ�� �̻��̾�� �Ѵ�. 
			{
				if (fabs(contourArea(Mat(approx))) < 30000)   /// ������ ����ũ�� �̸��̾�� �Ѵ�.
				{
					rectangle(img_result, boundingRect(contours[i]), Scalar(0, 0, 255), 2, 8, 0);
					Rect Result = boundingRect(contours[i]);


					//printf("pink: %d,%d\n", Result.x, Result.y);
					line(img_result, Point(0, 30), Point(img_result.rows, 30), Scalar(0, 0, 255), 3);
					line(img_result, Point(0, 350), Point(img_result.rows, 350), Scalar(0, 0, 255), 3);
					circle(img_result, Point(Result.x, Result.y), 1, Scalar(255, 255, 255), 2);
					imshow("pink�ν�", img_result);
					moveWindow("pink�ν�", 900, 500);
					if (count_determiner1)
					{
						/// ������ �Ʒ����� �����ϴ� �ٿ����.

						// �ٿ���� �Ѿ�� ã�� ������������ ��ٸ��ϴ�
						if (Result.y > 350)
						{
							printf("UP!1");
							count_determiner1 = false;
						}
					}
					else
					{
						// �ݴ�.
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


		//morphological closing ������ ���� �޿�� 
		dilate(img_mask2, img_mask2, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(img_mask2, img_mask2, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		vector<vector<Point> > contours2;
		// ������ ã��
		findContours(img_mask2, contours2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		vector<Point2f> approx2;
		Mat img_result2 = img_frame2.clone();
		determiner_holder = count_determiner2;
		// ã�� ������ ������ŭ �̹����� �׸���

		for (size_t i = 0; i < contours2.size(); i++)
		{
			approxPolyDP(Mat(contours2[i]), approx2, arcLength(Mat(contours2[i]), true)*0.02, true);

			//printf("%f \n", fabs(contourArea(Mat(approx))));
			if (fabs(contourArea(Mat(approx2))) > 1000)  /// ������ ����ũ�� �̻��̾�� �Ѵ�. 
			{
				if (fabs(contourArea(Mat(approx2))) < 30000)   /// ������ ����ũ�� �̸��̾�� �Ѵ�.
				{
					rectangle(img_result2, boundingRect(contours2[i]), Scalar(0, 0, 255), 2, 8, 0);
					Rect Result = boundingRect(contours2[i]);


					// printf("green: %d, %d\n", Result.x, Result.y);

					//rectangle(img_result, Point(295, 80), Point(15, 146), Scalar(255, 200, 200), 3);
					circle(img_result2, Point(Result.x, Result.y), 1, Scalar(255, 255, 255), 2);
					line(img_result2, Point(0, 355), Point(img_result.rows, 355), Scalar(0, 0, 255), 3);
					line(img_result2, Point(0, 100), Point(img_result.rows, 100), Scalar(0, 0, 255), 3);
					imshow("green�ν�", img_result2);
					moveWindow("green�ν�", 300, 500);
					if (count_determiner2)
					{
						/// ������ �Ʒ����� �����ϴ� �ٿ����.

						// �ٿ���� �Ѿ�� ã�� ������������ ��ٸ��ϴ�
						if (Result.y > 355)
						{
							printf("UP!2");
							count_determiner2 = false;
						}
					}
					else
					{
						// �ݴ�.
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
		imshow("����", img_frame);
		moveWindow("��Ȱ�", 500, 0);
		// ���̰� �Ʒ����� ���� �ٲ�� ī��Ʈ, ������ �Ʒ��ٲ�� ī��Ʈ
		//printf("%d \n", changed);

		if (determiner_holder != count_determiner1 && determiner_holder != count_determiner2)
		{
			changed += 1;
			// �Ʒ����� ��, ������ �Ʒ��� ������ ���ؼ� ��.
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