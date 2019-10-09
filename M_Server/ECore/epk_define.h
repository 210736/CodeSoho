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

// EPK文件头结构
struct tagEPKHeader
{
	DWORD	dwLable;		// 类型标志
	DWORD	dwVersion;		// 版本
	DWORD	dwTableStart;	// 文件表起始字节
	DWORD	dwDataStart;	// 数据块起始地址
	DWORD	dwValidTableNum;// 有效Table项个数，包括有效文件和碎片
	DWORD	dwMaxTableNum;	// 最大Table项个数
	DWORD	dwPackageSize;	// 当前包大小
	DWORD	dwFileNum;		// 当前文件个数	
	DWORD	dwReserved[8];	
}; // 16 DWORD


// EPK文件表结构
struct tagEPKTable
{
	DWORD		dwCRC;				// CRC较验
	DWORD		dwFatherCRC;		// 父目录CRC
	DWORD		dwStartPos;			// 开始位置
	DWORD		dwPackedSize;		// 压缩后长度
	DWORD		dwOriginSize;		// 原文件长度
	WORD		wFlag;				// 标志
	WORD		wNameSize;			// 文件名长度
	FILETIME	LastWriteTime;		// 最后写入时间
	BYTE		MD5[16];			// MD5
};


#define GET_FILTER(f)			((EFilter)(((f) & 0xff00) >> 8))
#define IS_COMPRESSED(f)		(GET_FILTER(f) != EF_Changeless)
#define IS_FILE_REMOVED(f)		((f) &  0x0010)
#define IS_FILE_OR_DIR_VALID(f) (((f) & 0x0001) && !IS_FILE_REMOVED(f))


#pragma pack(pop)

} // namespace ECore{