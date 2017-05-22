#include <opencv2\opencv.hpp>
#include <cmath>

using namespace std;
using namespace cv;

long double pi = 3.1415926535897932384626433832795;

void transform(Mat image, int num);

int main() {
	Mat image1, image2, image3, image4, image5;

	image1 = imread("Image 1.jpg", CV_LOAD_IMAGE_COLOR);
	cvtColor(image1, image1, COLOR_BGR2HSV);

	image2 = imread("Image 2.jpg", CV_LOAD_IMAGE_COLOR);
	cvtColor(image2, image2, COLOR_BGR2HSV);

	image3 = imread("Image 3.jpg", CV_LOAD_IMAGE_COLOR);
	cvtColor(image3, image3, COLOR_BGR2HSV);

	image4 = imread("Image 4.jpg", CV_LOAD_IMAGE_COLOR);
	cvtColor(image4, image4, COLOR_BGR2HSV);

	image5 = imread("Image 5.jpg", CV_LOAD_IMAGE_COLOR);
	cvtColor(image5, image5, COLOR_BGR2HSV);

	transform(image1, 1);
	transform(image2, 2);
	transform(image3, 3);
	transform(image4, 4);
	transform(image5, 5);
	
	system("pause");
	return 0;
}

void transform (Mat image, int num){
	Mat imageObj, imageTar;
	Mat TarImg, ObjImg, ObjImgCheck, TarImgCheck;

	vector<vector<Point>> conObj, conTar;
	vector<Vec4i> hieObj, hieTar;
	Point cenObj;
	size_t conObjSize;
	size_t conTarSize;

	double white = 0;
	double angle = 0;
	float deltaY = 0;
	float deltaX = 0;
	size_t maxRObj = 0;
	size_t closestRTar = 0;
	int adj = 13;

	/*
	Target:
		S: 0,6

	Object:
		H: 100,126
		S: 35,255
		V: 95,255
	*/

	inRange(image, Scalar(0, 0, 0), Scalar(179, 6, 255), imageTar);
	inRange(image, Scalar(100, 35, 95), Scalar(126, 255, 255), imageObj);

	findContours(imageObj, conObj, hieObj, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	findContours(imageTar, conTar, hieTar, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	conObjSize = conObj.size();
	conTarSize = conTar.size();

	vector<vector<Point>> conPolyObj(conObjSize);
	vector<Point2f> cObj(conObjSize);
	vector<float> rObj(conObjSize);
	vector<vector<Point>> conPolyTar(conTarSize);
	vector<Point2f> cTar(conTarSize);
	vector<float> rTar(conTarSize);

	for (size_t i = 0; i < conObjSize; i++) {
		approxPolyDP(Mat(conObj[i]), conPolyObj[i], 3, true);
		minEnclosingCircle((Mat)conPolyObj[i], cObj[i], rObj[i]);
	}

	for (size_t i = 0; i < conTarSize; i++) {
		approxPolyDP(Mat(conTar[i]), conPolyTar[i], 3, true);
		minEnclosingCircle((Mat)conPolyTar[i], cTar[i], rTar[i]);
	}

	for (size_t i = 0; i < conObjSize; i++) {
		maxRObj = ((rObj[i] > rObj[maxRObj]) ? i : maxRObj);
	}

	for (size_t i = 0; i< conTarSize; i++) {
		closestRTar = ((abs(rObj[maxRObj] - rTar[closestRTar]) > abs(rObj[maxRObj] - rTar[i])) ? i : closestRTar);
	}

	deltaX = cTar[closestRTar].x - cObj[maxRObj].x;
	deltaY = cTar[closestRTar].y - cObj[maxRObj].y;

	ObjImg = imageObj(Range((cObj[maxRObj].y - rObj[maxRObj]), (cObj[maxRObj].y + rObj[maxRObj])), Range((cObj[maxRObj].x - rObj[maxRObj]), (cObj[maxRObj].x + rObj[maxRObj])));
	TarImg = imageTar(Range((cTar[closestRTar].y - rTar[closestRTar]), (cTar[closestRTar].y + rTar[closestRTar])), Range((cTar[closestRTar].x - rTar[closestRTar]), (cTar[closestRTar].x + rTar[closestRTar])));
	
	cenObj.x = ObjImg.cols / 2;
	cenObj.y = ObjImg.rows / 2;

	white = ObjImg.cols * ObjImg.rows;

	for (double ang = 0; ang < 360; ang++) {
		double whiteCount = 0;
		
		warpAffine(ObjImg, ObjImgCheck, getRotationMatrix2D(cenObj, ang, 1), Size(TarImg.cols, TarImg.rows));
		TarImgCheck = TarImg - ObjImgCheck;

		for (int x = 0; x < TarImgCheck.cols; x++) {
			for (int y = 0; y < TarImgCheck.rows; y++) {
				whiteCount = ((TarImgCheck.at<uchar>(y, x) > 0) ? whiteCount + 1 : whiteCount);
			}
		}
		if (whiteCount < white) {
			white = whiteCount;
			angle = ang;
		}
	}

	cout << "Image " << num << endl;

	cout << '|' << setw(adj) << cos(angle * pi / 180);
	cout << setw(adj) << -sin(angle * pi / 180);
	cout << setw(adj) << deltaX << " |" << endl;

	cout << '|' << setw(adj) << sin(angle * pi / 180);
	cout << setw(adj) << cos(angle * pi / 180);
	cout << setw(adj) << deltaY << " |" << endl;

	cout << '|' << setw(adj) << 0;
	cout << setw(adj) << 0;
	cout << setw(adj) << 1;
	cout << " |" << endl;

	for (int i = 0; i < (3 * adj + 3); i++) cout << '_';
	cout << endl;
}