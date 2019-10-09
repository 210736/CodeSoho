//-----------------------------------------------------------------------------
// Time: 2003-1-2
// Auth: Lyp
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{
const long EPK_LABEL		= 0x1A545352L;
const long EPK_MAGIC		= EPK_LABEL;
const long EPK_VERSION		= 0x20110224L;
const long EPK_VERSION_NEW	= 0x20110425L;

//-----------------------------------------------------------------------------
// base struct
//-----------------------------------------------------------------------------
#pragma pack(push, 4)

// EPK�ļ�ͷ�ṹ
struct tagEPKHeader
{
	DWORD	dwLable;		// ���ͱ�־
	DWORD	dwVersion;		// �汾
	DWORD	dwTableStart;	// �ļ�����ʼ�ֽ�
	DWORD	dwDataStart;	// ���ݿ���ʼ��ַ
	DWORD	dwValidTableNum;// ��ЧTable�������������Ч�ļ�����Ƭ
	DWORD	dwMaxTableNum;	// ���Table�����
	DWORD	dwPackageSize;	// ��ǰ����С
	DWORD	dwFileNum;		// ��ǰ�ļ�����	
	DWORD	dwReserved[8];	
}; // 16 DWORD


// EPK�ļ���ṹ
struct tagEPKTable
{
	DWORD		dwCRC;				// CRC����
	DWORD		dwFatherCRC;		// ��Ŀ¼CRC
	DWORD		dwStartPos;			// ��ʼλ��
	DWORD		dwPackedSize;		// ѹ���󳤶�
	DWORD		dwOriginSize;		// ԭ�ļ�����
	WORD		wFlag;				// ��־
	WORD		wNameSize;			// �ļ�������
	FILETIME	LastWriteTime;		// ���д��ʱ��
	BYTE		MD5[16];			// MD5
};


#define GET_FILTER(f)			((EFilter)(((f) & 0xff00) >> 8))
#define IS_COMPRESSED(f)		(GET_FILTER(f) != EF_Changeless)
#define IS_FILE_REMOVED(f)		((f) &  0x0010)
#define IS_FILE_OR_DIR_VALID(f) (((f) & 0x0001) && !IS_FILE_REMOVED(f))


#pragma pack(pop)

} // namespace ECore{