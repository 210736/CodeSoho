/*/////////////////////////////////////////////////////////////////////  

FTPclient.cpp (c) GDI 1999  

作者: Caps77  


/////////////////////////////////////////////////////////////////////*/   

#include "stdafx.h"    

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>


#include "ftpclass.h"   

#include <Shellapi.h>//文件操作
#include <assert.h>



//////////////////////////////////////////////////////////////////////    
// Construction/Destruction    
//////////////////////////////////////////////////////////////////////    

//构造函数，变量初始化    
CFTPclient::CFTPclient()   
{   
	m_retmsg = ""; 
	m_strLine = ""; 

	m_ReturnStringDeq.clear();

	m_bConnect = FALSE;

	//xue-2013.08add
	m_loc = locale::global(locale(".936"));

}   


CFTPclient::~CFTPclient()   
{
	CloseControlChannel();

	//xue-2013.08add
	locale::global(m_loc);
}   


//////////////////////////////////////////////////////////////////////    
// Public Functions    
//////////////////////////////////////////////////////////////////////    


// 登录到服务器
BOOL CFTPclient::LogOnToServer(string hostname,int hostport,string username, string password, string acct, string fwhost,string fwusername, string fwpassword,int fwport,int logontype) 
{
	int port,logonpoint=0;
	const int LO=-2, ER=-1;
	string buf,temp;
	const int NUMLOGIN=9; // 支持9种不同的登录方式    
	int logonseq[NUMLOGIN][100] = {   
		// 下面的数组保存了针对不同防火墙的登录序列    
		{0,LO,3, 1,LO,6, 2,LO,ER}, // 没有防火墙    
		{3,6,3, 4,6,ER, 5,ER,9, 0,LO,12, 1,LO,15, 2,LO,ER}, // 主机名    
		{3,6,3, 4,6,ER, 6,LO,9, 1,LO,12, 2,LO,ER}, // USER after logon    
		{7,3,3, 0,LO,6, 1,LO,9, 2,LO,ER}, //proxy OPEN    
		{3,6,3, 4,6,ER, 0,LO,9, 1,LO,12, 2,LO,ER}, // Transparent    
		{6,LO,3, 1,LO,6, 2,LO,ER}, // USER with no logon    
		{8,6,3, 4,6,ER, 0,LO,9, 1,LO,12, 2,LO,ER}, //USER fireID@remotehost    
		{9,ER,3, 1,LO,6, 2,LO,ER}, //USER remoteID@remotehost fireID    
		{10,LO,3, 11,LO,6, 2,LO,ER} // USER remoteID@fireID@remotehost    
	};

	if(logontype<0||logontype>=NUMLOGIN)
	{
		return FALSE; // illegal connect code 
	}

	if(!logontype) 
	{   
		temp=hostname;
		port=hostport;
	}   
	else 
	{   
		temp=fwhost;
		port=fwport;
	}   


	if(hostport!=21)
	{
		TCHAR charPort[10] = {0};
		sprintf_s(charPort,":%d",hostport);
		string strPort = charPort;

		hostname += strPort;
		//hostnameCopy.Format(hostnameCopy+":%d",hostport); // 如果端口不是默认端口21，则设定端口
	}

	m_ReturnStringDeq.clear();

	if(!OpenControlChannel(temp,port))
	{
		return FALSE; 
	}

	if(!FTPcommand(""))
	{
		return FALSE; // 获得连接服务器初始化信息
	}


	// 获得登录类型   
	string strUserTag = "USER ";
	string strPassTag = "PASS ";
	string strAcctTag = "ACCT ";
	string strSiteTag = "SITE ";
	string strAiteTag = "@";
	string strOpenTag = "OPEN ";
	string strSpaceTag = " ";

	while(1) 
	{   
		switch(logonseq[logontype][logonpoint]) 
		{   
		case 0: 
			temp = strUserTag + username;
			break;
		case 1:   
			temp = strPassTag + password;
			break;
		case 2:   
			temp = strAcctTag + acct;
			break;
		case 3:   
			temp = strUserTag + fwusername;
			break;
		case 4:   
			temp = strPassTag + fwpassword;
			break;
		case 5:   
			temp = strSiteTag + hostname;
			break;
		case 6:   
			temp = strUserTag + username + strAiteTag + hostname;
			break;
		case 7:   
			temp = strOpenTag + hostname;
			break;
		case 8:   
			temp = strUserTag + fwusername + strAiteTag + hostname;
			break;
		case 9:   
			temp = strUserTag + username + strAiteTag + hostname + strSpaceTag + fwusername;
			break;
		case 10:   
			temp = strUserTag + username + strAiteTag + fwusername + strAiteTag + hostname;
			break;
		case 11:   
			temp = strPassTag + password + strAiteTag + fwpassword;
			break;
		}   

		// 发出命令，获得响应
		if(!WriteStr(temp))
		{
			return FALSE;
		}

		if(!ReadStr())
		{    
			return FALSE;
		} 

		// 只有这些响应是合法的
		if(m_fc!=2&&m_fc!=3)
		{
			return FALSE;
		}
		logonpoint=logonseq[logontype][logonpoint+m_fc-1]; //get next command from array

		switch(logonpoint) 
		{   
		case ER: // 出现错误
			m_retmsg = "无法登录ftp，请检测用户名或密码是否正确！";
			return FALSE;
		case LO: // L0表示成功登录
			{
				m_bConnect = TRUE;
				return TRUE;
			}
		}   
	}   
}   


// 退出服务器
void CFTPclient::LogOffServer() 
{
	if (m_bConnect)
	{
		WriteStr("QUIT");
	}

	CloseControlChannel();
}   


// 发送命令到服务器
BOOL CFTPclient::FTPcommand(string command) 
{   
	if(command!=""&&!WriteStr(command))
	{
		return FALSE;
	}
	if((!ReadStr())||((m_fc!=2)&&(m_fc!=1)))
	{
		return FALSE;
	}

	return TRUE;
}   


// 上载或者下载文件，支持断点续传    
BOOL CFTPclient::MoveFile(string remotefile, string localfile,BOOL pasv,BOOL get) 
{   
	if (get)
	{
		return DownLoadFile(remotefile, localfile,pasv);
	}
	else
	{
		return UpLoadFile(remotefile, localfile,pasv);
	}
}   


// 通过控制通道向服务器发送命令    
BOOL CFTPclient::WriteStr(string outputstring)
{
	int nRet = 0;
	outputstring += "\r\n";

	nRet = ::send(this->m_Ctrlsok, outputstring.c_str(),\
		outputstring.size(), 0);

	if(nRet == 0)
	{
		return FALSE;
	}

	return TRUE;
}   





//////////////////////////////////////////////////////////////////////    
// Private functions    
//////////////////////////////////////////////////////////////////////    

BOOL CFTPclient::ReadStr()
{
	if(!ReadStr2())
	{
		return FALSE;
	}

	if(m_retmsg.size()<4||m_retmsg.at(3)!='-')
	{
		return TRUE;
	}

	int retcode=_ttoi(m_retmsg.c_str());
	while(1) 
	{ 
		//处理多行服务器响应
		if(m_retmsg.size()>3&&(m_retmsg.at(3)==' '&&_ttoi(m_retmsg.c_str())==retcode))
		{
			return TRUE;
		}

		if(!ReadStr2())
		{
			return FALSE;
		}
	}
}

// 从服务器控制通道获取一行响应，并获取第一个数字
BOOL CFTPclient::ReadStr2()
{   
	//如果队列中有数据，直接使用，不用从socket读取
	if (m_ReturnStringDeq.size()>0)
	{
		m_retmsg = m_ReturnStringDeq.front();
		m_ReturnStringDeq.pop_front();

		if(m_retmsg.size()>0)
		{
			m_fc=m_retmsg.at(0)-48; // get 1st digit of the return code (indicates primary result)    
		}

		return TRUE;
	}

	int nRet = 0;
	char *readbuf = new char[1024];
	memset(readbuf, 0, 1024);
	nRet = ::recv(this->m_Ctrlsok, readbuf, 1024, 0);


	if (nRet != 0)
	{
		//按行存储收到的回复
		int i=0;
		string strLeft;
		while (readbuf[i] != '\0')
		{
			strLeft.push_back(readbuf[i]);
			if (readbuf[i] == '\n')
			{
				strLeft.push_back('\0');

				m_ReturnStringDeq.push_back(strLeft);

				strLeft.clear();
			}

			++i;
		}

		//返回一行20130702sp+  18:39发现此处bug，已修改
		if (!m_ReturnStringDeq.empty())
		{
			m_retmsg = m_ReturnStringDeq.front();
			m_ReturnStringDeq.pop_front();

			if(m_retmsg.size()>0)
			{
				m_fc=m_retmsg.at(0)-48; // get 1st digit of the return code (indicates primary result)    
			}
		}



		delete [] readbuf;
		return TRUE;
	}
	else
	{
		delete [] readbuf;
		return FALSE;
	}
}   


// 打开控制通道
BOOL CFTPclient::OpenControlChannel(string serverhost,int serverport) 
{
	int           Error = 0;// catches return value of WSAStartup      
	WORD          VersionRequested;// passed to WSAStartup       
	WSADATA       WsaData;// receives data from WSAStartup     
	BOOL          ReturnValue = TRUE; // return value      
	int           nRet = 0;
	// Start WinSock 2.  If it fails, we don't need to call       
	// WSACleanup().

	VersionRequested = MAKEWORD(2, 0);
	Error = WSAStartup(VersionRequested, &WsaData);
	if(Error)
	{
		ReturnValue = FALSE;
	}
	else
	{
		// Now confirm that the WinSock 2 DLL supports the exact version          
		// we want. If not, make sure to call WSACleanup().          
		if (LOBYTE(WsaData.wVersion) != 2 ||
			HIBYTE(WsaData.wVersion) != 0) 
		{
			WSACleanup();
			ReturnValue = FALSE;
		}
	}

	m_Ctrlsok = socket(AF_INET,SOCK_STREAM,0);
	if (m_Ctrlsok == SOCKET_ERROR)
	{
		nRet = closesocket(m_Ctrlsok);
		::WSACleanup();
		return FALSE;
	}

	struct sockaddr_in sockAddr;
	sockAddr.sin_addr.S_un.S_addr= inet_addr(serverhost.c_str());
	sockAddr.sin_family=AF_INET;
	sockAddr.sin_port=htons(serverport);
	if(0 != (connect(m_Ctrlsok, (sockaddr*)&sockAddr,sizeof(sockAddr))))
	{   
		int nErr = ::WSAGetLastError(); 
		nRet = closesocket(m_Ctrlsok);

		::WSACleanup();
		return FALSE;
	}


	//xue-add
	char opBuffer[10] = {0};
	if (setsockopt(m_Ctrlsok,IPPROTO_TCP,TCP_NODELAY,opBuffer,10) != 0)
	{
		return FALSE;
	}

	return TRUE;
}   


// 关闭控制通道
void CFTPclient::CloseControlChannel() 
{
	::shutdown(m_Ctrlsok, 2);
	::closesocket(m_Ctrlsok);
	::WSACleanup();

	return;
}   


//列出文件列表
BOOL CFTPclient::List(string strHostDir)
{
	if(!FTPcommand("TYPE I"))
	{
		return FALSE; // 请求二进制模式
	}


	//创建监听套接字并发送PORT命令
	SOCKET sockSrvr;//监听ftpsocket
	if (!ListenAndSendPort(sockSrvr))
	{
		return FALSE;
	}


	//发送LIST命令并接收服务器传回的数据
	if (!SendListAndRecvEcho(strHostDir,sockSrvr))
	{
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);

		return FALSE;
	}

	::shutdown(sockSrvr, 2);
	::closesocket(sockSrvr);

	return TRUE;
}   


BOOL CFTPclient::GetLine(int& ndx)   
{   
	m_strLine = "";
	int nBytes = m_buf.GetSize();
	BOOL bLine = FALSE;
	while ( bLine == FALSE && ndx < nBytes )   
	{   
		char ch = (char)(m_buf.GetAt( ndx ));
		switch( ch )   
		{   
		case '\n': // 行尾    
			bLine = TRUE;
			break;
		default:   // 其他情况    
			m_strLine.push_back(ch);
			break;
		}   
		++ndx;
	}   
	//m_strLine = m_strLine.Left(m_strLine.GetLength() - 1);
	string strLineLeft = m_strLine.substr(0,m_strLine.size() - 1);
	m_strLine = ws2s(utf82ws(strLineLeft));

	return bLine;
}   

BOOL CFTPclient::DeleteFtpFile(string strFilePath)   
{   
	//xue-2013.08
	string remoteDirUTF8=ws2utf8(s2ws(strFilePath));

	string outputstring = "DELE";
	outputstring += " ";
	outputstring += remoteDirUTF8;
	char temp[100];
	string strtemp;
	if(!WriteStr(outputstring))
	{
		return FALSE; 
	}

	if(this->FTPcommand(""))
	{   
		int i = 0;
		memset(temp, 0, 100);
		strcpy_s(temp, (const char*)m_retmsg.c_str());
		strtemp = temp;

		strtemp = MakeStringLower(strtemp);
		//strtemp.MakeLower();

		i = strtemp.find("no such", 0);
		if(i != -1)
		{
			return FALSE;
		}

		i = strtemp.find("dele command successful", 0);
		if(i != -1)
		{
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}   


//根据文件ftp路径获取文件长度
int CFTPclient::GetFTPFileLen(string strFilePath)   
{   
	string rmFileDir, rmFileName, strTemp, strFileName;
	BOOL bDirectory = 1;
	char szrmFile[_MAX_PATH];
	int nIndex = 0;
	rmFileDir = "";
	memset(szrmFile, 0, _MAX_PATH);
	//strncpy(szrmFile, strFilePath, strFilePath.GetLength());
	strncpy_s(szrmFile, strFilePath.c_str(), strFilePath.size());
	if(!List(szrmFile))  
	{
		return -1;
	}

	string strFileList;
	int i = 0;
	int nSize = m_buf.GetSize();
	char* fileString = new char[nSize];
	for(i = 0; i < nSize; i++){   
		fileString[i] = m_buf.GetAt(i);
	}   
	strFileList = fileString;
	delete [] fileString;
	nIndex = 0;
	if((nIndex = strFileList.find("Jan")) != -1)   
		goto ExitFind;
	else if((nIndex = strFileList.find("Feb")) != -1)   
		goto ExitFind;
	else if((nIndex = strFileList.find("Mar")) != -1)   
		goto ExitFind;
	else if((nIndex = strFileList.find("Apr")) != -1)   
		goto ExitFind;
	else if((nIndex = strFileList.find("May")) != -1)   
		goto ExitFind;
	else if((nIndex = strFileList.find("Jun")) != -1)   
		goto ExitFind;
	else if((nIndex = strFileList.find("Jul")) != -1)   
		goto ExitFind;
	else if((nIndex = strFileList.find("Aug")) != -1)   
		goto ExitFind;
	else if((nIndex = strFileList.find("Sep")) != -1)   
		goto ExitFind;
	else if((nIndex = strFileList.find("Oct")) != -1)   
		goto ExitFind;
	else if((nIndex = strFileList.find("Nov")) != -1)   
		goto ExitFind;
	else if((nIndex = strFileList.find("Dec")) != -1)   
		goto ExitFind;

ExitFind:   
	//strTemp = strFileList.Left(nIndex - 1);
	strTemp = strFileList.substr(0,nIndex - 1);

	while(1)
	{   
		nIndex = strTemp.find(" ", 0);

		// strTemp.Delete(0, nIndex + 1);
		strTemp.erase(0,nIndex + 1);
		if(nIndex == -1)   
			break;
	}   
	if(strTemp == "")   
		return 0;
	else   
		return (atol(strTemp.c_str()));
}   

BOOL CFTPclient::MoveAllFiles(string rf, string lf, BOOL pasv, BOOL get, BOOL reWrite, string userName, BOOL steakPath)
{
	if (get)//下载
	{
		if (steakPath && ((rf.rfind('\\') == rf.size() - 1) || (rf.rfind('/') == rf.size() - 1)))
		{
			// 从rfCopy中获取文件夹的名称:
			string t_ItemName = rf.substr(0,rf.size()-1);

			t_ItemName = t_ItemName.substr(t_ItemName.rfind('\\')+1,t_ItemName.size() - t_ItemName.rfind('\\') - 1);
			//t_ItemName = t_ItemName.Right(t_ItemName.GetLength() - t_ItemName.ReverseFind('\\') - 1);

			if (lf.rfind('\\') != lf.size() - 1){	lf += "\\";	}
			lf += t_ItemName;
			lf += "\\";	
			if (CreateDirectory(lf.c_str() ,NULL) != TRUE && GetLastError() != ERROR_ALREADY_EXISTS)
			{
				return FALSE;
			}

		}else
		{
			//CString t_ItemName = rfCopy.Right(rfCopy.GetLength() - rfCopy.ReverseFind('\\') - 1);
			string t_ItemName = rf.substr(rf.rfind('\\') + 1,rf.size() - rf.rfind('\\') - 1);

			if (lf.rfind('\\') != lf.size() - 1){	lf += "\\";	}
			lf += t_ItemName;
		}

		return DownloadAllFiles(rf, lf, pasv, get, reWrite);
	}else//上传
	{
		// 判断lf 是文件还是目录
		//
		if (((lf.rfind('\\') != lf.size() - 1) &&(lf.rfind('/') != lf.size() - 1)))
		{
			//单个文件的情况
			string t_ItemName = lf.substr(lf.rfind('\\') + 1,\
				lf.size() - lf.rfind('\\') - 1);

			//可以允许远程目录末尾不带'\'
			if (rf.rfind('\\') != rf.size() - 1)
			{
				rf += "\\";
			}

			rf += t_ItemName;

			return MoveFile(rf, lf, pasv, get);
		}

		//上传的是目录
		//
		// 从lf中获取本地文件夹的名称
		string t_ItemName = lf.substr(0,lf.size() - 1);
		t_ItemName = t_ItemName.substr(t_ItemName.rfind('\\')+1,\
			t_ItemName.size() - t_ItemName.rfind('\\') - 1);

		BOOL rc = TRUE;
		// 递归访问所有文件
		HANDLE hFind = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATA FindFileData;

		string lfAll = lf + "*";
		hFind = FindFirstFile(lfAll.c_str(), &FindFileData);

		assert(hFind != INVALID_HANDLE_VALUE);
		BOOL hasCreatUserFolder = FALSE;
		while (FindNextFile(hFind, &FindFileData) != 0) 
		{
			string t_lf = FindFileData.cFileName;
			if (t_lf == "..")
			{
				continue;
			}
			t_lf = lf;
			string t_rf = rf;
			if (!hasCreatUserFolder)
			{
				rf += t_ItemName;
				rf += "\\";	
				t_rf  = rf;
				if (!CreateDir(t_rf))
				{
					return FALSE;
				}

				hasCreatUserFolder = TRUE;
			}
			t_lf += FindFileData.cFileName;
			t_rf += FindFileData.cFileName;
			HANDLE t_hFind = hFind;
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				t_lf += "\\";
				t_rf += "\\";
				if (!CreateDir(t_rf))
				{
					return FALSE;
				}

				string str_tlf_all = t_lf + "*";
				t_hFind = FindFirstFile(str_tlf_all.c_str(), &FindFileData);
			}
			rc &= UploadAllFiles(t_rf, t_lf, pasv, get, t_hFind);
		}
		FindClose(hFind);
		return rc;
	}
}


// 下载文件夹中的内容到本地目录
BOOL CFTPclient::DownLoadContentsInFolder(string rf, string lf)
{
	return DownloadAllFiles(rf, lf, FALSE, TRUE, TRUE);
}


// 下载文件和文件夹
BOOL CFTPclient::DownloadAllFiles(string remotePath, string localPath, BOOL pasv, BOOL get, BOOL reWrite)
{
	string rf = remotePath;
	string lf = localPath;

	if (((rf.rfind('\\') != rf.size() - 1) &&(rf.rfind('/') != rf.size() - 1)))
	{
		// 单个文件
		return this->MoveFile(rf, lf, pasv, get/*, reWrite*/);
	}	


	//切换到当前目录
	BOOL rc = CD(rf);	
	if (!rc)
	{
		return FALSE;
	}

	//获取远程目录
	rc = List(rf);

	int ndx = 0;
	string listName = "";
	BOOL dir;
	WORD ti;
	string strName, strDate, strSec, strSize;

	FileNamePool thisFloorFiles;
	_FtpLocalPathMatch* t_pMatch = NULL;
	while(GetLine(ndx))
	{	
		//strName = ParseLine(m_strLine, dir);
		ParseLine(m_strLine, dir, ti, strSec, strDate,strSize, strName);

		t_pMatch =  new _FtpLocalPathMatch;
		t_pMatch->isDirectory = dir;
		t_pMatch->ftpPath = rf + strName;

		if (strName.find(FTP_USER_POST_FIX) != -1 && dir)
		{
			t_pMatch->localPath = lf.erase(lf.size() - 1);
		}else
		{
			t_pMatch->localPath = lf + strName;
		}

		thisFloorFiles.insert(make_pair(dir, t_pMatch));
	}

	for (FileNamePool::iterator iter = thisFloorFiles.begin(); iter != thisFloorFiles.end(); ++iter)
	{
		t_pMatch = iter->second;
		if (iter->first)
		{		
			// 创建路径
			t_pMatch->localPath += "\\";
			t_pMatch->ftpPath += "\\";
			if (CreateDirectory((t_pMatch->localPath).c_str() ,NULL) != TRUE && GetLastError() != ERROR_ALREADY_EXISTS)
			{
				rc = FALSE;
			}
		}
		rc &= DownloadAllFiles(t_pMatch->ftpPath, t_pMatch->localPath , pasv, get, reWrite);
		delete t_pMatch;
		t_pMatch = NULL;
	}
	thisFloorFiles.clear();

	Back();

	return rc;
}


BOOL CFTPclient::UploadAllFiles(string remotePath, string localPath, BOOL pasv, BOOL get, HANDLE hFind )
{
	string rf = remotePath;
	string lf = localPath;

	// 判断lf 是个路径
	if (((lf.rfind('\\') != lf.size() - 1) &&(lf.rfind(('/') != lf.size() - 1))))
	{
		// 单个文件
		return MoveFile(rf, lf, pasv, get);
	}
	BOOL rc =  TRUE;
	HANDLE t_hFind = INVALID_HANDLE_VALUE;
	// 递归访问所有文件
	WIN32_FIND_DATA FindFileData;
	//ASSERT(hFind != INVALID_HANDLE_VALUE);

	while (FindNextFile(hFind, &FindFileData) != 0) 
	{
		string t_lf = FindFileData.cFileName;
		if (t_lf == "..")
		{
			continue;
		}
		t_lf = lf;
		string t_rf = rf;
		t_lf += FindFileData.cFileName;
		t_rf += FindFileData.cFileName;
		t_hFind = hFind;
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			t_lf += "\\";
			t_rf += "\\";
			if (!CreateDir(t_rf))
			{
				return FALSE;
			}

			string str_tlf_all = t_lf + "*";
			t_hFind = FindFirstFile(str_tlf_all.c_str(), &FindFileData);
		}
		rc &= UploadAllFiles(t_rf, t_lf, pasv, get, t_hFind);
	}
	DWORD error = GetLastError();
	return rc;
}

BOOL CFTPclient::CreateDir(string dir, BOOL pwd)
{
	// 判断当前目录是否存在
	BOOL rc = CD(dir);

	if (rc == FALSE)
	{
		// 创建目录
		int iPos = dir.find_first_not_of(' ');//TrimLeft();
		dir.erase(0,iPos);

		iPos = dir.find_last_not_of(' ');//TrimRight();
		if (iPos < dir.size()-1)
		{
			dir.erase(iPos+1);
		}

		//xue-2013.08添加
		string remotedirUTF8=ws2utf8(s2ws(dir));

		string strMKDTag = "MKD ";
		string cmd = strMKDTag + remotedirUTF8;// + "\r\n";
		rc = FTPcommand(cmd);	

		if (rc == TRUE && pwd)
		{	
			rc = FTPcommand("PWD");	
		}
	}
	return rc;
}

BOOL CFTPclient::DeleteFtpDir(string rf, BOOL bDir)
{
	if (!bDir)
	{
		return DeleteFtpFile(rf); 
	}
	else
	{
		//xue-2013.08添加
		string remotedirUTF8=ws2utf8(s2ws(rf));

		string strCommand = "RMD ";
		strCommand += remotedirUTF8;
		if (FTPcommand(strCommand))
		{
			return TRUE;
		}
		else
		{
			// 切换到当前目录
			if (!CD(rf))
			{
				return FALSE;
			}

			BOOL rc = List(rf);

			int ndx = 0;
			BOOL dir;
			WORD ti;

			string strName, strDate, strSec, strSize;

			FileNamePool thisFloorFiles;
			_FtpLocalPathMatch* t_pMatch = NULL;
			while(GetLine(ndx))
			{	
				ParseLine(m_strLine, dir, ti, strSec, strDate,strSize, strName);

				t_pMatch =  new _FtpLocalPathMatch;
				t_pMatch->isDirectory = dir;
				t_pMatch->ftpPath = rf + strName;

				if (dir)
				{
					t_pMatch->ftpPath += "/";
				}

				thisFloorFiles.insert(make_pair(dir, t_pMatch));

			}

			for (FileNamePool::iterator iter = thisFloorFiles.begin(); iter != thisFloorFiles.end(); ++iter)
			{
				t_pMatch = iter->second;

				DeleteFtpDir(t_pMatch->ftpPath, t_pMatch->isDirectory);

				delete t_pMatch;
				t_pMatch = NULL;
			}

			thisFloorFiles.clear();
			Back();

			//xue-2013.08添加
			remotedirUTF8=ws2utf8(s2ws(rf));

			string strCommand_1 = "RMD ";
			strCommand_1 += remotedirUTF8;

			if (FTPcommand(strCommand_1))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
}

BOOL CFTPclient::DeleteLocalDir(string strLocalDir)
{
	//删除部署目录
	TCHAR path[300] = {0};

	int iLenth = strLocalDir.size();
	for (int i=0; i<iLenth-1; i++)//-1是为了去除路径最后的"\"
	{
		path[i] = strLocalDir.at(i);
	}

	SHFILEOPSTRUCT FileOp;   
	FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI ;   
	FileOp.hNameMappings = NULL;   
	FileOp.hwnd = NULL;   
	FileOp.lpszProgressTitle = NULL;   
	FileOp.pFrom = path;   
	FileOp.pTo = NULL;   
	FileOp.wFunc = FO_DELETE; 

	if (SHFileOperation(&FileOp) == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


//获得FTP上文件大小,尚未实现
long CFTPclient::GetFTPFileSize(string strPath)
{

	long lSize = 0;

	//遍历strpath字符串，如果路径中包含“\\”则将"\\"转换为"/"
	int i = strPath.size();

	if (i <= 0)
	{
		return 0;
	}

	i = i-1;
	for (; i >= 0; i --)
	{
		if (strPath[i] == '\\')
		{
			strPath[i] = '/';
		}

	}
	//将完整路径保存在strpath中
	string strPath_all = strPath;
	//分离出路径和文件名
	int s_pos = strPath.rfind('/');
	int sizestr = strPath.size();
	strPath = strPath.substr(0,sizestr-s_pos);

	int j = strPath_all.size();
	int size_name =j-i;
	string s_strFileName = ""; 
	string s_strpath = "";
	if(s_pos != 0)
	{
		int s_ilength = strPath_all.size();
		s_strFileName = strPath_all.substr(s_pos+1,size_name-1);//右侧字符
		s_strpath = strPath;
	}

	//切换到ftp中的路径
	if (!CD(strPath))
	{
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////


	//返回当前ftp路径中的文件列表
	if (!List(strPath_all))
	{
		return 0;
	}

	//FTPcommand("");
	int ndx = 0;

	BOOL dir;
	WORD ti;
	string strName, strDate, strSec, strSize;

	while(GetLine(ndx))
	{	
		ParseLine(m_strLine, dir, ti, strSec, strDate,strSize, strName);

		if (strName == s_strFileName)
		{
			lSize = atol(strSize.c_str());

		}
	}

	return lSize;

}

//获得FTP上文件夹大小，包括子目录及文件,尚未实现
long CFTPclient::GetFTPDirSize(string strDir)
{
	//目录不存在，返回0
	if (!CD(strDir))
	{
		return 0;
	}

	if (!List(strDir))
	{
		return 0;
	}

	int ndx = 0;
	BOOL dir = FALSE;
	WORD ti = 0;
	string strName = "";
	string strDate = "";
	string strSec = "";
	string strSize = "";

	FileInfoList thisFiles;
	FtpInfo* t_pMatch = NULL;
	while(GetLine(ndx))
	{	
		ParseLine(m_strLine, dir, ti, strSec, strDate,strSize, strName);
		t_pMatch =  new FtpInfo;

		t_pMatch->isDirectory = dir;
		t_pMatch->ftpItemName = strName;
		t_pMatch->lSize = atol(strSize.c_str());
		thisFiles.push_back(t_pMatch);

	}

	long rc = 0;
	int vsize = thisFiles.size();
	for (int i=0; i<vsize; ++i)
	{
		rc += GetDirSize(thisFiles[i],strDir);
	}

	for (int i=0; i<vsize; i++)
	{
		delete thisFiles[i];
		thisFiles[i] = NULL;
	}
	thisFiles.clear();

	return rc;
}

//获得本地文件大小,尚未实现
DWORD64 CFTPclient::GetLocalFileSize(string szPath)
{
	TCHAR szFileFilter[512];
	strcpy_s(szFileFilter,szPath.c_str());

	DWORD64 dwSize = 0;
	WIN32_FIND_DATA fileinfo;

	HANDLE hFind = NULL;
	hFind = FindFirstFile(szFileFilter,&fileinfo);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		dwSize = fileinfo.nFileSizeLow;

		FindClose(hFind);
	}

	return dwSize;
}

//获得本地文件夹大小，包括子目录及文件,尚未实现
DWORD64 CFTPclient::GetLocalFolderSize(string szPath, DWORD *dwFiles, DWORD *dwFolders)
{

	TCHAR szFileFilter[512];
	TCHAR szFilePath[512];
	HANDLE hFind = NULL;
	WIN32_FIND_DATA fileinfo;
	DWORD64    dwSize = 0;

	strcpy_s(szFilePath,szPath.c_str());
	//strcat(szFilePath,"\\");
	strcpy_s(szFileFilter,szFilePath);
	strcat_s(szFileFilter,"*.*");


	hFind = FindFirstFile(szFileFilter,&fileinfo);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	do
	{
		if(fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!strcmp(fileinfo.cFileName,".") || !strcmp(fileinfo.cFileName,".."))
			{
				//Do nothing for "." and ".." folders
			}
			else
			{
				TCHAR sztmp[512];
				strcpy_s(sztmp,szFilePath);
				strcat_s(sztmp,fileinfo.cFileName);
				strcat_s(sztmp,"\\");

				dwSize = dwSize + GetLocalFolderSize(sztmp,dwFiles,dwFolders);
				if(dwFolders != NULL)
				{
					++(*dwFolders);
				}
			}
		}
		else
		{
			if(dwFiles != NULL)
			{
				++(*dwFiles);
			}
		}


		dwSize += fileinfo.nFileSizeLow;

	}while(FindNextFile(hFind,&fileinfo));

	FindClose(hFind);
	return dwSize;
}


// 切换服务器的目录
BOOL CFTPclient::CD(string dir)
{
	int ndx=0;

	// 创建目录
	int iPos = dir.find_first_not_of(' ');//TrimLeft();
	dir.erase(0,iPos);

	iPos = dir.find_last_not_of(' ');//TrimRight();
	if (iPos < dir.size()-1)
	{
		dir.erase(iPos+1);
	}

	//xue-2013.08
	string remoteDirUTF8=ws2utf8(s2ws(dir));

	string cmd = "CWD " + remoteDirUTF8;// + "\r\n";
	BOOL rc = FALSE;

	rc = FTPcommand(cmd);

	if (rc == TRUE)
	{
		rc = FTPcommand("PWD");	// 显示当前路径
	}/*else{	// 不成功时,需不需要回退到上一级目录呢?  +		+		cmd	"CWD /first"	std::basic_string<char,std::char_traits<char>,std::allocator<char> >
	 cmd	"CWD /SysMC/WinAgent/WinAgent/0x0804.ini"	std::basic_string<char,std::char_traits<char>,std::allocator<char> >

	 }*/
	return rc;
}

// 回退上一级目录
BOOL CFTPclient::Back()
{
	int ndx=0;
	return FTPcommand("CDUP");
}

// 解析文件列表的一行(即当前m_strLine的内容)
BOOL CFTPclient::ParseLine(string strLine,
						   BOOL&	m_bDirectory,
						   WORD&	m_bSec,
						   string& m_strSec,
						   string& m_strDate,
						   string& m_size,
						   string& m_strName)
{
	if(strLine.empty())
		return FALSE;
	TCHAR ch = strLine.at(0);
	if(ch == 'd' || ch == 'D'){
		m_bDirectory = TRUE;
	}
	else
		if(ch == '-')
			m_bDirectory = FALSE;
		else{
			if(strLine.find("<DIR>") != -1 || strLine.find("<dir>") != -1)
				m_bDirectory = TRUE;
			else
				m_bDirectory = FALSE;
			if(strLine.size() < 40)
				return FALSE;

			//m_strNameCopy = strLineCopy.Mid(39);
			//m_strDateCopy = strLineCopy.Left(18);
			m_strName = strLine.substr(39,strLine.size()-39);
			m_strDate = strLine.substr(0,18);

			//m_strNameCopy.TrimLeft();
			int iPos = m_strName.find_first_not_of(' ');
			m_strName.erase(0,iPos);

			//m_strNameCopy.TrimRight();
			iPos = m_strName.find_last_not_of(' ');
			if (iPos < m_strName.size()-1)
			{
				m_strName.erase(iPos+1);
			}

			return TRUE;
		}
		m_bSec = 0;
		for(int i = 0; i < 9; i++)
		{
			ch = strLine.at(i);
			if(ch == '-')
				m_bSec |= 0x01;
			m_bSec <<= 1;
		}
		//m_strSecCopy = strLineCopy.Mid(1, 9);

		m_strSec = strLine.substr(1,9);

		// 解析文件大小
		int curPos= 0;

		m_size = TokenizeString(strLine,curPos);

		int t_spaceCount = 0;
		//CString t_str;
		string t_str;
		m_strDate = "";
		long nameStartPos = -1;
		while (curPos != -1)
		{
			t_str = TokenizeString(strLine,curPos);
			++t_spaceCount;

			if (t_spaceCount == 4)
			{
				m_size = t_str;
			}

			if (t_spaceCount >= 5 && t_spaceCount <= 6)
			{
				m_strDate += t_str; 
				m_strDate +=" ";
			}

			if (t_spaceCount == 7)
			{
				m_strDate += t_str;
				nameStartPos = curPos;
				break;
			}
			// if (t_spaceCount == 8)	{m_strNameCopy = t_str;}
		};

		m_strName = strLine.substr(nameStartPos+1,strLine.size() - nameStartPos-1);

		// end 解析文件大小
		return TRUE;
}


//下载单个文件
BOOL CFTPclient::DownLoadFile(string strRemoteFile, string strLocalFile,BOOL bPasv)
{
	locale loc = locale::global(locale(""));

	std::ofstream datafile;
	if (!CreateLocalFile(strLocalFile,datafile))
	{
		return FALSE;
	}

	if(!FTPcommand("TYPE I")) // 请求二进制传输
	{
		datafile.close();
		return FALSE; 
	}


	//创建监听套接字并发送PORT命令
	SOCKET sockSrvr;
	if (!ListenAndSendPort(sockSrvr))
	{
		datafile.close();
		::DeleteFile(strLocalFile.c_str()); 
		return FALSE;
	}

	//xue-2013.08
	string remoteDirUTF8=ws2utf8(s2ws(strRemoteFile));

	//发送下载文件的命令
	string strCmdTmp = "RETR ";
	strCmdTmp += remoteDirUTF8;
	if (!FTPcommand(strCmdTmp))
	{
		datafile.close();
		::DeleteFile(strLocalFile.c_str());

		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);

		return FALSE;
	}


	//接收文件并本地化
	if (!ReceiveFileData(sockSrvr,datafile))
	{
		datafile.close();
		::DeleteFile(strLocalFile.c_str());

		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);

		return FALSE;
	}


	//接收“226 Transfer OK”回复
	FTPcommand("");

	::shutdown(sockSrvr, 2);
	::closesocket(sockSrvr);

	datafile.close();

	return TRUE; // 传输成功    
}

//上传单个文件
BOOL CFTPclient::UpLoadFile(string strRemoteFile, string strLocalFile,BOOL bPasv)
{
	ifstream datafile;
	long LocalFileLen = 0;
	BOOL LocalFileExist = 0;

	locale loc = locale::global(locale(""));

	//打开本地文件，并获取文件长度
	if (!OpenLocalFile(strLocalFile,datafile,LocalFileLen))
	{
		return FALSE;
	}


	// 请求二进制传输
	if(!FTPcommand("TYPE I"))
	{
		datafile.close();

		return FALSE;     
	}


	//创建并初始化监听套接字，发送PORT命令
	SOCKET sockSrvr;//监听套接字

	if (!ListenAndSendPort(sockSrvr))
	{
		datafile.close();

		return FALSE;
	}


	//xue-2013.08
	string remoteDirUTF8=ws2utf8(s2ws(strRemoteFile));

	//发送STOR命令，上传文件
	string strCmdTmp = "STOR ";
	strCmdTmp += remoteDirUTF8;

	if(!FTPcommand(strCmdTmp))
	{
		datafile.close();

		return FALSE;     
	}


	//用socekt发送文件
	if (!SendFileData(sockSrvr,datafile))
	{
		datafile.close();

		return FALSE;     
	}


	//接收226 transfer ok回复
	if(!FTPcommand(""))
	{
		datafile.close();

		return FALSE;     
	}


	//释放资源
	datafile.close();

	::shutdown(sockSrvr, 2);
	::closesocket(sockSrvr);

	return TRUE; // 传输成功    
}

//将string中的大写字母转换为小写
string CFTPclient::MakeStringLower(string str)
{
	for (int i=0; i<str.size(); ++i)
	{
		if ((str[i] >= 65) && (str[i] <= 90))
		{
			str[i] += 32;
		}
	}

	return str;
}

string CFTPclient::TokenizeString(string strSourse,int& iCurPos)
{
	string strCopy = strSourse.substr(iCurPos,strSourse.size());

	//去掉前面多余的空格
	int iPos = strCopy.find_first_not_of(' ');
	if (iPos != -1)
	{
		strCopy.erase(0,iPos);
		iCurPos += iPos;
	}

	iPos = strCopy.find(" ");

	if (iPos != -1)
	{
		strCopy.erase(iPos,strCopy.size());
		iCurPos += iPos;
	}
	else
	{
		iCurPos = -1;
	}

	return strCopy;
}



string CFTPclient::ws2s(const wstring& ws)
{
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = WideCharToMultiByte(CP_ACP, 0, _Source, -1, NULL, 0, NULL, NULL);
	char *_Dest = new char[_Dsize];
	WideCharToMultiByte(CP_ACP, 0, _Source, -1, _Dest, (int)_Dsize, NULL, NULL);
	string result = _Dest;
	delete []_Dest;
	return result;
}

wstring CFTPclient::utf82ws(const string& s)
{
	const char* _Source = s.c_str();
	size_t _Dsize = MultiByteToWideChar(CP_UTF8, 0, _Source, -1, NULL, 0);
	wchar_t *_Dest = new wchar_t[_Dsize];
	MultiByteToWideChar(CP_UTF8, 0, _Source, -1, _Dest, (int)_Dsize);
	wstring result = _Dest;
	delete []_Dest;
	return result;
}

wstring CFTPclient::s2ws(const string& s)
{
	const char* _Source = s.c_str();
	size_t _Dsize = MultiByteToWideChar(CP_ACP, 0, _Source, -1, NULL, 0);
	wchar_t *_Dest = new wchar_t[_Dsize];
	MultiByteToWideChar(CP_ACP, 0, _Source, -1, _Dest, _Dsize);
	wstring result = _Dest;
	delete []_Dest;
	return result;
}
string CFTPclient::ws2utf8(const wstring& ws)
{
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = WideCharToMultiByte(CP_UTF8, 0, _Source, -1, NULL, 0, NULL, NULL);
	char *_Dest = new char[_Dsize];
	WideCharToMultiByte(CP_UTF8, 0, _Source, -1, _Dest, _Dsize, NULL, NULL);
	string result = _Dest;
	delete []_Dest;
	return result;
}

//模拟CByteArray
ByteArraySim::ByteArraySim()
{
	m_pBData = NULL;
	m_iSize = 0;
}

ByteArraySim::~ByteArraySim()
{
	delete []m_pBData;
	m_pBData = NULL;
	m_iSize = 0;
}

void ByteArraySim::SetSize(int nNewSize)
{
	int iMinSize = min(m_iSize,nNewSize);
	m_iSize = nNewSize;

	BYTE* pBData = new BYTE[m_iSize];
	memset(pBData,0,m_iSize);
	memcpy(pBData,m_pBData,iMinSize);

	delete []m_pBData;
	m_pBData = pBData;
	pBData = NULL;
}

int ByteArraySim::GetSize()
{
	return m_iSize;
}

BYTE* ByteArraySim::GetData()
{
	return m_pBData;
}

void ByteArraySim::RemoveAll()
{
	delete []m_pBData;
	m_pBData = NULL;
	m_iSize = 0;
}

BYTE ByteArraySim::GetAt(int nIndex)
{
	return *(m_pBData+nIndex);
}




//以下是递归调用函数，可以将GetFTPDirSize改造，暂时这样
long CFTPclient::GetDirSize(FtpInfo* pFileInfo,string strPath)
{
	if (!(pFileInfo->isDirectory))
	{
		return pFileInfo->lSize;
	}

	//目录不存在，返回0
	if (!CD(pFileInfo->ftpItemName))
	{
		return 0;
	}

	if (!List(strPath))
	{
		return 0;
	}


	int ndx = 0;
	string listName = "";
	BOOL dir;
	WORD ti;
	string strName, strDate, strSec, strSize;
	//StringUtil aStringUtl;
	FileInfoList thisFiles;
	FtpInfo* t_pMatch = NULL;
	while(GetLine(ndx))
	{	
		ParseLine(m_strLine, dir, ti, strSec, strDate,strSize, strName);

		t_pMatch =  new FtpInfo;
		t_pMatch->isDirectory = dir;
		t_pMatch->ftpItemName = strName;
		t_pMatch->lSize = atol(strSize.c_str());
		thisFiles.push_back(t_pMatch);

	}

	long rc = 0;
	int vsize = thisFiles.size();
	for (int i=0; i<vsize; ++i)
	{
		rc += GetDirSize(thisFiles[i],strPath);
	}

	for (int i=0; i<vsize; i++)
	{
		delete thisFiles[i];
		thisFiles[i] = NULL;
	}
	thisFiles.clear();

	Back();

	return rc;
}

//整理程序添加子函数

//打开本地文件
//输入：strLocalFile本地文件全路径
//输出：datafile文件流，LocalFileLen文件长度
bool CFTPclient::OpenLocalFile(string strLocalFile,ifstream& datafile,long& LocalFileLen)
{
	//打开本地文件，若是上传，打不开本地文件返回FASLE
	datafile.open(strLocalFile.c_str(), ios::in|ios::binary);

	if(datafile.is_open())
	{
		datafile.seekg(0, ios::end);
		LocalFileLen = datafile.tellg();

		datafile.seekg(0, ios::beg);
	}
	else
	{
		m_retmsg = "打开本地文件失败！";

		return false;
	}

	return true;
}



//功能：创建本地文件，下载文件时使用
//输入：strLocalFile本地文件全路径
//输出：datafile文件流
//返回：成功返回true
bool CFTPclient::CreateLocalFile(string strLocalFile,ofstream& datafile)
{
	locale loc = locale::global(locale(""));	
	datafile.open(strLocalFile.c_str(),ios::in|ios::out|ios::trunc|ios::binary);

	if(!(datafile.is_open()))
	{
		return false;
	}

	return true;
}


//功能：创建接收数据的套接字，并发送PORT命令到服务器端
//输出：sockSrvr返回接收连接的套接字
//返回：成功返回true
bool CFTPclient::ListenAndSendPort(SOCKET& sockSrvr)
{
	// 获取本地的ip地址，发送到服务器
	int nRet = 0;
	int addr_size = 0;
	struct sockaddr_in saSrc;


	//获取主机IP地址
	memset(&saSrc, 0, sizeof(sockaddr_in));
	addr_size = sizeof(saSrc);
	nRet = ::getsockname(m_Ctrlsok, (LPSOCKADDR)&saSrc, &addr_size);
	if(nRet == SOCKET_ERROR)
	{
		int aa = WSAGetLastError();
		return false;
	}

	string lhost = inet_ntoa(saSrc.sin_addr);
	for (int i=0; i<lhost.size(); ++i)
	{
		if (lhost[i] == '.')
		{
			lhost[i] = ',';
		}
	}


	//创建监听socket
	sockSrvr = socket(AF_INET, SOCK_STREAM, 0);
	if (sockSrvr == INVALID_SOCKET)    
	{
		return false;
	}

	//绑定socket
	memset(&saSrc, 0, sizeof(sockaddr_in));
	saSrc.sin_family = AF_INET;
	saSrc.sin_port = htons(0);//系统会分配一个随机端口
	nRet = ::bind(sockSrvr, (LPSOCKADDR)&saSrc, addr_size);

	if (nRet == SOCKET_ERROR)    
	{
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);

		return false;
	}

	//监听服务器端连接
	nRet = ::listen(sockSrvr, 2);
	if (nRet == SOCKET_ERROR)    
	{
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);

		return false;
	}

	//获取监听端口    
	memset(&saSrc, 0, sizeof(sockaddr_in));
	nRet = ::getsockname(sockSrvr, (LPSOCKADDR)&saSrc, &addr_size);
	if(nRet == SOCKET_ERROR)
	{
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);

		return false;
	}

	u_short localsock = 0;
	localsock = ntohs(saSrc.sin_port);

	char cPort[10] = {0};
	sprintf_s(cPort,",%d,%d",localsock / 256, localsock % 256);//lhost.Format(lhost+",%d,%d", localsock / 256, localsock % 256);
	lhost += cPort;


	//发送端口到ftp服务器
	string strCmdTmp = "PORT ";
	strCmdTmp += lhost;

	if(!FTPcommand(strCmdTmp))
	{
		::shutdown(sockSrvr, 2);
		::closesocket(sockSrvr);  

		return false;
	}

	return true;
}

//功能：用socket上传文件
//输入：sockSrvr监听套接字，datafile文件流
//返回：成功返回true
bool CFTPclient::SendFileData(SOCKET& sockSrvr,ifstream& datafile)
{
	SOCKET datachannel;
	int addr_size = sizeof(sockaddr_in);

	struct sockaddr_in saDest;
	memset(&saDest, 0, sizeof(sockaddr_in));


	//接收连接
	datachannel = ::accept(sockSrvr, (LPSOCKADDR)&saDest, &addr_size);
	if(datachannel == INVALID_SOCKET)
	{   
		return false;
	}

	//设置套接字为阻塞模式
	DWORD lpArgument = 0;
	if((0 != ioctlsocket(datachannel, FIONBIO, &lpArgument))) 
	{   
		::shutdown(datachannel, 2);
		::closesocket(datachannel);

		return false;
	}   

	// 连接成功，然后进行阻塞式数据传输
	datafile.seekg(0, ios::beg); 

	int numread = 0;
	int numsent = 0;
	const int BUFSIZE = 4096;
	char cbuf[BUFSIZE] = {0};
	while(1) 
	{
		datafile.read(cbuf,BUFSIZE);
		int numread = datafile.gcount();

		if((numsent=::send(datachannel, cbuf, numread, 0))==SOCKET_ERROR)
		{
			break;
		}

		// 如果发送出去的字节少于从文件读取的字节，则调整发送指针，以使得数据发送正确
		if(numread!=numsent)
		{
			datafile.seekg(numsent-numread,ios::cur);
		}

		if (datafile.eof())
		{
			break;
		}
	}

	::shutdown(datachannel, 2);
	::closesocket(datachannel);

	return true;
}


//功能：接收文件到本地，下载文件
//输入：sockSrvr监听套接字，datafile文件流
//返回：成功返回true
bool CFTPclient::ReceiveFileData(SOCKET& sockSrvr,ofstream& datafile)
{
	struct sockaddr_in saDest; 
	int addr_size = sizeof(saDest);
	memset(&saDest, 0, sizeof(sockaddr_in));

	SOCKET datachannel;
	datachannel = ::accept(sockSrvr, (LPSOCKADDR)&saDest, &addr_size);
	if(datachannel == INVALID_SOCKET)
	{
		return false;
	}   


	DWORD lpArgument = 0;
	if((0 != ioctlsocket (datachannel, FIONBIO, &lpArgument))) 
	{
		::shutdown(datachannel, 2);
		::closesocket(datachannel); 

		return false;
	}   

	// 连接成功，然后进行阻塞式数据传输
	// 开始传输数据
	int num = 0;
	const int BUFSIZE = 4096;
	char cbuf[BUFSIZE] = {0};

	while(1) 
	{
		if(!(num = ::recv(datachannel, cbuf, BUFSIZE, 0)) || num == SOCKET_ERROR)
		{
			break;
		}
		else
		{
			datafile.write(cbuf,num);
		}
	}

	::shutdown(datachannel, 2);
	::closesocket(datachannel);

	return true;
}


//功能：发送LIST命令，并接收回复信息，回复信息放在m_buf中
//输入：strHostDir远程Ftp目录，sockSrvr监听套接字
//返回：成功返回true
bool CFTPclient::SendListAndRecvEcho(string strHostDir,SOCKET& sockSrvr)
{
	//xue-2013.08
	string remoteDirUTF8=ws2utf8(s2ws(strHostDir));

	//发送列表命令
	string strOut = "";
	strOut += "LIST";

	if (strHostDir != "")
	{
		strOut += " ";
		strOut += remoteDirUTF8;  
	}

	if(!WriteStr(strOut))
	{
		return false;
	}

	if(!ReadStr())
	{
		return false;
	}


	//返回错误信息
	if(m_fc==4 || m_fc==5)
	{
		return false;
	}

	SOCKET datachannel;   //数据传输socket
	struct sockaddr_in saDest;
	int addr_size = sizeof(sockaddr_in); 

	memset(&saDest, 0, sizeof(sockaddr_in));
	datachannel = ::accept(sockSrvr, (LPSOCKADDR)&saDest, &addr_size);
	if(datachannel == INVALID_SOCKET)
	{
		return false;
	}

	DWORD lpArgument=0;
	if((0 != ioctlsocket (datachannel, FIONBIO, &lpArgument))) 
	{
		::shutdown(datachannel, 2);
		::closesocket(datachannel);
		return false;
	}

	const int BUFSIZE = 4096;
	m_buf.RemoveAll();
	m_buf.SetSize(BUFSIZE);

	int num = 0;
	int sum = 0;
	while(1) 
	{ 
		// 获得数据    
		if(!(num = ::recv(datachannel, (char *)(m_buf.GetData()+sum), BUFSIZE, 0))    
			|| num == SOCKET_ERROR)
		{
			break;
		}

		sum += num;
		m_buf.SetSize(sum + BUFSIZE);
	}

	if(!FTPcommand(""))
	{
		return false;
	}

	::shutdown(datachannel, 2);
	::closesocket(datachannel);

	return true;
}