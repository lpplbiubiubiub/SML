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

//������ͼƬ  �ָ�Ϊ8*8ͼ���
void ImageSplit(Mat srcImage, vector<Mat>& resMat, int kernelSize = 8, int stride = 2);
//��ɫͼ��dct�任
void ImageDctConv(Mat& srcImage);
//meanΪ������
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
	MulGuass(string imgName);//���캯��
	MulGuass(vector<Mat> trainMat);//���캯��
	void ExpectStep();//E��
	void MaxStep();//M��
	void Train();//ѵ��
	Mat allMat;//������������ �����������е���ʽ����
	vector<Mat> mean;//��ֵ ����ΪK
	
private:
	void init(vector<Mat> trainMat);
private:

	int matRows;
	int matCols;
	int matType;
	int K;//��˹�����ĸ���
	int I;//ѵ�����ݳ���
	vector<Mat> X;//ѵ������ ����ΪI
	vector<double> lamda;//��Ӧֵ ����ΪK
	
	vector<Mat> conv;//Э���� ����ΪK
	Mat lik; //����Ϊi*k
	Mat rik;//����Ϊi*k
	Mat maxVec;//���ֵ���� Ϊ��һ����׼��
	Mat minVec;//���ֵ���� Ϊ��һ����׼��

	double trainLoss;//����em�㷨�ı�־

	//��д�ļ����
	string imgName;
	string dataName;
	vector<MeanCovWeight> dat;

private:
	void ConvertData();
	void EMConvertData();
};

#endif