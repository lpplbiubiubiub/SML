#ifndef EM_H
#define EM_H


const int NUMOFGAUSS = 8;
const int NUMOFROWS = 10;
const int NUMOFFLOORGAUSS = 64;
struct MeanCovWeight
{
	double Mean[NUMOFROWS];
	double Cov[NUMOFROWS];//Ϊ�ԽǾ���
	double Weig;
};




#endif // !EM_H