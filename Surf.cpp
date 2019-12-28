#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "iostream"
#include "fstream" 
#include "time.h"

using namespace cv;
using namespace std;
using namespace xfeatures2d;

vector<DMatch> GetGoodMatches(Mat a, Mat b);

int main()
{
	//计时
	long time = clock();
	//使用循环打开库中图像
	string pattern = "C:/Users/ysyan/Desktop/标识牌识别/ConsoleApplication1 - 副本/ConsoleApplication1/pic/*.png";
	Mat image = imread("b.jpg");
	vector<cv::String> fn;
	glob(pattern, fn, false);
	size_t count = fn.size();//库中图像的数量
	//建立集合，计算并储存匹配点数
	int arry[200];
	for (size_t i = 0; i < count; i++)
	{
		Mat image1 = imread(fn[i]);
		Mat image2 = image;
		vector<DMatch>GoodMatches = GetGoodMatches(image1, image2);
		arry[i] = GoodMatches.size();
	}
	//从集合中选出匹配对数最多的一个，再次计算
	int aim = 0;
	for (int i = 0; i < count; i++) { if (arry[aim] < arry[i]) aim = i; }
	Mat image1 = image;
	Mat image2 = imread(fn[aim]);

	vector<KeyPoint> keypoints1, keypoints2;
	Ptr<SURF> detector = SURF::create(700);
	detector->detect(image1, keypoints1);
	detector->detect(image2, keypoints2);
	vector<DMatch>goodmatches = GetGoodMatches(image1, image2);
	Mat imageMatches;
	drawMatches(image1, keypoints1, image2, keypoints2, goodmatches, imageMatches, Scalar(255, 0, 0));//进行绘制
	cout << "goodMatch个数：" << goodmatches.size() << endl;
	printf("匹配花费时间%dms", clock() - time);
	imshow("image", imageMatches);
	waitKey();
	return 0;
}

vector<DMatch> GetGoodMatches(Mat a, Mat b)
{
	Mat image1 = a;
	Mat image2 = b;
	//检测surf特征点
	vector<KeyPoint> keypoints1, keypoints2;
	Ptr<SURF> detector = SURF::create(700);
	detector->detect(image1, keypoints1);
	detector->detect(image2, keypoints2);
	//计算特征向量矩阵
	Mat descriptors1, descriptors2;
	Ptr<SURF> surfDesc = SURF::create();
	surfDesc->compute(image1, keypoints1, descriptors1);
	surfDesc->compute(image2, keypoints2, descriptors2);
	//进行特征匹配
	vector<DMatch> matches;
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
	matcher->match(descriptors1, descriptors2, matches);
	//计算匹配结果中欧氏距离的最大和最小值
	double max_dist = 0;
	double min_dist = 100;
	for (int m = 0; m<matches.size(); m++)
	{
		double dist = matches[m].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	//筛选出较好的匹配点
	vector<DMatch> goodmatches;
	for (int n = 0; n<matches.size(); n++)
	{
		if (matches[n].distance < 0.3 * max_dist)
		{
			goodmatches.push_back(matches[n]);
		}
	}
	return goodmatches;
}