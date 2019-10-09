#pragma once
#include "farchive.h"

namespace ECore
{
	/**	\class FArchiveAdv 
		\brief ������ȡ���ڲ��ļ�ָ��Ľӿ�
		\remark ��Ҫ����Ϊ�ⲿ���������Ҫ���ļ�����Seek����
	*/
	class FArchiveAdv :	public FArchive
	{
	public:
		FArchiveAdv(void)			{}
		virtual ~FArchiveAdv(void)	{}

		FILE* GetWriteFile() const	{	return FArchive::m_pWriteFile;}
		HANDLE GetReadFile() const	{	return FArchive::m_hReadFile;}
		VFS* GetReadFS() const	{	return FArchive::m_pFS;}
	};
}//namespace ECore