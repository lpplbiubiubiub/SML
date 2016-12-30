#include <opencv2\opencv.hpp>
#include <vector>
#include "MulGauss.h"
#ifndef FLOORMULGAUSS_H
#define FLOORMULGAUSS_H
using namespace std;
using namespace cv;
double inline CalcTrace(Mat srcMat)
{
	double traceSum=0.;
	for (int i = 0; i < srcMat.rows;i++)
	{
		traceSum += srcMat.at<double>(i,i);
	}
	return traceSum;
}
void convertDataTOMat(vector<MeanCovWeight> guassPara, vector<Mat>& meanMat, vector<Mat>& covsMat, vector<double>& weigVec);
double  inline  CalcFloorPoss(vector<Mat> meanVec, vector<Mat> covsMat, vector<double> weigVec, Mat test);
double CalcTestPoss(string fileName, vector<Mat> meanVec, vector<Mat> covsMat, vector<double> weigVec);

class FloorMulGauss
{
public:
	FloorMulGauss(vector<vector<MeanCovWeight>> rawData);//原始数据
	void Init(vector<vector<MeanCovWeight>> rawData);
	void EStep();
	void MStep();
	void train();
private:
	Mat allMat;//所有的训练集
	unsigned int numOfTrain;//D 图像的数量
	unsigned int numOfGauss;//K 高斯模型的数量
	unsigned int numOfFloorModel;//M 语义层高斯模型数量
	unsigned int DIM;//维度


	//来自底层图像的8个高斯模型
	//D个图像
	//图像信息
	vector<vector<MeanCovWeight>> rawData; //D*K 个
	vector<vector<Mat> > KDGuassMeans;//D*K 个 D*1维
	vector<vector<Mat> > KDGuassCovs;//D*K 个 D*D维
	vector<vector<double>> KDWeight;//D*K 个

	//语义信息
	//由64个高斯模型组成
	vector<Mat> means;//均值矩阵 M个 D*1维
	vector<Mat> covs;//协方差矩阵 对角矩阵 M个 D*D维
	vector<double> weight;//权值 M个

	//中间变量计算
	vector<vector<vector<double>>> Hmjk;//个数为 M*D*K个
	//
	vector<vector<vector<double>>> Wmjk;//个数为 M*D*K个
	//为计算h准备
	vector<vector<vector<double>>> Lmij;//个数为 M*D*K个

	//似然函数
	double L;

	//权值
	vector<MeanCovWeight> FloorMCW;
public:
	string fileName;

};
#endif