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

ECore::MemTrace g_MemMap;	// ȫ�ֶ����Ա��ڳ������������

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
// Ϊ����������ڴ����õ�����Ϣ���Ա����
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
	Log();	// �������ʱд��log
}


//-----------------------------------------------------------------------------
// InsertMemNode
//-----------------------------------------------------------------------------
LPVOID MemTrace::Alloc(LPCSTR szFile, INT nLine, size_t uiSize, BOOL bArray)
{
	LPVOID pAddress = NULL;
	size_t uiNewSize = uiSize + sizeof(DWORD) * 2;	// ͷβ�������־
	
	while( true )	// ���Է����ڴ�
	{
		pAddress = malloc(uiNewSize);
		if( pAddress )	// ����ɹ�
		{
			if( m_bTerminate )
				return pAddress;
			break;
		}

		// ���䲻�ɹ����ҳ���ǰ��������
		new_handler fGlobalHandler = set_new_handler(0);
		set_new_handler(fGlobalHandler);

		if( fGlobalHandler )
			(*fGlobalHandler)();	// ���ó�����
		else 
			throw std::bad_alloc();
	}

	// ����ɹ���д����β���������
	*(DWORD*)pAddress = 0xDeadBeef;
	*(DWORD*)((BYTE*)pAddress + uiSize + sizeof(DWORD)) = 0xDeadBeef;

	// ��ָ����Ƶ��ⲿ���õĵ�ַ
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
		__asm int 3;	// �ϵ�
	}

	return pAddress;
}


//-----------------------------------------------------------------------------
// RemoveMemNode
//-----------------------------------------------------------------------------
VOID MemTrace::Free(LPVOID pAddress, BOOL bArray)
{
	if( m_bTerminate )	// ��ֹ���������󱻼�������
		return;

	tagNode* pNode = m_mapMem.Peek(pAddress);
	if( VALID(pNode) )
	{
		m_mapMem.Erase(pAddress);
		pAddress = (BYTE*)pAddress - sizeof(DWORD);	// �õ�ʵ�ʵ�ַ

		BOOL bStartChanged = (*(DWORD*)pAddress != 0xDeadBeef);
		BOOL bEndChanged = (*(DWORD*)((BYTE*)pAddress + pNode->uiSize + sizeof(DWORD)) != 0xDeadBeef);
		BOOL bBig = pNode->uiSize > 1 * 1024 * 1024;

		if( bStartChanged || bEndChanged )	// ������
		{
			ASSERT(0);
			pNode->bBoundError = true;
			m_listBoundErrorNode.PushBack(pNode);
			return;
		}
		else if( pNode->bArray != (bArray==TRUE) ) // �����ͷż��
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
		else	// �޴�
		{
			free(pNode);	// �ͷŽڵ�����
		}
	}

	free(pAddress);	// �ͷ�ʵ���ڴ�
}

//-----------------------------------------------------------------------------
// ReplaceMemNode
//-----------------------------------------------------------------------------
LPVOID MemTrace::ReAlloc( LPCSTR szFile, INT nLine, size_t uiSize, BOOL bArray, LPVOID pAddress )
{
	if( m_bTerminate )	// ��ֹ���������󱻼�������
		return realloc( pAddress, uiSize );

	// �Ƿ����������
	tagNode* pNode = m_mapMem.Peek(pAddress);
	if( !VALID(pNode) )
		return realloc( pAddress, uiSize );

	if( pAddress && uiSize )
	{
		// �������ڴ�
		LPVOID pNew = g_MemMap.Alloc(szFile, nLine, uiSize, bArray);
		if( !pNew )
			return NULL;

		// ����
		memcpy( pNew, pAddress, EMin(pNode->uiSize, uiSize));

		// �ͷ�ԭ�ڴ�
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
// Ϊ����������ڴ����õ�����Ϣ���Ա����
//-----------------------------------------------------------------------------
BOOL MemTrace::SetDesc(LPVOID pAddress, LPCSTR szDesc)
{
	if( !szDesc )
		return FALSE;

	if( m_bTerminate )	// ��ֹ���������󱻼�������
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
	// ���ɺ��ʵ��ļ���
	CHAR szFileName[MAX_PATH], szTime[MAX_PATH], szTemp[MAX_PATH];
	GetModuleFileNameA(NULL, szTemp, MAX_PATH);
	CHAR *pResult = strrchr(szTemp, '\\');
	pResult = pResult ?	pResult+1 :	pResult = (CHAR*)szTemp;
	sprintf_s(szFileName, "log\\%s", pResult);
	pResult = strrchr(szFileName, '.');
	if( pResult )
		*pResult = '\0';

	// ����ʱ��
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	szTime[0] = '\0';
	WORD wDate, wTime;
	if (FileTimeToLocalFileTime(&CurrentTime, &CurrentTime) &&
		FileTimeToDosDateTime(&CurrentTime, &wDate, &wTime))
	{
		// �������Լ�ʱ����ͽ���id
		sprintf_s(szTime, "_Mem[%d-%d-%d %02d%02d%02d %05d].log",
			(wDate / 32) & 15, wDate & 31, (wDate / 512) + 1980,
			(wTime >> 11), (wTime >> 5) & 0x3F, (wTime & 0x1F) * 2, 
			GetCurrentProcessId());
		strcat_s(szFileName, szTime);
	}

	FILE *fp = fopen(szFileName, "wb"); // ���ԭ�ȵ�����
	if( !fp ) return;

	std::list<tagNode*> listNode;
	std::list<tagNode*>::iterator it;
	
	// �ڴ�й¶
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


	// �ͷ����ʹ���
	listNode.clear();
	fprintf(fp, "-----------------  Free Array Error  -----------------\r\n");
	m_listArrayErrorNode.Export(listNode);
	for(it = listNode.begin(); it != listNode.end(); ++it)
	{
		tagNode* pNode = (*it);
		fprintf(fp, "%s, line:%d, size:%d, address:0x%X, desc:%s\r\n", pNode->szFile, pNode->nLine, pNode->uiSize, pNode->pAddress, pNode->szDesc);
	}

	// Խ�����
	listNode.clear();
	fprintf(fp, "-----------------  Bound Error  -----------------\r\n");
	m_listBoundErrorNode.Export(listNode);
	for(it = listNode.begin(); it != listNode.end(); ++it)
	{
		tagNode* pNode = (*it);
		fprintf(fp, "%s, line:%d, size:%d, address:0x%X, desc:%s\r\n", pNode->szFile, pNode->nLine, pNode->uiSize, pNode->pAddress, pNode->szDesc);
	}

	// �޴��ڴ����
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