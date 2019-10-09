//-----------------------------------------------------------------------------
// Auth: Lyp
// Time: 2003-1-8
//-----------------------------------------------------------------------------
#pragma once

namespace ECore{

enum EFilter	// ����
{
	EF_Null,
	EF_Changeless,
	EF_Lzo1X,
	EF_Lzo1X999,
	EF_LZMA,
	FMF_End
};
 
struct tagFiltParam	// ����������
{
	EFilter		eFilter;		// (in)���к��ֹ���
	BOOL		bCompress;		// (in)�����������
	LPVOID		pMemIn;			// (in)Դָ��(ָ�����뻺��)
	LPVOID		pMemOut;		// (in)Ŀ��ָ��(ָ���������)
	LPVOID		pMemTemp;		// (in)��ʱ����ָ��LZO1X999��
	DWORD		dwInBuffer;		// (in)���뻺���С
	DWORD		dwOutBuffer;	// (in)��������С
	SIZE_T		stInData;		// (in)�������ݴ�С
	SIZE_T		stOutData;		// (out)������ݴ�С
	
};


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class ECORE_API Filter
{
public:

	BOOL Init(EFilter eFilter);
	VOID Filt(tagFiltParam* pParam);

	// �õ�����������Ҫ��Dest���帽��ֵ
	DWORD GetExtSize(DWORD dwSize, EFilter eFilter);

	Filter();

private:
	AtomMutex m_Lock;
};



} // namespace ECore{
