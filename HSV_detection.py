# -*- coding: utf-8 -*-
import cv2
import numpy as np


# 콜백함수입니다.
# 필요하지 않기때문에 안쓰겠습니다 ( PASS)
def nothing(a):
    pass


def trackbar():
    window_name = "COLOR_DETECTION"

    # 트랙바 담을 윈도우 생성
    cv2.namedWindow(window_name, cv2.WINDOW_GUI_NORMAL)

    # 트랙바 생성
    cv2.createTrackbar("thresh_val", window_name, 0, 255, nothing)
    cv2.createTrackbar("lower_h", window_name, 100, 255, nothing)
    cv2.createTrackbar("lower_s", window_name, 100, 255, nothing)
    cv2.createTrackbar("lower_v", window_name, 100, 255, nothing)
    cv2.createTrackbar("upper_h", window_name, 255, 255, nothing)
    cv2.createTrackbar("upper_s", window_name, 255, 255, nothing)
    cv2.createTrackbar("upper_v", window_name, 255, 255, nothing)
    cnt = 0

    

     cap = cv2.VideoCapture(cv2.CAP_ANY)
    while True:
        # 캠이 작동하는지 확인합니다.
        if (not cap.isOpened()):
            print("Cannot capture video")
            break
        ret, frame = cap.read()
        frame = cv2.resize(frame, (700, 400))
        cnt += 1

        # threshold 값 트랙바에서 찾구요. 밑에도 마찬가지입니다.
        thresh_val = cv2.getTrackbarPos("thresh_val", window_name)
        lower_h = cv2.getTrackbarPos("lower_h", window_name)
        lower_s = cv2.getTrackbarPos("lower_s", window_name)
        lower_v = cv2.getTrackbarPos("lower_v", window_name)
        upper_h = cv2.getTrackbarPos("upper_h", window_name)
        upper_s = cv2.getTrackbarPos("upper_s", window_name)
        upper_v = cv2.getTrackbarPos("upper_v", window_name)

        # opencv는 컬러가 BGR로 들어갑니다. HSV 원하시니 변경해야겠죠. 보통 컬러디텍션할때 HSV 많이씁니다.
        # RGB보다 빛에 영향을 덜 받거든요
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

        # 트랙바 용입니다.
        H = cv2.inRange(hsv, (lower_h, lower_s, lower_v), (upper_h, upper_s, upper_v))
        # 커널 입혀야지 디텍션 좀 됩니다.

        cv2.imshow("원본", H)
        cv2.imshow("frame", frame)
        cv2.imshow(window_name, H)
        if cv2.waitKey(500) & 0xFF == ord('q'):
            break


def main():
    trackbar()


if __name__ == '__main__':
    main()

