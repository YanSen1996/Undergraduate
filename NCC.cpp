#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "math.h"
#include "iostream"
#include "fstream" 
#include "time.h"

using namespace cv;
using namespace std;

struct R { int x; int y; double NCC; };
R get_NCC(Mat a, Mat b);
struct RR { int x; int y; double NCC; int s; };
double get_mean(Mat a);
double get_variance(Mat a, double mean);

int main()
{
	//计时
	long time = clock();
	//使用循环打开库中图像并记录库中素材数量
	string pattern = "C:/Users/ysyan/Desktop/标识牌识别/NCC_Surf/ConsoleApplication1/Pictures/*.bmp";
	Mat image = imread("b.jpg");
	vector<cv::String> fn;
	glob(pattern, fn, false);
	size_t count = fn.size();
	//建立集合，计算并储存匹配点数
	RR arry[100] = { 0,0,0,0 };
	for (size_t i = 0; i < count; i++)
	{
		Mat a = image;
		Mat b = imread(fn[i]);
		cvtColor(a, a, CV_RGB2GRAY);
		cvtColor(b, b, CV_RGB2GRAY);
		R ncc = get_NCC(a, b);
		arry[i].x = ncc.x;
		arry[i].y = ncc.y;
		arry[i].NCC = ncc.NCC;
		arry[i].s = i;
	}
	//从集合中选出NCC的值最接近1的一个
	RR aim{ 0,0,0,0 };
	for (int i = 0; i < count; i++)
	{
		if (aim.NCC < arry[i].NCC)
			aim = arry[i];
	}
	//输出匹配结果
	Mat dst = imread(fn[aim.s]);
	if (aim.NCC < 0.8)
	{
		printf("NCC的值为%f\n", aim);
		printf("未能成功匹配");
	}
	else
	{
		imshow("识别图", dst);
		printf("NCC的值为%f;\nx=%d，y=%d\n;", aim.NCC, aim.x, aim.y);
	}
	printf("匹配花费时间%dms", clock() - time);
	waitKey();
}

R get_NCC(Mat a, Mat b)
{
	int width = a.cols;
	int height = a.rows;
	int N = b.rows;
	//图像匹配的起始行和终止行  
	int line_start = 0;
	int line_end = height - N;
	//图像匹配的起始列和终止列  
	int row_start = 0;
	R window{ 0,0,0 };
	int row_end = width - N;
	double ncc = 0;
	if (width < N || height < N)
	{
		printf("对不起！图片过小，无法识别。");
		system("pause");
		waitKey();
	}
	else
	{
		double mean_b = get_mean(b);
		double variance_b = get_variance(b, mean_b);
		for (int x = row_start; x < row_end; x++)
		{
			for (int y = line_start; y < line_end; y++)
			{
				Rect size(x, y, N, N);
				Mat r = a(size);
				double mean_r = get_mean(r);
				double variance_r = get_variance(r, mean_r);
				double diff = 0;
				for (int i = 0; i < N; i++)
				{
					for (int j = 0; j < N; j++)
					{
						double diff_r = r.at<uchar>(i, j) - mean_r;
						double diff_b = b.at<uchar>(i, j) - mean_b;
						double variance = variance_r * variance_b;
						diff = diff + (diff_r * diff_b);
						//printf("diff的值为%f\n", diff);
						ncc = diff / variance;
						if (ncc > window.NCC)
						{
							window.NCC = ncc;
							window.x = x;
							window.y = y;
						}
					}
				}
			}
			//printf("%f",max);
		}
	}
	return window;
}

double get_mean(Mat a)
{
	int width = a.rows;
	int height = a.cols;
	int m = width * height;
	double n = 0;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			n = n + a.at<uchar>(i, j);
		}
	}
	double mean = n / m;
	return mean;
}

double get_variance(Mat a, double mean)
{
	int width = a.rows;
	int height = a.cols;
	int m = width * height;
	double x = 0, y = 0;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			x = a.at<uchar>(i, j) - mean;
			y = y + x * x;
		}
	}
	double variance = sqrt(y);
	return variance;
}