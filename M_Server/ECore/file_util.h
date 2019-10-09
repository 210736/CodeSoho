#pragma once

#include "dyna_array.h"
#include "simple_vector.h"

#pragma warning(push)
#pragma warning(disable: 4267)
namespace ECore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 字符串的读写

	INLINE void FWriteString(FILE *fp,const WCHAR *str)
	{
		unsigned int strLen=wcslen(str)+1;
		fwrite(&strLen,sizeof(unsigned int),1,fp);
		fwrite(str,strLen*sizeof(WCHAR),1,fp);
	}

	INLINE void FWriteString(FILE *fp,LPCSTR str)
	{
		unsigned int strLen=strlen(str)+1;
		fwrite(&strLen,sizeof(unsigned int),1,fp);
		fwrite(str,strLen*sizeof(CHAR),1,fp);
	}
	
	INLINE void FReadString(VFS *pFS,HANDLE hFile,wstring& str)
	{
		WCHAR szTemp[HUGE_STRING];
		unsigned int strLen;
		pFS->Read(hFile,&strLen,sizeof(unsigned int));
		if(strLen > 0)
		{
			ASSERT(strLen < HUGE_STRING);
			pFS->Read(hFile,szTemp,strLen*sizeof(WCHAR));
			str.assign(szTemp, strLen-1);
		}
		else
		{
			str.clear();
		}
	}

	INLINE void FReadString(VFS *pFS,HANDLE hFile,LPWSTR szStr)
	{
		unsigned int strLen;
		pFS->Read(hFile,&strLen,sizeof(unsigned int));
		if(strLen > 0)
		{
			ASSERT(strLen < LONG_STRING);
			pFS->Read(hFile,szStr,strLen*sizeof(WCHAR));
		}
		else
		{
			szStr[0] = 0;
		}
	}

	INLINE void FReadString(VFS *pFS,HANDLE hFile,string& str)
	{
		CHAR szTemp[HUGE_STRING];
		unsigned int strLen;
		pFS->Read(hFile,&strLen,sizeof(unsigned int));
		if(strLen > 0)
		{
			ASSERT(strLen < HUGE_STRING);
			pFS->Read(hFile,szTemp,strLen*sizeof(CHAR));
			str.assign(szTemp, strLen-1);
		}
		else
		{
			str.clear();
		}
	}

	INLINE void FReadString(VFS *pFS,HANDLE hFile,LPSTR szStr)
	{
		unsigned int strLen;
		pFS->Read(hFile,&strLen,sizeof(unsigned int));
		if(strLen > 0)
		{
			ASSERT(strLen < LONG_STRING);
			pFS->Read(hFile,szStr,strLen*sizeof(CHAR));
		}
		else
		{
			szStr[0] = 0;
		}
	}
	

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 简单数据类型的读写


	template<typename ValType>
	void FReadValue(FILE *fp,ValType& val)
	{
		fread(&val,sizeof(ValType),1,fp);
	}

	template<typename ValType>
		void FWriteValue(FILE *fp,ValType& val)
	{
		fwrite(&val,sizeof(ValType),1,fp);
	}

	template<typename ValType>
		void FReadValue(VFS *pFS,HANDLE hFile,ValType& val)
	{
		pFS->Read(hFile,&val,sizeof(ValType));
	}
	

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 简单数据类型的vector的读写


	template<typename ValType>
		void FWriteValVector(FILE *fp,const vector<ValType>& valVector)
	{
		unsigned int size=(unsigned int)valVector.size();
		fwrite(&size,sizeof(size),1,fp);
		if(size==0)
			return;
		fwrite(&valVector[0],sizeof(ValType),size,fp);
	}

	template<typename ValType>
		void FReadValVector(VFS *pFS,HANDLE hFile,vector<ValType>& valVector)
	{
		//--
		unsigned int size=0;
		pFS->Read(hFile,&size,sizeof(size));
		if(0 == size)
		{
			valVector.clear();
			return;
		}

		valVector.resize(size);
		pFS->Read(hFile,&valVector[0],sizeof(ValType)*size);
	}

	template<typename ValType>
	void FWriteValArray(FILE *fp,const DynaArray<ValType>& valArray)
	{
		unsigned int size=(unsigned int)valArray.size();
		fwrite(&size,sizeof(size),1,fp);
		if(size==0)
			return;
		fwrite(&valArray[0],sizeof(ValType),size,fp);
	}

	template<typename ValType>
	void FReadValArray(VFS *pFS,HANDLE hFile,DynaArray<ValType>& valArray)
	{
		unsigned int size=0;
		pFS->Read(hFile,&size,sizeof(size));
		if(0 == size)
		{
			valArray.clear();
			return;
		}

		valArray.resize(size);
		pFS->Read(hFile,&valArray[0],sizeof(ValType)*size);
	}

	template<typename ValType>
	void FWriteValArray(FILE *fp,const SimpleVector<ValType>& valArray)
	{
		unsigned int size=(unsigned int)valArray.size();
		fwrite(&size,sizeof(size),1,fp);
		if(size==0)
			return;
		fwrite(&valArray[0],sizeof(ValType),size,fp);
	}

	template<typename ValType>
	void FReadValArray(VFS *pFS,HANDLE hFile,SimpleVector<ValType>& valArray)
	{
		unsigned int size=0;
		pFS->Read(hFile,&size,sizeof(size));
		if(0 == size)
		{
			valArray.clear();
			return;
		}

		valArray.resize(size);
		pFS->Read(hFile,&valArray[0],sizeof(ValType)*size);
	}

	template<typename ValType>
	void FReadValArray(FILE *fp,SimpleVector<ValType>& valArray)
	{
		unsigned int size=0;
		fread(&size,sizeof(size),1,fp);
		if(0 == size)
		{
			valArray.clear();
			return;
		}

		valArray.resize(size);
		fread(&valArray[0],sizeof(ValType)*size,1,fp);
	}


}//namespace ECore

#pragma warning(pop)