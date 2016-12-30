#include "MulGauss.h"
/////////////////////////////////////////////////////////////////////////////////////////////
//函数名：DataWrite
//功能：写入二进制文件
//基本思想：二进制文件的写入
//执行条件：fileName文件路径有效
//输入参数：fileName(文件路径)，data（待写入的数组）
//输出参数：无
//返回值	：成功标志
//说明：无
//设计者	：lppl
//日期：2016-9-29
//////////////////////////////////////////////////////////////////////////////////////////////
int DataWrite(const char *fileName, vector<MeanCovWeight> data)
{
	FILE *fp = NULL;//获取句柄
	fopen_s(&fp, fileName, "wb");//b表示以二进制方式打开文件
	if (fp == NULL) //打开文件失败，返回错误信息
	{
		throw "打开文件失败";
	}
	MeanCovWeight temp;
	for (auto r : data)//遍历data数组
	{
		temp = r;
		fwrite(&temp, sizeof(struct MeanCovWeight), 1, fp); //向文件中写入数据
	}
	fclose(fp);//关闭文件
	return 0;

}
/////////////////////////////////////////////////////////////////////////////////////////////
//函数名：DataRead
//功能：读取二进制文件
//基本思想：二进制文件的读取
//执行条件：fileName文件路径有效
//输入参数：fileName(文件路径)，data（待写入的数组）
//输出参数：无
//返回值	：成功标志
//说明：无
//设计者	：lppl
//日期：2016-9-29
//////////////////////////////////////////////////////////////////////////////////////////////
int DataRead(const char *fileName, vector<MeanCovWeight>& data)
{
	FILE *fp = NULL;
	fopen_s(&fp, fileName, "rb");
	if (fp == NULL) //打开文件失败，返回错误信息
	{
		throw "打开文件失败";
	}

	MeanCovWeight temp;//单个结构体 中间变量

	while (fread(&temp, sizeof(struct MeanCovWeight), 1, fp))//逐个读取
	{
		data.push_back(temp);//压入数组
	}
	fclose(fp);//关闭文件
	return 0;
}


void ImageSplit(Mat srcImage, vector<Mat>& resMat, int kernelSize , int stride )
{
	//获取图片长宽
	int imgWidth = srcImage.cols;
	int imgHeight = srcImage.rows;
	Rect rect(Point(0, 0), Point(kernelSize, kernelSize));
	
	//获取图片列表
	for (int row = 0; row < (imgHeight - kernelSize+1) / stride; row ++)
	{
		for (int col = 0; col < (imgWidth - kernelSize + 1) / stride; col ++)
		{
			
			Rect rectTemp = rect + Point(col*stride, row*stride);
			
			Mat temMat = srcImage(rectTemp).clone();
			Mat tempDestMat;
			ImageDctConv(temMat);
			ConvertMatToVec(temMat, tempDestMat, NUMOFROWS);
			resMat.push_back(tempDestMat);
			
		}
	}
	
}

void ImageDctConv(Mat& srcImage)
{
	vector<Mat> YUV;
	
	split(srcImage, YUV);

	Mat float_R, float_G, float_B;
	vector<Mat> DFT_dst(3);
	YUV[0].convertTo(float_R, CV_64FC1);
	YUV[1].convertTo(float_G, CV_64FC1);
	YUV[2].convertTo(float_B, CV_64FC1);

	DFT_dst[0] = float_R.clone();
	DFT_dst[1] = float_G.clone();
	DFT_dst[2] = float_B.clone();

	
	dct(float_R, DFT_dst[0]);
	dct(float_G, DFT_dst[1]);
	dct(float_B, DFT_dst[2]);


	merge(DFT_dst, srcImage);
	
	
}

//输入为列向量组
MulGuass::MulGuass(vector<Mat> trainMat)
{
	init(trainMat);
	
}


MulGuass::MulGuass(string imgName_)
{
	imgName = imgName_;
	//生成data位置
	int posOfDot = imgName.find_last_of(".");
	dataName = imgName.substr(0, posOfDot);
	dataName += ".data";



	Mat testMat = imread(imgName);
	
	vector<Mat> testMatVec;
	ImageSplit(testMat, testMatVec);
	init(testMatVec);


	EM em = EM(NUMOFGAUSS, 1);
	em.train(allMat);
	Mat means = em.get<Mat>("means");
	Mat weight = em.get<Mat>("weights");
	const vector<Mat>& covs = em.get<vector<Mat> >("covs");
	
	for (int i = 0; i < NUMOFGAUSS; i++)
	{
		MeanCovWeight temp;
		dat.push_back(temp);
	}
	for (int i = 0; i < NUMOFGAUSS; i++)
	{

		for (int j = 0; j < means.cols; j++)
		{
			dat[i].Mean[j] = means.at<double>(i, j);
			
		}
		
	}
	
	for (int i = 0; i < NUMOFGAUSS; i++)
	{

		for (int j = 0; j < covs[i].rows; j++)
		{
			dat[i].Cov[j] = covs[i].at<double>(j, j);
			
		}
		
	}
	
	for (int i = 0; i < NUMOFGAUSS; i++)
	{
		dat[i].Weig = weight.at<double>(0,i);
		
	}
	

	DataWrite(dataName.c_str(), dat);
	/*
	init(testMatVec);
	Train();
	ConvertData();
	*/

	


}

void MulGuass::init(vector<Mat> trainMat)
{
	srand((unsigned)time(0));

	I = trainMat.size();//训练数据长度
	if (I == 0)
	{
		throw("数据长度为0");
	}
	matCols = trainMat[0].cols;
	matRows = trainMat[0].rows;
	matType = trainMat[0].type();

	K = NUMOFGAUSS;//高斯分量的个数
	X = trainMat;//引用复制
	//Normlize(X, maxVec,minVec);

	Mat tempConv = Mat::zeros(matRows, matRows, CV_64FC1);
	Mat tempSum = Mat::zeros(matRows, 1, CV_64FC1);

	for (int i = 0; i < I; i++)
	{
		Mat tempMat = trainMat[i].t();
		tempSum += trainMat[i].clone();
		allMat.push_back(tempMat.clone());

	}
	tempSum = tempSum / (I + 0.);
	for (int i = 0; i < I; i++)
	{

		tempConv += (trainMat[i] - tempSum)*(trainMat[i] - tempSum).t();
	}
	tempConv = tempConv / (I + 0.);

	for (int i = 0; i < K; i++)
	{

		Mat tempMean = trainMat[rand() % I];//随机

		lamda.push_back(1 / (K + 0.));
		mean.push_back(tempMean.clone());
		conv.push_back(tempConv.clone());

	}




	lik.create(I, K, CV_64FC1); //长度为i*k
	rik.create(I, K, CV_64FC1);//长度为i*k
	trainLoss = -999999.;



}














//e步
void MulGuass::ExpectStep()
{
	
	Mat resOfSum = Mat::zeros(1 , 1, CV_64FC1);
	for (int i = 0; i < I;i++)
	{

		for (int k = 0; k < K;k++)
		{
			lik.at<double>(i, k) = lamda[k] * CalcGaussPossDiagonal(mean[k], conv[k], X[i]);
			
		}
		resOfSum = lik.row(i)*(Mat::ones(K, 1, CV_64FC1));//求和
		for (int k = 0; k < K; k++)
		{
			
			rik.at<double>(i, k) = lik.at<double>(i, k) / (resOfSum.at<double>(0,0) );
			
		}
		
	}
	
	
	
	
}
//m步
void MulGuass::MaxStep()
{
	double temp;
	Mat resOfSumIk_Rik = Mat::zeros(1, 1, CV_64FC1);
	Mat resOfSumI_Rik = Mat::zeros(1, 1, CV_64FC1);
	Mat resOfSumI_RikX = Mat::zeros(matRows, matCols, CV_64FC1);
	Mat resOfSumI_RikXX = Mat::zeros(matRows, matRows, CV_64FC1);
	
	//计算rik所有元素
	for (int i = 0; i < I;i++)
	{
		resOfSumIk_Rik += rik.row(i)*(Mat::ones(K, 1, CV_64FC1));
		
	}
	
	//更新参数
	for (int k = 0; k < K; k++)
	{
		
		resOfSumI_RikX = Mat::zeros(matRows, matCols, CV_64FC1);
		resOfSumI_RikXX = Mat::zeros(matRows, matRows, CV_64FC1);
		
		for (int i = 0; i < I; i++)
		{
			resOfSumI_RikX += rik.at<double>(i, k)*X[i];
			resOfSumI_RikXX += rik.at<double>(i, k)*((X[i] - mean[k])*(X[i] - mean[k]).t());
		}

		

		resOfSumI_Rik = (Mat::ones(1, I, CV_64FC1))*rik.col(k);
		lamda[k] = (resOfSumI_Rik.at<double>(0, 0)) / (resOfSumIk_Rik.at<double>(0,0));
		
		
		mean[k] = resOfSumI_RikX / (resOfSumI_Rik.at<double>(0, 0));
		conv[k] = resOfSumI_RikXX / (resOfSumI_Rik.at<double>(0, 0));
		
		
		
	}
	
	//统计loss
	double loss = 0.;
	double tempLoss = 0.;
	for (int i = 0; i < I;i++)
	{
		
		tempLoss = 0.;
		for (int k = 0; k < K;k++)
		{
			tempLoss+=lamda[k] * CalcGaussPoss(mean[k], conv[k], X[i]);
		}
		loss += log(tempLoss);
	}
	trainLoss = loss;
	
}
void MulGuass::Train()
{
	double lossBef ;
	do
	{
		lossBef = trainLoss;
		ExpectStep();
		MaxStep();
		
	} 
	while (trainLoss>lossBef+0.1);
}
//训练完毕 转化数据为data文件
void MulGuass::ConvertData()
{
	//得到data文件的文件名
	
	for (int i = 0; i < NUMOFGAUSS;i++)
	{
		MeanCovWeight temp;
		dat.push_back(temp);
	}
	for (int i = 0; i < NUMOFGAUSS;i++)
	{
		
		for (int j = 0; j < mean[i].rows; j++)
		{
			dat[i].Mean[j] = mean[i].at<double>(j, 0);
		}
		
	}
	for (int i = 0; i < NUMOFGAUSS; i++)
	{

		for (int j = 0; j < conv[i].rows; j++)
		{
			dat[i].Cov[j] = conv[i].at<double>(j, j);
		}

	}

	for (int i = 0; i < NUMOFGAUSS; i++)
	{	
		dat[i].Weig = lamda[i];
	}


	DataWrite(dataName.c_str(), dat);


}