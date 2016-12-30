#include "MulGauss.h"
#include "MatConvert.h"
#include "Normlize.h"
#include <random>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <thread>

#include "FileRead.h"
#include "WriteFeatureData.h"
#include "FloorMulGauss.h"
map<int, double > mapIntDouble;
typedef pair<int, double> PAIR;
bool cmp_by_value(const PAIR& lhs, const PAIR& rhs) {
	return lhs.second > rhs.second;
}

struct CmpByValue {
	bool operator()(const PAIR& lhs, const PAIR& rhs) {
		return lhs.second > rhs.second;
	}
};


//使用多线程函数 进行加速
void threadProcess(string path, vector<vector<Mat>> meanVec, vector<vector<Mat>> covsMat, vector<vector<double>> weigVec, vector<double> freqVec, int index,int stride=26)
{
	float tempPoss;
	for (int i = 0; i < stride;i++)
	{
		tempPoss = log(CalcTestPoss(path, meanVec[index + i], covsMat[index + i], weigVec[index + i])) + log(freqVec[index + i]);
		mapIntDouble[index + i] = tempPoss;
	}
	
}

void process(string path, vector<vector<Mat>> meanVec, vector<vector<Mat>> covsMat, vector<vector<double>> weigVec, vector<double> freqVec)
{
	vector<int> indexTemp(10);
	for (int i = 0; i < 10;i++)
	{
		indexTemp[i] = 26 * i;
	}
	std::thread t0(threadProcess, path, meanVec, covsMat, weigVec, freqVec, indexTemp[0],26);
	std::thread t1(threadProcess, path, meanVec, covsMat, weigVec, freqVec, indexTemp[1], 26);
	std::thread t2(threadProcess, path, meanVec, covsMat, weigVec, freqVec, indexTemp[2], 26);
	std::thread t3(threadProcess, path, meanVec, covsMat, weigVec, freqVec, indexTemp[3], 26);
	std::thread t4(threadProcess, path, meanVec, covsMat, weigVec, freqVec, indexTemp[4], 26);
	std::thread t5(threadProcess, path, meanVec, covsMat, weigVec, freqVec, indexTemp[5], 26);
	std::thread t6(threadProcess, path, meanVec, covsMat, weigVec, freqVec, indexTemp[6], 26);
	std::thread t7(threadProcess, path, meanVec, covsMat, weigVec, freqVec, indexTemp[7], 26);
	std::thread t8(threadProcess, path, meanVec, covsMat, weigVec, freqVec, indexTemp[8], 26);
	std::thread t9(threadProcess, path, meanVec, covsMat, weigVec, freqVec, indexTemp[9], 26);

	t0.join();
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();
	t8.join();
	t9.join();
}
int main(int argc, char* argv[])
{
	const char* nums= argv[1];
	
	unsigned int testImgIndex = atoi(nums);
	//统计词频
	//读取mat数据 选定第一列
	Mat lableMat,testLableMat;
	readTxt("mat.txt", lableMat);
	
	//读取测试数据标注数据
	readTxt("testMatClone.txt", testLableMat);
	
	//获取文件列表
	vector<string> path;
	readImgPath("testFile.txt", path);

	//获取标注列表
	vector<string> fileNameVec;
	readFileName("fileName.txt", fileNameVec);
	
	//统计词频
	vector<double>vecOfWordFre(lableMat.cols);
	
	Mat sumOfWord = Mat::zeros(1,lableMat.cols,CV_32SC1);
	for (int i = 0; i < lableMat.rows;i++)
	{
		sumOfWord += lableMat.row(i);
	}
	int sum = 0;
	for (int i = 0; i < lableMat.cols; i++)
	{

		sum += sumOfWord.at<int>(0,i);
	}
	for (int i = 0; i < lableMat.cols; i++)
	{

		vecOfWordFre[i] = sumOfWord.at<int>(0, i)/(sum+0.);
		
	}


	vector<vector<MeanCovWeight>> floorGuass;
	for (int i = 0; i < lableMat.cols; i++)
	{
		//语义层初始化
		char str[10];
		int a = i;
		sprintf(str, "%d", a);

		//临时文件名
		string temp(str);
		temp = "FloorGaussDat//" + temp + ".data";

		vector<MeanCovWeight> tempMCW;
		DataRead(temp.c_str(), tempMCW);
		floorGuass.push_back(tempMCW);

	}

	//保存概率值的数组
	vector<double> possVec;
	//保存索引值的素组
	vector<size_t> indexVec;

	//所有变量
	vector<vector<Mat>> meanAllVec;
	vector<vector<Mat>> covsAllVec;
	vector<vector<double>> weigAllVec;

	for (int i = 0; i < lableMat.cols; i++)
	{ 
		vector<Mat> meanVec;
		vector<Mat> covVec;
		vector<double> weigVec;
		convertDataTOMat(floorGuass[i], meanVec, covVec, weigVec);
		meanAllVec.push_back(meanVec);
		covsAllVec.push_back(covVec);
		weigAllVec.push_back(weigVec);
	}
	float tempPoss = 0.;

	//测试图像
	
	cout << path[testImgIndex] << endl;
	process(path[testImgIndex], meanAllVec, covsAllVec, weigAllVec, vecOfWordFre);
	Mat testImg = imread(path[testImgIndex]);
	imshow("测试图像", testImg);
	vector<PAIR> name_score_vec(mapIntDouble.begin(), mapIntDouble.end());
	sort(name_score_vec.begin(), name_score_vec.end(), CmpByValue());
	
	cout << "测试文件的标注为 ";
	for (int i = 0; i < lableMat.cols; i++)
	{
		if (testLableMat.at<int>(testImgIndex, i))
		{
			cout << fileNameVec[i] << " ";
		}
	}
	cout << endl;
	cout << "程序给出的标注为 ";
	for (int i = 0; i <=5; ++i) {
		cout << fileNameVec[name_score_vec[i].first]<<" ";
	}

	waitKey(0);
	return 0;
	
}