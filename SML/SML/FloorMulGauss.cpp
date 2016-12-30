#include "FloorMulGauss.h"


//����data��ȡ��������ת��Ϊmat����
void convertDataTOMat(vector<MeanCovWeight> guassPara,vector<Mat>& meanMat, vector<Mat>& covsMat,vector<double>& weigVec)
{
	Mat meanTemp = Mat::zeros(NUMOFROWS, 1, CV_64FC1);
	Mat varTemp = Mat::zeros(NUMOFROWS, NUMOFROWS, CV_64FC1);
	double tempWeig = 0.;
	for (int i = 0; i < guassPara.size(); i++)
	{

		for (int k = 0; k < NUMOFROWS; k++)
		{
			meanTemp.at<double>(k, 0) = guassPara[i].Mean[k];
			varTemp.at<double>(k, k) = guassPara[i].Cov[k];
			tempWeig = guassPara[i].Weig;
		}
		//��ֵ���
		meanMat.push_back(meanTemp.clone());
		covsMat.push_back(varTemp.clone());
		weigVec.push_back(tempWeig);
	}
}

//����һ��10ά���� ��Ӧ�ĸ���ֵ
double  inline  CalcFloorPoss(vector<Mat> meanVec,vector<Mat> covsMat,vector<double> weigVec, Mat test)
{
	double sumOfPoss = 0.;
	for (int i = 0; i < meanVec.size(); i++)
	{
		sumOfPoss += weigVec[i] * CalcGaussPoss(meanVec[i], covsMat[i], test);
	}
	
	return sumOfPoss;
}

//���㵥������ֲ���һ��ͼƬ����Ӧֵ
double CalcTestPoss(string fileName, vector<Mat> meanVec, vector<Mat> covsMat, vector<double> weigVec)
{
	double possSum = 0.;
	Mat testMat = imread(fileName);

	vector<Mat> testMatVec;
	ImageSplit(testMat, testMatVec);
	//��Ϊ������ ��
	for (int i = 0; i < testMatVec.size(); i++)
	{
		possSum += CalcFloorPoss(meanVec, covsMat, weigVec, testMatVec[i]);
	}

	return possSum;
}






















FloorMulGauss::FloorMulGauss(vector<vector<MeanCovWeight>> rawData_)//ԭʼ����
{
	Init(rawData_);

}

void FloorMulGauss::Init(vector<vector<MeanCovWeight>> rawData_)
{
	srand(unsigned(time(0)));
	//����ռ�
	numOfTrain = rawData_.size();//D ͼ�������
	if (numOfTrain==0)
	{
		throw("the number of example is zero");
	}
	numOfGauss = NUMOFGAUSS;//K ��˹ģ�͵�����
	numOfFloorModel = NUMOFFLOORGAUSS;//M ������˹ģ������
	DIM = NUMOFROWS;//ά��


	//���Եײ�ͼ���8����˹ģ��
	//D��ͼ��
	//ͼ����Ϣ
	Mat meanTemp = Mat::zeros(DIM, 1 , CV_64FC1);
	Mat covsTemp = Mat::zeros(DIM, DIM, CV_64FC1);
	
	rawData = rawData_; //D*K �� ���ø���
	
	for (int i = 0; i < numOfTrain;i++)
	{
		vector<Mat> meanVecTemp;
		vector<Mat> covsVecTemp;
		for (int j = 0; j < numOfGauss; j++)
		{
			meanVecTemp.push_back(meanTemp.clone());
			covsVecTemp.push_back(covsTemp.clone());
		}
		KDGuassMeans.push_back(meanVecTemp);//D*K �� D*1ά
		KDGuassCovs.push_back(covsVecTemp);
		meanVecTemp.clear();
		covsVecTemp.clear();
	}
	
	vector<vector <double> > ivec(numOfTrain, vector<double>(numOfGauss));//D*K ��
	KDWeight = ivec;


	//������Ϣ
	//��64����˹ģ�����

	//��ʼ�� 1/M
	vector<vector <double> > jkTemp(numOfTrain, vector<double>(numOfGauss, 1/(numOfFloorModel + 0.)));//D*K ��
	for (int i = 0; i < numOfFloorModel;i++)
	{
		means.push_back(meanTemp.clone());
		covs.push_back(covsTemp.clone());
		Hmjk.push_back(jkTemp);//����Ϊ M*D*K��
		Wmjk.push_back(jkTemp);
		Lmij.push_back(jkTemp);//����Ϊ M*D*K��
	}
	vector <double> weigTemp(numOfFloorModel, 1 / (numOfFloorModel + 0.));
	weight = weigTemp;
	
	//������ռ� ��ʼ��ֵ

	
	//д��ѵ����Ϣ
	for (int i = 0; i < numOfTrain; i++)
	{
		
		for (int j = 0; j < numOfGauss; j++)
		{
			for (int k = 0; k < DIM;k++)
			{
				
				KDGuassMeans[i][j].at<double>(k,0)= rawData[i][j].Mean[k];
				KDGuassCovs[i][j].at<double>(k, k) = rawData[i][j].Cov[k];
				
						
			}
			KDWeight[i][j] = rawData[i][j].Weig;//D*K ��
			Mat temp = KDGuassMeans[i][j].t();
			allMat.push_back(temp.clone());
		}

	}
	vector<int> randItem(numOfFloorModel);
	int sumOfRand = 0;
	for (int j = 0; j < numOfTrain; j++)
	{
		for (int k = 0; k < numOfGauss; k++)

		{
			sumOfRand = 0;
			for (int m = 0; m < numOfFloorModel; m++)
			{
				randItem[m]=rand()%10000;
				sumOfRand += randItem[m];
			}
			for (int m = 0; m < numOfFloorModel; m++)
			{
				Hmjk[m][j][k] = randItem[m] / (sumOfRand + 0.);
			}
		}
	}



	
	//����k��ֵ����

	
	int numOfAdd= NUMOFFLOORGAUSS - allMat.rows;
	Mat allMatTem,labels,centers;
	if (allMat.rows<NUMOFFLOORGAUSS)
	{
		for (int i = 0; i < numOfAdd; i++)
		{
			Mat temp = KDGuassMeans[rand() % numOfTrain][rand() % numOfGauss].t();
			allMat.push_back(temp.clone());
			
		}
	}
	


	allMat.convertTo(allMatTem,CV_32FC1);
	kmeans(allMatTem, NUMOFFLOORGAUSS, labels, TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0),
		3, KMEANS_PP_CENTERS, centers);
	
	centers.convertTo(centers,CV_64FC1);//��ֵ���� Ԥ��
	//�����ʼ�� Ԥ��
	//
	Mat sumMeanMat = Mat::zeros(DIM, 1, CV_64FC1);//��ֵ�Ӻ�
	Mat sumCovMat = Mat::zeros(DIM, DIM, CV_64FC1);//Э����Ӻ�
	for (int i = 0; i < numOfTrain; i++)
	{
		for (int j = 0; j < numOfGauss; j++)
		{
			sumMeanMat += KDGuassMeans[i][j];
		}

	}

	sumMeanMat = sumMeanMat / numOfTrain;
	Mat covMatTemp = Mat::zeros(DIM, DIM, CV_64FC1);
	Mat subMatTemp = Mat::zeros(DIM, 1, CV_64FC1);
	for (int i = 0; i < numOfTrain; i++)
	{
		for (int j = 0; j < numOfGauss; j++)
		{
			subMatTemp = (sumMeanMat - KDGuassMeans[i][j]);
			for (int k = 0; k < DIM; k++)
			{
				covMatTemp.at<double>(k, k) = pow(subMatTemp.at<double>(k,0),2);
			}
			//sumCovMat += (covMatTemp + KDGuassCovs[i][j])*KDWeight[i][j];

			sumCovMat += ( KDGuassCovs[i][j])*KDWeight[i][j];
			
		}
	}
	sumCovMat = sumCovMat / numOfTrain;
	//д���ֵ ����
	for (int i = 0; i < NUMOFFLOORGAUSS;i++)
	{
		means[i] = centers.row(i).t();
		
		covs[i] = sumCovMat.clone();
		
	}
	
	L = -99999999;

	
	for (int i = 0; i < NUMOFFLOORGAUSS;i++)
	{
		MeanCovWeight temp;
		FloorMCW.push_back(temp);
	}
}
void FloorMulGauss::EStep()
{
	double sumOfWeig = 0.;
	vector<double> sumOfWeigVec(numOfTrain*numOfGauss);
	for (int j = 0; j < numOfTrain; j++)
	{
		for (int k= 0; k < numOfGauss; k++)
		 
		{
			sumOfWeig = 0.;
			for (int m = 0; m < numOfFloorModel;m++)
			{
				
				Lmij[m][j][k] = CalcGaussPoss(means[m], covs[m],KDGuassMeans[j][k] )*exp(-0.5*CalcTrace(covs[m].inv()*KDGuassCovs[j][k]));
				
				Lmij[m][j][k] = pow(Lmij[m][j][k], KDWeight[j][k])*weight[m];
				
				sumOfWeig += Lmij[m][j][k];
				
			}
			
			sumOfWeigVec[j*numOfGauss + k] = sumOfWeig;
		}
	}

	//����h
	for (int j = 0; j < numOfTrain; j++)
	{
		for (int k = 0; k < numOfGauss; k++)
		{	
			for (int m = 0; m < numOfFloorModel; m++)
			{

					Hmjk[m][j][k] = Lmij[m][j][k] / sumOfWeigVec[j*numOfGauss + k];


				
			}
		}
	}
	//�������
}
void FloorMulGauss::MStep()
{
	
	vector<double> sumOfHPie(numOfFloorModel);
	vector<double> sumOfHjk(numOfFloorModel);//�������г�ʼ��������

	double sumOfHPieTem = 0.;
	for (int m = 0; m < numOfFloorModel; m++)
	{ 
		sumOfHPieTem = 0.;
		for (int j = 0; j < numOfTrain; j++)
		{
			for (int k = 0; k < numOfGauss; k++)
			{
				sumOfHPieTem += Hmjk[m][j][k] * KDWeight[j][k];
				sumOfHjk[m] += Hmjk[m][j][k];
			}
		}
		sumOfHPie[m] = sumOfHPieTem ;
		
	}

	double temp=0.;

		temp = 0.;
		for (int j = 0; j < numOfTrain; j++)
		{
			for (int k = 0; k < numOfGauss; k++)
			{
				temp = 0.;
				for (int m = 0; m < numOfFloorModel; m++)
				{
					temp += Hmjk[m][j][k];
				}
				
			}
		}
		
	
	for (int m = 0; m < numOfFloorModel; m++)
	{
		
		for (int j = 0; j < numOfTrain; j++)
		{
			for (int k = 0; k < numOfGauss; k++)
			{

					Wmjk[m][j][k] = Hmjk[m][j][k] * KDWeight[j][k] / sumOfHPie[m];
			
			}
		}
		

	}
	
	for (int m = 0; m < numOfFloorModel; m++)
	{
		//Ȩֵ�������
		weight[m] = sumOfHjk[m] / (numOfTrain*numOfGauss);
		means[m] = Mat::zeros(means[m].size(), means[m].type());
		covs[m] = Mat::zeros(covs[m].size(), covs[m].type());


		for (int j = 0; j < numOfTrain; j++)
		{
			for (int k = 0; k < numOfGauss; k++)
			{
				//��ֵ�������
				means[m] += Wmjk[m][j][k] * KDGuassMeans[j][k];
				
			}

		}
		
	}


	Mat covMatTemp = Mat::zeros(DIM, DIM, CV_64FC1);
	Mat subMatTemp = Mat::zeros(DIM, 1, CV_64FC1);
	for (int m = 0; m < numOfFloorModel; m++)
	{
		
		for (int j = 0; j < numOfTrain; j++)
		{
			for (int k = 0; k < numOfGauss; k++)
			{
				
				subMatTemp = (KDGuassMeans[j][k] - means[m]);
				for (int l = 0; l < DIM; l++)
				{
					covMatTemp.at<double>(l, l) = pow(subMatTemp.at<double>(l, 0), 2);
				}
				//Э����������
				covs[m] += (covMatTemp + KDGuassCovs[j][k])*Wmjk[m][j][k];
				//covs[m] += (KDGuassCovs[j][k] + subMatTemp*subMatTemp.t())*Wmjk[m][j][k];
			}

		}
		
	}

	//���������Ȼ����
	double resPossTem;
	L = 0.;
	for (int i = 0; i < numOfTrain;i++)
	{
		for (int j = 0; j < numOfGauss; j++)
		{ 
			resPossTem = 0.;
			for (int m = 0; m < numOfFloorModel;m++)
			{
				resPossTem += weight[m] * CalcGaussPoss(means[m], covs[m], KDGuassMeans[i][j]);
				
			}
			L += (resPossTem);
			
		}
	}

	//����ʱֵд���ļ�

			for (int m = 0; m < numOfFloorModel; m++)
			{
				for (int k = 0; k < DIM; k++)
				{
					FloorMCW[m].Cov[k] = covs[m].at<double>(k,k);
					FloorMCW[m].Mean[k] = means[m].at<double>(k,0);

				}
				FloorMCW[m].Weig = weight[m];
			}
	

}
void FloorMulGauss::train()
{
	double LTemp = L;
	static int  count = 0;
	do
	{ 
		count++;
		LTemp = L;
		cout << LTemp<<endl;

		EStep();
		MStep();
		
	} while (L>LTemp&&count<20);
	//д�������ļ�
	DataWrite(fileName.c_str(), FloorMCW);
}