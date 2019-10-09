
#include "rtti_obj.h"
#include "rtti_info.h"
#include "dyna_array.h"
#include "simple_vector.h"
#pragma once
#include "Math.h"
#include "Color.h"
#include "vfs.h"

#pragma warning(push)
#pragma warning(disable: 4267)

namespace ECore
{
	/**	\class FArchive
		\brief ���л���ʹ�õ�Ŀ���ĵ�
		\remarks ���л��ʺ����ڴ����ӵĶ���,�������νӿڴ洢�Ķ���
	*/
	class ECORE_API FArchive
	{
	public:
		FArchive(void);
		virtual ~FArchive(void);

		bool OpenForWrite(LPCSTR szFileName);
		bool OpenForRead(VFS* pFS,LPCSTR szFileName,bool bThrow=true);
		void Close();

		template<typename ValType>
		FArchive& operator<< (const ValType& val)
		{
			ASSERT(m_pWriteFile!=NULL);
			if(m_pWriteFile)
				fwrite(&val,sizeof(ValType),1,m_pWriteFile);
			return *this;
		}

		template<typename ValType>
		FArchive& operator>>(ValType& val)
		{
			ASSERT(m_pFS!=NULL && m_hReadFile!=NULL);
			if(m_pFS)
				m_pFS->Read(m_hReadFile,&val,sizeof(ValType));
			return *this;
		}

		//--string
		FArchive& operator<<(wstring& szStr)
		{
			return *this<<szStr.c_str();
		}

		FArchive& operator>>(wstring& szStr)
		{
			WCHAR szTemp[HUGE_STRING];
			unsigned int len=0;
			*this>>len;
			if( len == 0 )
			{
				szStr.clear();
				return *this;
			}

			ASSERT(len<HUGE_STRING);
			//TCHAR* szTmp=(TCHAR*)g_pSafeMemCache->Alloc(len*sizeof(TCHAR));
			m_pFS->Read(m_hReadFile,szTemp,len*sizeof(WCHAR));
			szStr.assign(szTemp, len-1);
			//g_pSafeMemCache->Free(szTmp);
			return *this;
		}

		FArchive& operator<<(string& szStr)
		{
			return *this<<szStr.c_str();
		}

		FArchive& operator>>(string& szStr)
		{
			CHAR szTemp[HUGE_STRING];
			unsigned int len=0;
			*this>>len;
			if( len == 0 )
			{
				szStr.clear();
				return *this;
			}

			ASSERT(len<HUGE_STRING);
			m_pFS->Read(m_hReadFile,szTemp,len*sizeof(CHAR));
			szStr.assign(szTemp, len-1);
			return *this;
		}

		FArchive& operator<<(const WCHAR* szStr)
		{
			unsigned int len = wcslen(szStr) + 1;
			*this << len;
			fwrite(szStr,len*sizeof(WCHAR),1,m_pWriteFile);	//д���ַ����ͽ�β��0
			return *this;
		}

		FArchive& operator>>(WCHAR* szStr)
		{
			unsigned int len=0;
			*this>>len;
			if( len == 0 )
			{
				szStr[0] = 0;
				return *this;
			}

			m_pFS->Read(m_hReadFile, szStr, len*sizeof(WCHAR));
			return *this;
		}

		FArchive& operator<<(CONST CHAR* szStr)
		{
			unsigned int len = strlen(szStr) + 1;
			*this << len;
			fwrite(szStr,len*sizeof(CHAR),1,m_pWriteFile);	//д���ַ����ͽ�β��0
			return *this;
		}

		FArchive& operator>>(CHAR* szStr)
		{
			unsigned int len=0;
			*this>>len;
			if( 0 == len )
			{
				szStr[0] = 0;
				return *this;
			}

			m_pFS->Read(m_hReadFile, szStr, len*sizeof(CHAR));
			return *this;
		}


	public:
		void BufferWrite(const void *pBuf,DWORD size)
		{
			ASSERT(m_pWriteFile!=NULL);
			if(m_pWriteFile)
				fwrite(pBuf,size,1,m_pWriteFile);
		}
		void BufferRead(void *pBuf,DWORD size)
		{
			ASSERT(m_pFS!=NULL && m_hReadFile!=NULL);
			if(m_pFS)
				m_pFS->Read(m_hReadFile,pBuf,size);
		}
		
	protected:
		int			m_mode;	//-1��Ч,0ΪOpenFroRead(),1ΪOpenForWrite
		FILE*		m_pWriteFile;	//д��ʱʹ��stdio file system
		VFS*		m_pFS;
		HANDLE		m_hReadFile;	//����ʱʹ�������ļ�ϵͳ
	};

	/**	\class ISerializable
		\brief ���л��ӿ�
		\remarks ISerializable��һ�����ӿ�class,�ⲿclass���Զ�̳�
	*/
	class ECORE_API ISerializable
	{
	public:
		ISerializable()	{}
		virtual ~ISerializable()	{}

		virtual void Serialize(FArchive& ar) =0;
		virtual void Deserialize(FArchive& ar) =0;
	};

	/**	��һ��������ISerializable���������"ָ��"��vector���л�
		\remarks Ϊ��֧�������л�,object�����ǿɶ�̬������RTTIObj
	*/
	template<typename SerType>
		void SerialObjPtrVector(FArchive& ar,vector<SerType*>& objPtrVector)
	{
		unsigned int size=objPtrVector.size();
		ar<<size;
		if(size==0)//û���κ�Ԫ��
			return;
		//--
		for(size_t i=0; i<objPtrVector.size(); ++i)
		{
			SerType* obj=objPtrVector[i];
			//--����class name
			const RTTInfo *pClass=obj->GetRTTInfo();
			ar<<pClass->m_dwClassID;
			ASSERT(pClass->m_pfnCreateObj!=NULL);

			obj->Serialize(ar);
		}
	}
	template<typename SerType>
		void DeserialObjPtrVector(FArchive& ar,vector<SerType*>& objPtrVector)
	{
		DWORD dwClassID = 0;
		unsigned int size=0;
		ar>>size;
		if(size==0)
			return;
		//--
		ASSERT(objPtrVector.size()==0);//���治�����κ�Ԫ��
		objPtrVector.resize(size);
		for(size_t i=0; i<objPtrVector.size(); ++i)
		{
			ar>>dwClassID;
			SerType* obj=(SerType*)RTTFactory::Inst()->CreateObj(dwClassID);
			obj->Deserialize(ar);
			objPtrVector[i]=obj;
		}
	}

	/**	��һ��������ISerializable���������"ָ��"��Vector���л�
		\remarks Ϊ��֧�������л�,object�����ǿɶ�̬������RTTIObj
	*/
	template<typename SerType>
	void SerialObjPtrVector(FArchive& ar,SimpleVector<SerType*>& objPtrArray)
	{
		unsigned int size=objPtrArray.size();
		ar<<size;
		if(size==0)//û���κ�Ԫ��
			return;
		//--
		for(size_t i=0; i<objPtrArray.size(); ++i)
		{
			SerType* obj=objPtrArray[i];
			//--����class name
			const RTTInfo *pClass=obj->GetRTTInfo();
			ar<<pClass->m_dwClassID;
			ASSERT(pClass->m_pfnCreateObj!=NULL);

			obj->Serialize(ar);
		}
	}
	template<typename SerType>
	void DeserialObjPtrVector(FArchive& ar,SimpleVector<SerType*>& objPtrArray)
	{
		DWORD dwClassID = 0;
		unsigned int size=0;
		ar>>size;
		if(size==0)
			return;
		//--
		ASSERT(objPtrArray.size()==0);//���治�����κ�Ԫ��
		objPtrArray.resize(size);
		for(size_t i=0; i<objPtrArray.size(); ++i)
		{
			ar>>dwClassID;
			SerType* obj=(SerType*)RTTFactory::Inst()->CreateObj(dwClassID);
			obj->Deserialize(ar);
			objPtrArray[i]=obj;
		}
	}


	/**	��һ������ISerializable����������vector���л�
	*/
	template<typename SerType>
		void SerialObjVector(FArchive& ar,vector<SerType>& objVector)
	{
		unsigned int size=objVector.size();
		ar<<size;
		if(size==0)
			return;
		for(size_t i=0; i<objVector.size(); ++i)
		{
			SerType& obj=objVector[i];
			obj.Serialize(ar);
		}
	}
	template<typename SerType>
		void DeserialObjVector(FArchive& ar,vector<SerType>& objVector)
	{
		objVector.clear();
		//--
		unsigned int size=0;
		ar>>size;
		if(size==0)
			return;
		objVector.resize(size);
		for(size_t i=0; i<objVector.size(); ++i)
		{
			SerType& obj=objVector[i];
			obj.Deserialize(ar);
		}
	}

	//--��һ��������(int,float��)��vector�������л�
	template<typename ValType>
		void SerialValVector(FArchive& ar,vector<ValType>& valVector)
	{
		unsigned int size=valVector.size();
		ar<<size;
		if(size==0)
			return;
		ar.BufferWrite(&valVector[0],size*sizeof(ValType));
	}
	template<typename ValType>
		void DeserialValVector(FArchive& ar,vector<ValType>& valVector)
	{
		valVector.clear();
		//--
		unsigned int size=0;
		ar>>size;
		if(size==0)
			return;
		valVector.resize(size);
		ar.BufferRead(&valVector[0],size*sizeof(ValType));
	}

	//--��һ��������(int,float��)��DynArray�������л�
	template<typename ValType>
		void SerialValArray(FArchive& ar,DynaArray<ValType>& valVector)
	{
		unsigned int size=valVector.size();
		ar<<size;
		if(size==0)
			return;
		ar.BufferWrite(&valVector[0],size*sizeof(ValType));
	}
	template<typename ValType>
		void DeserialValArray(FArchive& ar,DynaArray<ValType>& valVector)
	{
		valVector.clear();
		//--
		unsigned int size=0;
		ar>>size;
		if(size==0)
			return;
		valVector.resize(size);
		ar.BufferRead(&valVector[0],size*sizeof(ValType));
	}

	//--��һ��������(int,float��)��SimpleVector�������л�
	template<typename ValType>
	void SerialValVector(FArchive& ar,SimpleVector<ValType>& valVector)
	{
		unsigned int size=valVector.size();
		ar<<size;
		if(size==0)
			return;
		ar.BufferWrite(&valVector[0],size*sizeof(ValType));
	}
	template<typename ValType>
	void DeserialValVector(FArchive& ar,SimpleVector<ValType>& valVector)
	{
		valVector.clear();
		//--
		unsigned int size=0;
		ar>>size;
		if(size==0)
			return;
		valVector.resize(size);
		ar.BufferRead(&valVector[0],size*sizeof(ValType));
	}
}//namespace ECore

#pragma warning(pop)