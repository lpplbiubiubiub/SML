#include "Normlize.h"



void Normlize(vector<Mat>& srcMat, Mat& maxMat_, Mat& minMat_)
{
	Mat maxMat = srcMat[0].clone();
	Mat minMat = srcMat[0].clone();
	Mat absDiffMat = srcMat[0].clone();
	for (int i = 0; i < srcMat.size(); i++)
	{

		maxMat = cv::max(srcMat[i], maxMat);
		minMat = cv::min(srcMat[i],minMat);

	}
	maxMat_ = maxMat.clone();
	minMat_ = minMat.clone();
	absDiffMat = maxMat_ - minMat_;
	for (int i = 0; i < srcMat.size(); i++)
	{

		divide(srcMat[i] - minMat_, absDiffMat, srcMat[i]);
	}
	
}