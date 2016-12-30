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
	FloorMulGauss(vector<vector<MeanCovWeight>> rawData);//ԭʼ����
	void Init(vector<vector<MeanCovWeight>> rawData);
	void EStep();
	void MStep();
	void train();
private:
	Mat allMat;//���е�ѵ����
	unsigned int numOfTrain;//D ͼ�������
	unsigned int numOfGauss;//K ��˹ģ�͵�����
	unsigned int numOfFloorModel;//M ������˹ģ������
	unsigned int DIM;//ά��


	//���Եײ�ͼ���8����˹ģ��
	//D��ͼ��
	//ͼ����Ϣ
	vector<vector<MeanCovWeight>> rawData; //D*K ��
	vector<vector<Mat> > KDGuassMeans;//D*K �� D*1ά
	vector<vector<Mat> > KDGuassCovs;//D*K �� D*Dά
	vector<vector<double>> KDWeight;//D*K ��

	//������Ϣ
	//��64����˹ģ�����
	vector<Mat> means;//��ֵ���� M�� D*1ά
	vector<Mat> covs;//Э������� �ԽǾ��� M�� D*Dά
	vector<double> weight;//Ȩֵ M��

	//�м��������
	vector<vector<vector<double>>> Hmjk;//����Ϊ M*D*K��
	//
	vector<vector<vector<double>>> Wmjk;//����Ϊ M*D*K��
	//Ϊ����h׼��
	vector<vector<vector<double>>> Lmij;//����Ϊ M*D*K��

	//��Ȼ����
	double L;

	//Ȩֵ
	vector<MeanCovWeight> FloorMCW;
public:
	string fileName;

};
#endif