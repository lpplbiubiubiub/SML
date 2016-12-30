#include <opencv2\opencv.hpp>
#include <vector>
#include <ctime>
#include "MatConvert.h"
#include "Normlize.h"
#include "WriteFeatureData.h"
#include "EM.h"

#ifndef MULGAUSS_H
#define MULGAUSS_H
using namespace cv;
using namespace std;

int DataWrite(const char *fileName, vector<MeanCovWeight> data);
int DataRead(const char *fileName, vector<MeanCovWeight>& data);

//处理单张图片  分割为8*8图像块
void ImageSplit(Mat srcImage, vector<Mat>& resMat, int kernelSize = 8, int stride = 2);
//彩色图像dct变换
void ImageDctConv(Mat& srcImage);
//mean为列向量
double inline  CalcGaussPoss(Mat mean, Mat conv, Mat test)
{

	Mat temp;
	temp.create(1, 1, CV_64FC1);

	temp = (mean - test).t()*conv.inv()*(mean - test);

	double tempExp = -0.5*temp.at<double>(0, 0);

	double covDet = determinant(conv);
	
	return 1 / ( (pow(2 * CV_PI, mean.rows / 2.))*sqrt(covDet) )*exp(tempExp);

}

double inline  CalcGaussPossDiagonal(Mat mean, Mat conv, Mat test)
{
	Mat diagonal = Mat::eye(conv.rows,conv.cols,CV_64FC1);
	Mat temp;
	double sumTemp=0.;
	Mat diagonalTemp = mean.clone();
	
	diagonalTemp = (mean - test).mul(mean - test);
	for (int i = 0; i < conv.rows;i++)
	{
		sumTemp+=diagonalTemp.at<double>(i, 0) / conv.at<double>(i, i);
	}
	
	
	double tempExp = -0.5*sumTemp;

	double covDet = determinant(conv);

	return 1 / ((pow(2 * CV_PI, mean.rows / 2.))*sqrt(covDet))*exp(tempExp);

}




class MulGuass
{
public:
	MulGuass(string imgName);//构造函数
	MulGuass(vector<Mat> trainMat);//构造函数
	void ExpectStep();//E步
	void MaxStep();//M步
	void Train();//训练
	Mat allMat;//储存所有数据 单条数据以列的形式储存
	vector<Mat> mean;//均值 长度为K
	
private:
	void init(vector<Mat> trainMat);
private:

	int matRows;
	int matCols;
	int matType;
	int K;//高斯分量的个数
	int I;//训练数据长度
	vector<Mat> X;//训练数组 长度为I
	vector<double> lamda;//响应值 长度为K
	
	vector<Mat> conv;//协方差 长度为K
	Mat lik; //长度为i*k
	Mat rik;//长度为i*k
	Mat maxVec;//最大值矩阵 为归一化做准备
	Mat minVec;//最大值矩阵 为归一化做准备

	double trainLoss;//衡量em算法的标志

	//读写文件相关
	string imgName;
	string dataName;
	vector<MeanCovWeight> dat;

private:
	void ConvertData();
	void EMConvertData();
};

#endif