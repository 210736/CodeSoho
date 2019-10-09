#include "StdAfx.h"
#include ".\farchive.h"

namespace ECore
{
	FArchive::FArchive(void)
	{
		m_mode=-1;
		m_pWriteFile=NULL;
		m_pFS=NULL;
		m_hReadFile=NULL;
	}

	FArchive::~FArchive(void)
	{
		Close();
	}

	bool FArchive::OpenForWrite(LPCSTR szFileName)
	{
		Close();
		//--防止外部调用SetCurrentDirector改变工作路径,每次打开文件都设置一次
		//注意:MFC的CFileDialog会自动改变系统的当前路径.
		SetDefaultDir();

		//--
		m_pWriteFile=NULL;
		if(fopen_s(&m_pWriteFile,szFileName,"wb")!=0)
		{
			ERR(_T("Archive open for write failed."),SS::To16(szFileName));
			return false;
		}

		m_mode=1;
		return true;
	}

	bool FArchive::OpenForRead(VFS* pFS,LPCSTR szFileName,bool bThrow)
	{
		Close();
		ASSERT(pFS!=NULL);
		
		m_hReadFile=pFS->Open(szFileName);
		m_pFS=pFS;

		if(!VALID(m_hReadFile))
		{
			if(bThrow)
				THROW(_T("Archive open for read failed:%s."),SS::To16(szFileName));
			return false;
		}

		m_mode=0;
		return true;
	}

	void FArchive::Close()
	{
		switch(m_mode)
		{
		case 0:
			if(m_pFS!=NULL && m_hReadFile!=NULL)
				m_pFS->Close(m_hReadFile);
			m_pFS=NULL;
			m_hReadFile=NULL;
			break;
		case 1:
			if(m_pWriteFile!=NULL)
				fclose(m_pWriteFile);
			m_pWriteFile=NULL;
		}//endof switch
	}
}//namespace ECore