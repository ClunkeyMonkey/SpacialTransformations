#include <opencv2\opencv.hpp>
#include <cmath>
#include <Windows.h>

using namespace std;
using namespace cv;

int main() {
	Mat image1, image2, image3, image4, image5;
	Mat imageObj, imageTar;
	Mat image1o, image2o, image3o, image4o, image5o;
	Mat image1s, image2s, image3s, image4s, image5s;
	Mat TarImg, ObjImg, ObjImgCheck, TarImgCheck;

	vector<vector<Point>> conObj, conTar;
	vector<Vec4i> hieObj, hieTar;
	Point2f cenObj;

	//int sizeScale = 5;
	double white = 0;
	int angle = 0;
	float deltaY = 0;
	float deltaX = 0;
	int maxRObj = 0;
	int closestRTar = 0;

	/*int lowH = 0;
	int highH = 179;
	int lowS = 0;
	int highS = 255;
	int lowV = 0;
	int highV = 255;

	namedWindow("Control", WINDOW_AUTOSIZE);
	createTrackbar("lowH", "Control", &lowH, 179);
	createTrackbar("highH", "Control", &highH, 179);
	createTrackbar("lowS", "Control", &lowS, 255);
	createTrackbar("highS", "Control", &highS, 255);
	createTrackbar("lowV", "Control", &lowV, 255);
	createTrackbar("highV", "Control", &highV, 255);*/

	image1 = imread("Image 1.jpg", CV_LOAD_IMAGE_COLOR);
	//resize(image1, image1, Size(image1.cols / sizeScale, image1.rows / sizeScale));
	cvtColor(image1, image1o, COLOR_BGR2HSV);

	image2 = imread("Image 2.jpg", CV_LOAD_IMAGE_COLOR);
	//resize(image2, image2, Size(image2.cols / sizeScale, image2.rows / sizeScale));
	cvtColor(image2, image2o, COLOR_BGR2HSV);

	image3 = imread("Image 3.jpg", CV_LOAD_IMAGE_COLOR);
	//resize(image3, image3, Size(image3.cols / sizeScale, image3.rows / sizeScale));
	cvtColor(image3, image3o, COLOR_BGR2HSV);

	image4 = imread("Image 4.jpg", CV_LOAD_IMAGE_COLOR);
	//resize(image4, image4, Size(image4.cols / sizeScale, image4.rows / sizeScale));
	cvtColor(image4, image4o, COLOR_BGR2HSV);

	image5 = imread("Image 5.jpg", CV_LOAD_IMAGE_COLOR);
	//resize(image5, image5, Size(image5.cols / sizeScale, image5.rows / sizeScale));
	cvtColor(image5, image5o, COLOR_BGR2HSV);

	/*
	Target:
		S: 0,6

	Object:
		H: 100,126
		S: 35,255
		V: 95,255
	*/

	inRange(image5o, Scalar(0, 0, 0), Scalar(179, 6, 255), imageTar);
	inRange(image5o, Scalar(100, 35, 95), Scalar(126, 255, 255), imageObj);
	//imshow("Obj", imageObj);
	//imshow("Tar", imageTar);

	findContours(imageObj, conObj, hieObj, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	findContours(imageTar, conTar, hieTar, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point> > conPolyObj(conObj.size());
	vector<Point2f>cObj(conObj.size());
	vector<float>rObj(conObj.size());
	vector<vector<Point> > conPolyTar(conTar.size());
	vector<Point2f>cTar(conTar.size());
	vector<float>rTar(conTar.size());

	for (int i = 0; i < conObj.size(); i++) {
		approxPolyDP(Mat(conObj[i]), conPolyObj[i], 3, true);
		minEnclosingCircle((Mat)conPolyObj[i], cObj[i], rObj[i]);
	}

	for (int i = 0; i < conObj.size(); i++) {
		if (rObj[i] > rObj[maxRObj]) {
			maxRObj = i;
		}
	}

	for (int i = 0; i < conTar.size(); i++) {
		approxPolyDP(Mat(conTar[i]), conPolyTar[i], 3, true);
		minEnclosingCircle((Mat)conPolyTar[i], cTar[i], rTar[i]);
	}

	for (int i = 0; i< conTar.size(); i++) {
		if (abs(rObj[maxRObj] - rTar[closestRTar]) > abs(rObj[maxRObj] - rTar[i])) {
			closestRTar = i;
		}
	}

	/*drawContours(imageObj, conPolyObj, maxRObj, color, 1, 8, vector<Vec4i>(), 0, Point());
	circle(imageObj, cObj[maxRObj], (int)rObj[maxRObj], color, 1, 8, 0);

	drawContours(imageTar, conPolyTar, closestRTar, color, 1, 8, vector<Vec4i>(), 0, Point());
	circle(imageTar, cTar[closestRTar], (int)rTar[closestRTar], color, 1, 8, 0);*/

	deltaY = cTar[closestRTar].y - cObj[maxRObj].y;
	deltaX = cTar[closestRTar].x - cObj[maxRObj].x;

	cout << deltaX << ", " << deltaY << endl;

	ObjImg = imageObj(Range((cObj[maxRObj].y - rObj[maxRObj]), (cObj[maxRObj].y + rObj[maxRObj])), Range((cObj[maxRObj].x - rObj[maxRObj]), (cObj[maxRObj].x + rObj[maxRObj])));
	TarImg = imageTar(Range((cTar[closestRTar].y - rTar[closestRTar]), (cTar[closestRTar].y + rTar[closestRTar])), Range((cTar[closestRTar].x - rTar[closestRTar]), (cTar[closestRTar].x + rTar[closestRTar])));
	
	cenObj.x = ObjImg.cols / 2;
	cenObj.y = ObjImg.rows / 2;

	white = ObjImg.cols * ObjImg.rows;

	for (double ang = 0; ang < 360; ang++) {
		Mat rot = getRotationMatrix2D(cenObj, ang, 1);
		double whiteCount = 0;
		
		warpAffine(ObjImg, ObjImgCheck, rot, Size(TarImg.cols, TarImg.rows));
		TarImgCheck = TarImg - ObjImgCheck;

		for (int x = 0; x < TarImgCheck.cols; x++) {
			for (int y = 0; y < TarImgCheck.rows; y++) {
				if (TarImgCheck.at<uchar>(y, x) > 0) {
					whiteCount++;
				}
			}
		}
		//cout << ang << '\t' << whiteCount << endl;
		if (whiteCount < white) {
			white = whiteCount;
			angle = ang;
		}
	}
	cout << angle << endl;

	//imshow("Obj1", imageObj);
	//imshow("Tar1", imageTar);

	//imshow("ObjImg", ObjImg);
	//imshow("TarImg", TarImg);

	Sleep(3000);
	//waitKey(0);

	/*while (true) {
		
		inRange(image1o, Scalar(lowH, lowS, lowV), Scalar(highH, highS, highV), image1s);
		inRange(image2o, Scalar(lowH, lowS, lowV), Scalar(highH, highS, highV), image2s);
		inRange(image3o, Scalar(lowH, lowS, lowV), Scalar(highH, highS, highV), image3s);
		inRange(image4o, Scalar(lowH, lowS, lowV), Scalar(highH, highS, highV), image4s);
		inRange(image5o, Scalar(lowH, lowS, lowV), Scalar(highH, highS, highV), image5s);

		imshow("Image1s", image1s);
		imshow("Image1", image1);

		imshow("Image2s", image2s);
		imshow("Image2", image2);

		imshow("Image3s", image3s);
		imshow("Image3", image3);

		imshow("Image4s", image4s);
		imshow("Image4", image4);

		imshow("Image5s", image5s);
		imshow("Image5", image5);

		if (waitKey(1) == 27) {
			break;
		}
	}*/



	return 0;
}