#include "MatConvert.h"
//���ӦΪ������
void ConvertMatToVec(Mat& srcMat,Mat&desMat, unsigned int rows)
{
	int matRows = srcMat.rows;
	int matCols = srcMat.cols;
	int channels = srcMat.channels();
	srcMat.convertTo(srcMat, CV_64FC3);
	//�ȱ�Ϊ������
	Mat dst = Mat::zeros(1, matRows*matCols*channels, CV_64FC1);
	double* tempPic;
	double* dstPic = dst.ptr<double>(0);
	for (int i = 0; i < matRows; i++)//�������ص�
	{
		tempPic = srcMat.ptr<double>(i);
		for (int ii = 0; ii < matCols; ii++)
		{

			for (int iii = 0; iii < channels; iii++)
			{

				*dstPic = *tempPic;
				tempPic++;
				dstPic++;
			}

		}
	}
	resize(srcMat, srcMat, Size(1, matRows*matCols*channels));
	Mat tempMat= dst.colRange(0, rows).t();
	//��ת��Ϊ������
	desMat = tempMat.clone();
}