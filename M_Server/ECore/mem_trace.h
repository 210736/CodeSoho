//-----------------------------------------------------------------------------
// author Lyp
// date 2004-06-17
// last 2010-06-10
//-----------------------------------------------------------------------------
#pragma once

#ifdef MEM_TRACE

#undef new
#undef malloc
#undef calloc
#undef realloc
#undef free

namespace ECore {
//-----------------------------------------------------------------------------
// 内存跟踪监测
// 在跟踪时，可以将m_dwBreakAddress修改为内存泄露地址，当设定地址内存分配时，
// Alloc会自动断下来
//-----------------------------------------------------------------------------
class ECORE_API MemTrace
{
public:
	MemTrace();
	~MemTrace();

	LPVOID	Alloc(LPCSTR szFile, INT nLine, size_t uiSize, BOOL bArray);
	VOID	Free(LPVOID pAddress, BOOL bArray);
	LPVOID	ReAlloc(LPCSTR szFile, INT nLine, size_t uiSize, BOOL bArray, LPVOID pAddress);
	BOOL	SetDesc(LPVOID pAddress, LPCSTR szDesc);	// 为分配出来的内存设置调试信息，以便查找
	VOID	Log();

private:
	struct tagNode
	{
		LPVOID	pAddress;				// 分配地址
		LPCSTR	szFile;					// 文件名
		size_t	uiSize;					// 分配大小
		INT		nLine;					// 行号
		
		bool	bArray;					// 是否数组请求
		bool	bArrayError;			// 数组释放错误
		bool	bBoundError;			// 越界操作
		bool	bBig;					// 巨大的申请

		CHAR	szDesc[LONG_STRING];	// 调试用描述
	};

	BOOL VOLATILE 							m_bTerminate;
	MapTS<LPVOID, tagNode*, Mutex>			m_mapMem;
	ListTS<tagNode*>						m_listArrayErrorNode;
	ListTS<tagNode*>						m_listBoundErrorNode;
	ListTS<tagNode*>						m_listBigNode;

	DWORD m_dwBreakAddress;
};

} // namespace ECore {

ECORE_API VOID		WriteMem();
ECORE_API LPVOID	AllocateMemory(size_t uiSize, LPCSTR szFile, INT nLine, BOOL bArray, LPVOID pAddress=NULL); 
ECORE_API LPVOID	ReAllocateMemory(size_t uiSize, LPCSTR szFile, INT nLine, BOOL bArray, LPVOID pAddress); 
ECORE_API VOID 		DeallocateMemory(LPVOID pAddress, BOOL bArray);
ECORE_API BOOL		SetMemoryTraceDesc(LPVOID pAddress, LPCSTR szDesc);

INLINE LPVOID	operator new(size_t uiSize, LPCSTR szFile, INT nLine) { return AllocateMemory(uiSize, szFile, nLine, FALSE); }
INLINE LPVOID	operator new[](size_t uiSize, LPCSTR szFile, INT nLine) { return AllocateMemory(uiSize, szFile, nLine, TRUE); }
INLINE VOID		operator delete(LPVOID pAddress) { DeallocateMemory(pAddress, FALSE); }
INLINE VOID		operator delete[](LPVOID pAddress) { DeallocateMemory(pAddress, TRUE); }

// These two routines should never get called, unless an error occures during the 
// allocation process.  These need to be defined to make Visual C++ happy.
INLINE VOID		operator delete(LPVOID pAddress, LPCSTR szFile, INT nLine) { szFile;nLine;DeallocateMemory(pAddress, FALSE); }
INLINE VOID		operator delete[](LPVOID pAddress, LPCSTR szFile, INT nLine) { szFile;nLine;DeallocateMemory(pAddress, TRUE); }

#define new							new(__FILE__, __LINE__)
#define malloc(uiSize)				AllocateMemory(uiSize, __FILE__, __LINE__,  FALSE)
#define calloc(uiNum, uiSize)		AllocateMemory(((uiSize)*(uiNum)), __FILE__, __LINE__, FALSE)
#define realloc(pAddress, uiSize)	ReAllocateMemory(uiSize, __FILE__, __LINE__, FALSE, pAddress)
#define free(pAddress)				DeallocateMemory(pAddress, FALSE)


#endif // #ifdef MEM_TRACE