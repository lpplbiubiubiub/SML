#include "FileRead.h"
void readTxt(string file, Mat&resMat)
{
	
	cout << file << endl;
	ifstream infile;
	//复制代码
	infile.open(file.data());   //将文件流对象与文件连接起来 
	//若失败,则输出错误消息,并终止程序运行 
	string s;
	vector<string> svec;
	string buf;
	int count = 0;
	while (getline(infile, s))
	{
		
		stringstream ss(s);
		while (ss >> buf)
		{
			svec.push_back(buf);
		}

		Mat temp = Mat::zeros(1, svec.size(), CV_32SC1);
		for (int i = 0; i < svec.size(); i++)
		{

			temp.at<int>(0, i) = stoi(svec[i]);

		}
		svec.clear();
		resMat.push_back(temp.clone());

	}
	infile.close();             //关闭文件输入流 
	
}

void readFilePath(string file, vector<string>& filePath)
{
	ifstream infile;
	//复制代码
	infile.open(file.data());   //将文件流对象与文件连接起来 
	//assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行 


	string s;

	while (getline(infile, s))
	{
		filePath.push_back("Corel5k//" + s + ".data");
	}
}

void readImgPath(string file, vector<string>& filePath)
{
	ifstream infile;
	//复制代码
	infile.open(file.data());   //将文件流对象与文件连接起来 
	//assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行 


	string s;

	while (getline(infile, s))
	{
		filePath.push_back("Corel5k//" + s + ".jpeg");
	}
}
void readFileName(string file, vector<string>& filePath)
{
	ifstream infile;
	//复制代码
	infile.open(file.data());   //将文件流对象与文件连接起来 
	//assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行 


	string s;

	while (getline(infile, s))
	{
		filePath.push_back(s);
	}

}
void getFiles(string path, string exd, vector<string>& files)
{
	//文件句柄
	long   hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string pathName, exdName;

	if (0 != strcmp(exd.c_str(), ""))//比较后缀名是否为空
	{
		exdName = "\\*." + exd;//若后缀名不为空
	}
	else
	{
		exdName = "\\*";//后缀名为空
	}

	if ((hFile = _findfirst(pathName.assign(path).append(exdName).c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是文件夹中仍有文件夹,迭代之
			//如果不是,加入列表
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(pathName.assign(path).append("\\").append(fileinfo.name), exd, files);
			}
			else
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					files.push_back(pathName.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}



void get_image_path(string path, vector<string>& file_path_vec)
{
	struct _finddata_t   filefind;
	string  curr = path + "\\*.*";
	intptr_t handle;

	if ((handle = _findfirst(curr.c_str(), &filefind)) == -1)
		return;

	while (!_findnext(handle, &filefind))
	{
		if (!strcmp(filefind.name, "..")){
			continue;
		}

		if ((_A_SUBDIR == filefind.attrib)) //是目录
		{
		}
		else{  //不是目录，是文件

			string temp = filefind.name;
			int tempId = temp.find_last_of(".");
			string extname = temp.substr(tempId + 1);
			if (extname == "png" || extname == "jpg" || extname == "PNG" || extname == "JPG" || extname == "jpeg")
			{
				file_path_vec.push_back(path + "\\" + filefind.name);
			}
		}
	}
	_findclose(handle);
}


CBrowseDir::CBrowseDir()
{
	//用当前目录初始化m_szInitDir  
	_getcwd(m_szInitDir, _MAX_PATH);

	//如果目录的最后一个字母不是'\',则在最后加上一个'\'  
	int len = strlen(m_szInitDir);
	if (m_szInitDir[len - 1] != '\\')
		strcat_s(m_szInitDir, "\\");
}

bool CBrowseDir::SetInitDir(const char *dir)
{
	//先把dir转换为绝对路径  
	if (_fullpath(m_szInitDir, dir, _MAX_PATH) == NULL)
		return false;

	//判断目录是否存在  
	if (_chdir(m_szInitDir) != 0)
		return false;

	//如果目录的最后一个字母不是'\',则在最后加上一个'\'  
	int len = strlen(m_szInitDir);
	if (m_szInitDir[len - 1] != '\\')
		strcat_s(m_szInitDir, "\\");

	return true;
}

vector<string> CBrowseDir::BeginBrowseFilenames(const char *filespec)
{
	ProcessDir(m_szInitDir, NULL);
	return GetDirFilenames(m_szInitDir, filespec);
}

bool CBrowseDir::BeginBrowse(const char *filespec)
{
	ProcessDir(m_szInitDir, NULL);
	return BrowseDir(m_szInitDir, filespec);
}

bool CBrowseDir::BrowseDir(const char *dir, const char *filespec)
{
	_chdir(dir);

	//首先查找dir中符合要求的文件  
	long hFile;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(filespec, &fileinfo)) != -1)
	{
		do
		{
			//检查是不是目录  
			//如果不是,则进行处理  
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				char filename[_MAX_PATH];
				strcpy_s(filename, dir);
				strcat_s(filename, fileinfo.name);
				cout << filename << endl;
				if (!ProcessFile(filename))
					return false;
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	//查找dir中的子目录  
	//因为在处理dir中的文件时，派生类的ProcessFile有可能改变了  
	//当前目录，因此还要重新设置当前目录为dir。  
	//执行过_findfirst后，可能系统记录下了相关信息，因此改变目录  
	//对_findnext没有影响。  
	_chdir(dir);
	if ((hFile = _findfirst("*.*", &fileinfo)) != -1)
	{
		do
		{
			//检查是不是目录  
			//如果是,再检查是不是 . 或 ..   
			//如果不是,进行迭代  
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp
					(fileinfo.name, "..") != 0)
				{
					char subdir[_MAX_PATH];
					strcpy_s(subdir, dir);
					strcat_s(subdir, fileinfo.name);
					strcat_s(subdir, "\\");
					ProcessDir(subdir, dir);
					if (!BrowseDir(subdir, filespec))
						return false;
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return true;
}

vector<string> CBrowseDir::GetDirFilenames(const char *dir, const char *filespec)
{
	_chdir(dir);
	vector<string>filename_vector;
	filename_vector.clear();

	//首先查找dir中符合要求的文件  
	long hFile;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(filespec, &fileinfo)) != -1)
	{
		do
		{
			//检查是不是目录  
			//如果不是,则进行处理  
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				char filename[_MAX_PATH];
				strcpy_s(filename, dir);
				strcat_s(filename, fileinfo.name);
				filename_vector.push_back(filename);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	//查找dir中的子目录  
	//因为在处理dir中的文件时，派生类的ProcessFile有可能改变了  
	//当前目录，因此还要重新设置当前目录为dir。  
	//执行过_findfirst后，可能系统记录下了相关信息，因此改变目录  
	//对_findnext没有影响。  
	_chdir(dir);
	if ((hFile = _findfirst("*.*", &fileinfo)) != -1)
	{
		do
		{
			//检查是不是目录  
			//如果是,再检查是不是 . 或 ..   
			//如果不是,进行迭代  
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp
					(fileinfo.name, "..") != 0)
				{
					char subdir[_MAX_PATH];
					strcpy_s(subdir, dir);
					strcat_s(subdir, fileinfo.name);
					strcat_s(subdir, "\\");
					ProcessDir(subdir, dir);
					vector<string>tmp = GetDirFilenames(subdir, filespec);
					for (vector<string>::iterator it = tmp.begin(); it<tmp.end(); it++)
					{
						filename_vector.push_back(*it);
					}
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return filename_vector;
}

bool CBrowseDir::ProcessFile(const char *filename)
{
	return true;
}

void CBrowseDir::ProcessDir(const char
	*currentdir, const char *parentdir)
{
}

//从CBrowseDir派生出的子类，用来统计目录中的文件及子目录个数  
class CStatDir :public CBrowseDir
{
protected:
	int m_nFileCount;   //保存文件个数  
	int m_nSubdirCount; //保存子目录个数  

public:
	//缺省构造器  
	CStatDir()
	{
		//初始化数据成员m_nFileCount和m_nSubdirCount  
		m_nFileCount = m_nSubdirCount = 0;
	}

	//返回文件个数  
	int GetFileCount()
	{
		return m_nFileCount;
	}

	//返回子目录个数  
	int GetSubdirCount()
	{
		//因为进入初始目录时，也会调用函数ProcessDir，  
		//所以减1后才是真正的子目录个数。  
		return m_nSubdirCount - 1;
	}

protected:
	//覆写虚函数ProcessFile，每调用一次，文件个数加1  
	virtual bool ProcessFile(const char *filename)
	{
		m_nFileCount++;
		return CBrowseDir::ProcessFile(filename);
	}

	//覆写虚函数ProcessDir，每调用一次，子目录个数加1  
	virtual void ProcessDir
		(const char *currentdir, const char *parentdir)
	{
		m_nSubdirCount++;
		CBrowseDir::ProcessDir(currentdir, parentdir);
	}
};