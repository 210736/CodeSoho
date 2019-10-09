// FTPclient.h: interface for the CFTPclient class. 
// 
////////////////////////////////////////////////////////////////////// 


#if !defined(AFX_FTPCLIENT_H__05003AE0_E234_11D2_970B_00A024EBF6AB__INCLUDED_) 
#define AFX_FTPCLIENT_H__05003AE0_E234_11D2_970B_00A024EBF6AB__INCLUDED_ 

#if _MSC_VER >= 1000 
#pragma once 
#endif // _MSC_VER >= 1000 

#include <Winsock2.h>

#include <map>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
using namespace std;

#define FTP_USER_POST_FIX	".user"


// end add by jimmy
// add for download directory

//待转
struct _FtpLocalPathMatch 
{
	BOOL isDirectory;
	string ftpPath;
	string localPath;
};
typedef multimap<BOOL, _FtpLocalPathMatch*> FileNamePool;
//===============定义结构体  魏冠南 2012-11-30+++
struct FtpInfo 
{
	BOOL isDirectory;
	string ftpItemName;
	long lSize;
};
//===============定义vector用于存放ftpinfo的指针
typedef vector<FtpInfo*> FileInfoList;
//////////////////////////////////////////////////////////////////////////

//模拟CByteArray
class ByteArraySim 
{
public:
	ByteArraySim();
	~ByteArraySim();
	void SetSize(int nNewSize);
	int GetSize();
	BYTE* GetData();
	void RemoveAll();
	BYTE GetAt(int nIndex);

private:
	BYTE* m_pBData;

	int m_iSize;

};

class CFTPclient 
{ 
public:  
	CFTPclient(); 
	~CFTPclient(); 

	//登陆FTP
	//LogOnToServer("192.168.27.213",21,"xnpg", "123456", "", "","", "",1080,0);
	BOOL LogOnToServer(string hostname,int hostport,string username, string password, string acct, string fwhost,string fwusername, string fwpassword,int fwport,int logontype); 

	//退出登陆
	void LogOffServer(); 

	// 上传和下载文件及文件夹
	// 下载文件夹:ftp.MoveAllFiles("\\屏保\\", "D:\\图标\\",FALSE, TRUE, TRUE);
	// 下载文件:ftp.MoveAllFiles("\\屏保\\pingbao.exe", "D:\\图标\\",FALSE, TRUE, TRUE);
	// 上传文件夹:ftp.MoveAllFiles("\\SysMC\\", "D:\\图标\\屏保\\",FALSE, FALSE, TRUE);
	// 上传文件:ftp.MoveAllFiles("\\SysMC\\", "D:\\图标\\test\\J8F-CallLog-1.txt",FALSE, FALSE, TRUE);
	BOOL MoveAllFiles(string rf, string lf, BOOL pasv, BOOL get, BOOL reWrite, string userName = "", BOOL steakPath = TRUE);

	// 下载文件夹中的内容到本地目录
	BOOL DownLoadContentsInFolder(string rf, string lf);

	//发送命令到FTP
	BOOL FTPcommand(string command); 

	// 在FTP上创建空目录
	//CreateDir("/CreateDir_1");
	BOOL CreateDir(string strDir, BOOL pwd = FALSE);

	//删除FTP上目录或文件，目录包含子目录及文件
	//删除文件：ftp.DeleteFtpDir("/屏保/share_remove.reg",FALSE);
	//删除目录：ftp.DeleteFtpDir("/屏保/",TRUE);
	BOOL DeleteFtpDir(string rf, BOOL bDir);

	//删除本地目录，包含所有子目录及文件
	//DeleteLocalDir("D:\\图标\\屏保\\");
	BOOL DeleteLocalDir(string strLocalDir);

	//获得FTP上文件大小,尚未实现
	long GetFTPFileSize(string strPath);

	//获得FTP上文件夹大小，包括子目录及文件,尚未实现
	long GetFTPDirSize(string strDir);

	//获得本地文件大小
	DWORD64 GetLocalFileSize(string szPath);

	//获得本地文件夹大小，包括子目录及文件,尚未实现
	//szPath:文件所在路径

	DWORD64 GetLocalFolderSize(string szPath, DWORD *dwFiles, DWORD *dwFolders);

	// 切换服务器的目录
	BOOL CD(string dir);

	// 回退上一级目录
	// 尚未测试
	BOOL Back();

public: 
	//CByteArray m_buf;
	ByteArraySim m_buf;//模拟MFC CByteArray的部分功能

	SOCKET m_Ctrlsok; //控制命令传输套接字 
	int m_fc; 
	char m_errString[100]; 

	string m_retmsg;
	string m_strLine;

	BOOL m_bConnect;

private:
	BOOL ReadStr2(); 
	BOOL OpenControlChannel(string serverhost,int serverport); 
	void CloseControlChannel(); 

	BOOL ReadStr(); 
	BOOL WriteStr(string outputstring); 
	BOOL List(string strHostDir); 
	BOOL GetLine(int& ndx); 

	BOOL MoveFile(string remotefile, string localfile,BOOL pasv,BOOL get);

	//删除FTP上单个文件
	BOOL DeleteFtpFile(string strFilePath); 

	// 解析文件列表的一行(即当前m_strLine的内容)
	static BOOL ParseLine(string strLine,	//	传入的待解析串
		BOOL&	m_bDirectory,				// 是否是路径
		WORD&	m_bMask,						// 权限字符串中有效位标记
		string& m_strAuth,						// 权限字符串
		string& m_strDate,					// 创建时间
		string& m_size,							// 文件大小
		string& m_strName);					// 文件名


	BOOL DownloadAllFiles(string remotePath, string localPath, BOOL pasv, BOOL get, BOOL reWrite);

	BOOL UploadAllFiles(string remotePath, string localPath, BOOL pasv, BOOL get, HANDLE hd);

	//下载单个文件
	BOOL DownLoadFile(string strRemoteFile, string strLocalFile,BOOL bPasv);

	//上传单个文件
	BOOL UpLoadFile(string strRemoteFile, string strLocalFile,BOOL bPasv=FALSE);

	//获取FTP上文件的长度
	int GetFTPFileLen(string strFilePath); 

	//将string中的大写字母转换为小写
	string MakeStringLower(string str);

	//string模拟CString的Tokenize
	static string TokenizeString(string strSourse,int& iCurPos);


	static string ws2s(const wstring& ws);

	static wstring utf82ws(const string& s);

	//xue-2013.08添加
	static wstring s2ws(const string& s);
	static string ws2utf8(const wstring& ws);



private:
	//===========递归方法计算文件大小  魏冠南 2012-11-30
	long GetDirSize(FtpInfo* pFileInfo,string strPath);

	//xue-整理程序

	//功能：打开本地文件，上传时使用
	//输入：strLocalFile本地文件全路径
	//输出：datafile文件流，LocalFileLen文件长度
	//返回：成功返回true
	bool OpenLocalFile(string strLocalFile,ifstream& datafile,long& LocalFileLen);

	//功能：创建本地文件，下载文件时使用
	//输入：strLocalFile本地文件全路径
	//输出：datafile文件流
	//返回：成功返回true
	bool CreateLocalFile(string strLocalFile,ofstream& datafile);

	//功能：创建接收数据的套接字，并发送PORT命令到服务器端
	//输出：sockSrvr返回接收连接的套接字
	//返回：成功返回true
	bool ListenAndSendPort(SOCKET& sockSrvr);

	//功能：用socket上传文件
	//输入：sockSrvr监听套接字，datafile文件流
	//返回：成功返回true
	bool SendFileData(SOCKET& sockSrvr,ifstream& datafile);

	//功能：接收文件到本地，下载文件
	//输入：sockSrvr监听套接字，datafile文件流
	//返回：成功返回true
	bool ReceiveFileData(SOCKET& sockSrvr,ofstream& datafile);

	//功能：发送LIST命令，并接收回复信息，回复信息放在m_buf中
	//输入：strHostDir远程Ftp目录，sockSrvr监听套接字
	//返回：成功返回true
	bool SendListAndRecvEcho(string strHostDir,SOCKET& sockSrvr);

private:
	//控制端返回的字符串队列，按行划分
	deque<string> m_ReturnStringDeq;

	//xue-2013.08add
private:
	locale m_loc;

}; 

#endif // !defined(AFX_FTPCLIENT_H__05003AE0_E234_11D2_970B_00A024EBF6AB__INCLUDED_) 

