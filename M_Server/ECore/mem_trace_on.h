//-----------------------------------------------------------------------------
// author Lyp
// date 2004-06-17
// last 2010-06-10
//-----------------------------------------------------------------------------
#ifdef MEM_TRACE


#define new							new(__FILE__, __LINE__)
#define malloc(uiSize)				AllocateMemory(uiSize, __FILE__, __LINE__,  FALSE)
#define calloc(uiNum, uiSize)		AllocateMemory(((uiSize)*(uiNum)), __FILE__, __LINE__, FALSE)
#define realloc(pAddress, uiSize)	AllocateMemory(uiSize, __FILE__, __LINE__, FALSE, pAddress)
#define free(pAddress)				DeallocateMemory(pAddress, FALSE)


#endif