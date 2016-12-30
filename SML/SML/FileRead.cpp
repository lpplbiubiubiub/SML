#include "FileRead.h"
void readTxt(string file, Mat&resMat)
{
	
	cout << file << endl;
	ifstream infile;
	//���ƴ���
	infile.open(file.data());   //���ļ����������ļ��������� 
	//��ʧ��,�����������Ϣ,����ֹ�������� 
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
	infile.close();             //�ر��ļ������� 
	
}

void readFilePath(string file, vector<string>& filePath)
{
	ifstream infile;
	//���ƴ���
	infile.open(file.data());   //���ļ����������ļ��������� 
	//assert(infile.is_open());   //��ʧ��,�����������Ϣ,����ֹ�������� 


	string s;

	while (getline(infile, s))
	{
		filePath.push_back("Corel5k//" + s + ".data");
	}
}

void readImgPath(string file, vector<string>& filePath)
{
	ifstream infile;
	//���ƴ���
	infile.open(file.data());   //���ļ����������ļ��������� 
	//assert(infile.is_open());   //��ʧ��,�����������Ϣ,����ֹ�������� 


	string s;

	while (getline(infile, s))
	{
		filePath.push_back("Corel5k//" + s + ".jpeg");
	}
}
void readFileName(string file, vector<string>& filePath)
{
	ifstream infile;
	//���ƴ���
	infile.open(file.data());   //���ļ����������ļ��������� 
	//assert(infile.is_open());   //��ʧ��,�����������Ϣ,����ֹ�������� 


	string s;

	while (getline(infile, s))
	{
		filePath.push_back(s);
	}

}
void getFiles(string path, string exd, vector<string>& files)
{
	//�ļ����
	long   hFile = 0;
	//�ļ���Ϣ
	struct _finddata_t fileinfo;
	string pathName, exdName;

	if (0 != strcmp(exd.c_str(), ""))//�ȽϺ�׺���Ƿ�Ϊ��
	{
		exdName = "\\*." + exd;//����׺����Ϊ��
	}
	else
	{
		exdName = "\\*";//��׺��Ϊ��
	}

	if ((hFile = _findfirst(pathName.assign(path).append(exdName).c_str(), &fileinfo)) != -1)
	{
		do
		{
			//������ļ����������ļ���,����֮
			//�������,�����б�
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

		if ((_A_SUBDIR == filefind.attrib)) //��Ŀ¼
		{
		}
		else{  //����Ŀ¼�����ļ�

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
	//�õ�ǰĿ¼��ʼ��m_szInitDir  
	_getcwd(m_szInitDir, _MAX_PATH);

	//���Ŀ¼�����һ����ĸ����'\',����������һ��'\'  
	int len = strlen(m_szInitDir);
	if (m_szInitDir[len - 1] != '\\')
		strcat_s(m_szInitDir, "\\");
}

bool CBrowseDir::SetInitDir(const char *dir)
{
	//�Ȱ�dirת��Ϊ����·��  
	if (_fullpath(m_szInitDir, dir, _MAX_PATH) == NULL)
		return false;

	//�ж�Ŀ¼�Ƿ����  
	if (_chdir(m_szInitDir) != 0)
		return false;

	//���Ŀ¼�����һ����ĸ����'\',����������һ��'\'  
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

	//���Ȳ���dir�з���Ҫ����ļ�  
	long hFile;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(filespec, &fileinfo)) != -1)
	{
		do
		{
			//����ǲ���Ŀ¼  
			//�������,����д���  
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
	//����dir�е���Ŀ¼  
	//��Ϊ�ڴ���dir�е��ļ�ʱ���������ProcessFile�п��ܸı���  
	//��ǰĿ¼����˻�Ҫ�������õ�ǰĿ¼Ϊdir��  
	//ִ�й�_findfirst�󣬿���ϵͳ��¼���������Ϣ����˸ı�Ŀ¼  
	//��_findnextû��Ӱ�졣  
	_chdir(dir);
	if ((hFile = _findfirst("*.*", &fileinfo)) != -1)
	{
		do
		{
			//����ǲ���Ŀ¼  
			//�����,�ټ���ǲ��� . �� ..   
			//�������,���е���  
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

	//���Ȳ���dir�з���Ҫ����ļ�  
	long hFile;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(filespec, &fileinfo)) != -1)
	{
		do
		{
			//����ǲ���Ŀ¼  
			//�������,����д���  
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
	//����dir�е���Ŀ¼  
	//��Ϊ�ڴ���dir�е��ļ�ʱ���������ProcessFile�п��ܸı���  
	//��ǰĿ¼����˻�Ҫ�������õ�ǰĿ¼Ϊdir��  
	//ִ�й�_findfirst�󣬿���ϵͳ��¼���������Ϣ����˸ı�Ŀ¼  
	//��_findnextû��Ӱ�졣  
	_chdir(dir);
	if ((hFile = _findfirst("*.*", &fileinfo)) != -1)
	{
		do
		{
			//����ǲ���Ŀ¼  
			//�����,�ټ���ǲ��� . �� ..   
			//�������,���е���  
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

//��CBrowseDir�����������࣬����ͳ��Ŀ¼�е��ļ�����Ŀ¼����  
class CStatDir :public CBrowseDir
{
protected:
	int m_nFileCount;   //�����ļ�����  
	int m_nSubdirCount; //������Ŀ¼����  

public:
	//ȱʡ������  
	CStatDir()
	{
		//��ʼ�����ݳ�Աm_nFileCount��m_nSubdirCount  
		m_nFileCount = m_nSubdirCount = 0;
	}

	//�����ļ�����  
	int GetFileCount()
	{
		return m_nFileCount;
	}

	//������Ŀ¼����  
	int GetSubdirCount()
	{
		//��Ϊ�����ʼĿ¼ʱ��Ҳ����ú���ProcessDir��  
		//���Լ�1�������������Ŀ¼������  
		return m_nSubdirCount - 1;
	}

protected:
	//��д�麯��ProcessFile��ÿ����һ�Σ��ļ�������1  
	virtual bool ProcessFile(const char *filename)
	{
		m_nFileCount++;
		return CBrowseDir::ProcessFile(filename);
	}

	//��д�麯��ProcessDir��ÿ����һ�Σ���Ŀ¼������1  
	virtual void ProcessDir
		(const char *currentdir, const char *parentdir)
	{
		m_nSubdirCount++;
		CBrowseDir::ProcessDir(currentdir, parentdir);
	}
};