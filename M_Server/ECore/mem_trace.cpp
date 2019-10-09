//-----------------------------------------------------------------------------
// author Lyp
// date 2003-06-17
// last 2010-06-10
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "mem_trace.h"

#ifdef MEM_TRACE


#undef new
#undef malloc
#undef calloc
#undef realloc
#undef free

#pragma warning(disable:4311)
#pragma warning(disable:4313)
#pragma warning(disable:4073)
#pragma init_seg(lib)

ECore::MemTrace g_MemMap;	// 全局对象，以便于程序结束后析构

//-----------------------------------------------------------------------------
//! AllocateMemory
//-----------------------------------------------------------------------------
LPVOID AllocateMemory(size_t uiSize, LPCSTR szFile, INT nLine, BOOL bArray, LPVOID pAddress)
{
	if( pAddress )
		DeallocateMemory(pAddress, FALSE);

	return g_MemMap.Alloc(szFile, nLine, uiSize, bArray);
}


//-----------------------------------------------------------------------------
//! ReAllocateMemory
//-----------------------------------------------------------------------------
LPVOID ReAllocateMemory(size_t uiSize, LPCSTR szFile, INT nLine, BOOL bArray, LPVOID pAddress)
{
	return g_MemMap.ReAlloc(szFile, nLine, uiSize, bArray, pAddress);
}


//-----------------------------------------------------------------------------
//! DeallocateMemory
//-----------------------------------------------------------------------------
VOID DeallocateMemory(LPVOID pAddress, BOOL bArray)
{
	if( 0 == pAddress )
		return;  // ANSI states that delete will allow NULL pointers.

	g_MemMap.Free(pAddress, bArray);
}


//-----------------------------------------------------------------------------
// 为分配出来的内存设置调试信息，以便查找
//-----------------------------------------------------------------------------
BOOL SetMemoryTraceDesc(LPVOID pAddress, LPCSTR szDesc)
{
	if( 0 == pAddress )
		return FALSE;

	return g_MemMap.SetDesc(pAddress, szDesc);
}


VOID WriteMem()
{
	g_MemMap.Log();

}

namespace ECore {
//-----------------------------------------------------------------------------
//! construction
//-----------------------------------------------------------------------------
MemTrace::MemTrace()
{
	m_dwBreakAddress = INVALID;
	m_bTerminate = FALSE;
}

//-----------------------------------------------------------------------------
//! destruction
//-----------------------------------------------------------------------------
MemTrace::~MemTrace()
{
	m_bTerminate = TRUE;
	Log();	// 程序结束时写出log
}


//-----------------------------------------------------------------------------
// InsertMemNode
//-----------------------------------------------------------------------------
LPVOID MemTrace::Alloc(LPCSTR szFile, INT nLine, size_t uiSize, BOOL bArray)
{
	LPVOID pAddress = NULL;
	size_t uiNewSize = uiSize + sizeof(DWORD) * 2;	// 头尾防溢出标志
	
	while( true )	// 尝试分配内存
	{
		pAddress = malloc(uiNewSize);
		if( pAddress )	// 分配成功
		{
			if( m_bTerminate )
				return pAddress;
			break;
		}

		// 分配不成功，找出当前出错处理函数
		new_handler fGlobalHandler = set_new_handler(0);
		set_new_handler(fGlobalHandler);

		if( fGlobalHandler )
			(*fGlobalHandler)();	// 调用出错处理
		else 
			throw std::bad_alloc();
	}

	// 分配成功后写入首尾防溢出符号
	*(DWORD*)pAddress = 0xDeadBeef;
	*(DWORD*)((BYTE*)pAddress + uiSize + sizeof(DWORD)) = 0xDeadBeef;

	// 将指针后移到外部能用的地址
	pAddress = (BYTE*)pAddress + sizeof(DWORD);
	
	tagNode* pNode = (tagNode*)malloc(sizeof(tagNode));
	pNode->pAddress = pAddress;
	pNode->szFile = szFile;
	pNode->nLine = nLine;
	pNode->uiSize = uiSize;
	pNode->bArray = (bArray == TRUE);
	pNode->bArrayError = false;
	pNode->bBoundError = false;
	pNode->szDesc[0] = 0;

	ASSERT( !m_mapMem.IsExist(pAddress) );
	m_mapMem.Add(pAddress, pNode);


	if( VALID(m_dwBreakAddress) && ((DWORD)pAddress) == m_dwBreakAddress )
	{
		__asm int 3;	// 断点
	}

	return pAddress;
}


//-----------------------------------------------------------------------------
// RemoveMemNode
//-----------------------------------------------------------------------------
VOID MemTrace::Free(LPVOID pAddress, BOOL bArray)
{
	if( m_bTerminate )	// 防止对象析构后被继续调用
		return;

	tagNode* pNode = m_mapMem.Peek(pAddress);
	if( VALID(pNode) )
	{
		m_mapMem.Erase(pAddress);
		pAddress = (BYTE*)pAddress - sizeof(DWORD);	// 得到实际地址

		BOOL bStartChanged = (*(DWORD*)pAddress != 0xDeadBeef);
		BOOL bEndChanged = (*(DWORD*)((BYTE*)pAddress + pNode->uiSize + sizeof(DWORD)) != 0xDeadBeef);
		BOOL bBig = pNode->uiSize > 1 * 1024 * 1024;

		if( bStartChanged || bEndChanged )	// 溢出检测
		{
			ASSERT(0);
			pNode->bBoundError = true;
			m_listBoundErrorNode.PushBack(pNode);
			return;
		}
		else if( pNode->bArray != (bArray==TRUE) ) // 数组释放检测
		{
			pNode->bArrayError = true;
			m_listArrayErrorNode.PushBack(pNode);
			return;
		}
		else if( bBig )
		{
			pNode->bBig = true;
			m_listBigNode.PushBack(pNode);
			return;
		}
		else	// 无错
		{
			free(pNode);	// 释放节点内容
		}
	}

	free(pAddress);	// 释放实际内存
}

//-----------------------------------------------------------------------------
// ReplaceMemNode
//-----------------------------------------------------------------------------
LPVOID MemTrace::ReAlloc( LPCSTR szFile, INT nLine, size_t uiSize, BOOL bArray, LPVOID pAddress )
{
	if( m_bTerminate )	// 防止对象析构后被继续调用
		return realloc( pAddress, uiSize );

	// 是否从这里分配的
	tagNode* pNode = m_mapMem.Peek(pAddress);
	if( !VALID(pNode) )
		return realloc( pAddress, uiSize );

	if( pAddress && uiSize )
	{
		// 分配新内存
		LPVOID pNew = g_MemMap.Alloc(szFile, nLine, uiSize, bArray);
		if( !pNew )
			return NULL;

		// 拷贝
		memcpy( pNew, pAddress, EMin(pNode->uiSize, uiSize));

		// 释放原内存
		Free( pAddress, bArray );
		return pNew;
	}
	else if( pAddress )
	{
		Free( pAddress, bArray );
		return NULL;
	}
	else if( uiSize )
	{
		return Alloc(szFile, nLine, uiSize, bArray );
	}
	else
	{
		return NULL;
	}

}

//-----------------------------------------------------------------------------
// 为分配出来的内存设置调试信息，以便查找
//-----------------------------------------------------------------------------
BOOL MemTrace::SetDesc(LPVOID pAddress, LPCSTR szDesc)
{
	if( !szDesc )
		return FALSE;

	if( m_bTerminate )	// 防止对象析构后被继续调用
		return FALSE;

	tagNode* pNode = m_mapMem.Peek(pAddress);
	if( !VALID(pNode) )
		return FALSE;
	
	strcpy_s(pNode->szDesc, szDesc);
	return TRUE;
}


//-----------------------------------------------------------------------------
// Refresh
//-----------------------------------------------------------------------------
VOID MemTrace::Log()
{
	// 生成合适的文件名
	CHAR szFileName[MAX_PATH], szTime[MAX_PATH], szTemp[MAX_PATH];
	GetModuleFileNameA(NULL, szTemp, MAX_PATH);
	CHAR *pResult = strrchr(szTemp, '\\');
	pResult = pResult ?	pResult+1 :	pResult = (CHAR*)szTemp;
	sprintf_s(szFileName, "log\\%s", pResult);
	pResult = strrchr(szFileName, '.');
	if( pResult )
		*pResult = '\0';

	// 加上时间
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	szTime[0] = '\0';
	WORD wDate, wTime;
	if (FileTimeToLocalFileTime(&CurrentTime, &CurrentTime) &&
		FileTimeToDosDateTime(&CurrentTime, &wDate, &wTime))
	{
		// 年月日以及时分秒和进程id
		sprintf_s(szTime, "_Mem[%d-%d-%d %02d%02d%02d %05d].log",
			(wDate / 32) & 15, wDate & 31, (wDate / 512) + 1980,
			(wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2, 
			GetCurrentProcessId());
		strcat_s(szFileName, szTime);
	}

	FILE *fp = fopen(szFileName, "wb"); // 清空原先的内容
	if( !fp ) return;

	std::list<tagNode*> listNode;
	std::list<tagNode*>::iterator it;
	
	// 内存泄露
	fprintf(fp, "-----------------  Memory Leaks  -----------------\r\n");
	size_t uiTotalSize = 0;
	m_mapMem.ExportAllValue(listNode);
	for(it = listNode.begin(); it != listNode.end(); ++it)
	{
		tagNode* pNode = (*it);
		fprintf(fp, "%s, line:%d, size:%d, address:0x%X, desc:%s\r\n", pNode->szFile, pNode->nLine, pNode->uiSize, pNode->pAddress, pNode->szDesc);
		uiTotalSize += pNode->uiSize;
	}
	fprintf(fp, "Total leak size:%d\r\n\r\n", uiTotalSize);


	// 释放类型错误
	listNode.clear();
	fprintf(fp, "-----------------  Free Array Error  -----------------\r\n");
	m_listArrayErrorNode.Export(listNode);
	for(it = listNode.begin(); it != listNode.end(); ++it)
	{
		tagNode* pNode = (*it);
		fprintf(fp, "%s, line:%d, size:%d, address:0x%X, desc:%s\r\n", pNode->szFile, pNode->nLine, pNode->uiSize, pNode->pAddress, pNode->szDesc);
	}

	// 越界错误
	listNode.clear();
	fprintf(fp, "-----------------  Bound Error  -----------------\r\n");
	m_listBoundErrorNode.Export(listNode);
	for(it = listNode.begin(); it != listNode.end(); ++it)
	{
		tagNode* pNode = (*it);
		fprintf(fp, "%s, line:%d, size:%d, address:0x%X, desc:%s\r\n", pNode->szFile, pNode->nLine, pNode->uiSize, pNode->pAddress, pNode->szDesc);
	}

	// 巨大内存分配
	listNode.clear();
	fprintf(fp, "-----------------  Big Memory Allocate  -----------------\r\n");
	m_listBigNode.Export(listNode);
	for(it = listNode.begin(); it != listNode.end(); ++it)
	{
		tagNode* pNode = (*it);
		fprintf(fp, "%s, line:%d, size:%d, address:0x%X, desc:%s\r\n", pNode->szFile, pNode->nLine, pNode->uiSize, pNode->pAddress, pNode->szDesc);
	}

	fclose(fp);
}


} // namespace ECore {

#endif // #ifdef MEM_TRACE