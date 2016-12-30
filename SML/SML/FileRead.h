#ifndef FILEREAD_H
#define FILEREAD_H
#include "stdlib.h"  
#include "direct.h"  
#include "string.h"  
#include "string"  
#include "io.h"  
#include "stdio.h"   
#include <vector>  
#include <iostream>
#include <fstream>
#include <opencv2\opencv.hpp>
using namespace cv;
using namespace std;
void readTxt(string file, Mat& resMat);

void readFilePath(string file, vector<string>& filePath);

void readImgPath(string file, vector<string>& filePath);
void readFileName(string file, vector<string>& filePath);
void getFiles(string path, string exd, vector<string>& files);



void get_image_path(string path, vector<string>& file_path_vec);




class CBrowseDir
{
protected:
	//��ų�ʼĿ¼�ľ���·������'\'��β  
	char m_szInitDir[_MAX_PATH];

public:
	//ȱʡ������  
	CBrowseDir();

	//���ó�ʼĿ¼Ϊdir���������false����ʾĿ¼������  
	bool SetInitDir(const char *dir);

	//��ʼ������ʼĿ¼������Ŀ¼����filespecָ�����͵��ļ�  
	//filespec����ʹ��ͨ��� * ?�����ܰ���·����  
	//�������false����ʾ�������̱��û���ֹ  
	bool BeginBrowse(const char *filespec);
	vector<string> BeginBrowseFilenames(const char *filespec);

protected:
	//����Ŀ¼dir����filespecָ�����ļ�  
	//������Ŀ¼,���õ����ķ���  
	//�������false,��ʾ��ֹ�����ļ�  
	bool BrowseDir(const char *dir, const char *filespec);
	vector<string> GetDirFilenames(const char *dir, const char *filespec);
	//����BrowseDirÿ�ҵ�һ���ļ�,�͵���ProcessFile  
	//�����ļ�����Ϊ�������ݹ�ȥ  
	//�������false,��ʾ��ֹ�����ļ�  
	//�û����Ը�д�ú���,�����Լ��Ĵ�������  
	virtual bool ProcessFile(const char *filename);

	//����BrowseDirÿ����һ��Ŀ¼,�͵���ProcessDir  
	//�������ڴ�����Ŀ¼������һ��Ŀ¼����Ϊ�������ݹ�ȥ  
	//������ڴ������ǳ�ʼĿ¼,��parentdir=NULL  
	//�û����Ը�д�ú���,�����Լ��Ĵ�������  
	//�����û�����������ͳ����Ŀ¼�ĸ���  
	virtual void ProcessDir(const char *currentdir, const char *parentdir);
};


#endif