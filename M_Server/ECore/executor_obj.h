//-----------------------------------------------------------------------------
// File: executor_obj.h
// Desc: ���ڶ�����߳�ִ���壬ĳ����ͨ���̳и���õ�ĳ���������߳�ִ������
//       �̳и�������������Լ�ʵ��run�ӿ�
// Auth: Aslan
// Date: 2009-12-09
// Last: 2009-12-09
//-----------------------------------------------------------------------------
#pragma once

#include "executor.h"

namespace ECore{

//-----------------------------------------------------------------------------
// ���ڶ�����߳�ִ����
//-----------------------------------------------------------------------------
class ECORE_API ExecutorObj : public Executor
{
public:
	ExecutorObj();
	virtual ~ExecutorObj();

	//-------------------------------------------------------------------------
	// ִ�нӿڣ��̳�ExecutroObj��������Լ�ʵ��run
	//-------------------------------------------------------------------------
	virtual VOID	Run();

	//-------------------------------------------------------------------------
	// �ṩ�ر��߳�ʱ�Ļص��ӿڣ�ֻ��Ҫ����m_bTerminate�Ϳ�����
	//-------------------------------------------------------------------------
	virtual VOID	OnShutDown();

protected:
	BOOL volatile	m_bTerminate;		// �߳̽�����־	
};

} // namespace ECore{