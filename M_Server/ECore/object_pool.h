//------------------------------------------------------------------------------------------------------
// Auth: wb
// Date: 2011/11/18	
//------------------------------------------------------------------------------------------------------
#pragma once

#include "simple_vector.h"

namespace ECore
{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// DefaultObjFactory


	template<class ObjType>
	class DefaultObjFactory
	{
	public:
		ObjType* NewObj()			{	return new ObjType;}
		void DelObj(ObjType *pObj)	{	delete pObj; }
	};


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ObjectPool


	/**	
	 * �����,�ڴ���ʱһ�η���n����������ʱʹ��
	 */
	template< class ObjType >
	class ObjectPool : public MemCacheObj
	{
	public:
		/** 
		 * ���죬�������ж���
		 */
		ObjectPool(size_t size)
			: m_numAlloced( 0 )
			, m_maxAlloced( 0 )
			, m_pData( NULL )
		{
			BYTE* pData = (BYTE*)MCALLOC((DWORD)(sizeof(ObjType)*size));
			if( !VALID(pData) )
				return;

			m_pData = pData;
			m_objs.resize(size);
			for( size_t i=0; i<m_objs.size(); ++i, pData += sizeof(ObjType) )
			{
				ObjType *pObj = (ObjType*)pData;

				m_objs[i] = pObj;
#ifdef DEBUG
				m_objSet.insert(pObj);
#endif
			}
		}

		/** 
		 * �������ͷ����ж���
		 */
		virtual ~ObjectPool(void) {	Destroy(); }

		/** 
		 * �ͷ����ж���
		 */
		void Destroy()
		{
			MCFREE( m_pData );
			m_pData = NULL;

			m_objs.clear();
#ifdef DEBUG
			m_objSet.clear();
#endif
		}

		/** 
		 * ȡ��һ������
		 */
		ObjType* AllocObj()
		{
			if(m_objs.empty())
			{
				ASSERT( 0 && "Obj pool overflow");
				return NULL;
			}
			else
			{
				ObjType *pRet = m_objs.back();
				m_objs.pop_back();
				m_numAlloced++;

				if(m_numAlloced > m_maxAlloced)
					m_maxAlloced = m_numAlloced;

				return pRet;
			}
		}

		/** 
		 * �黹һ������
		 */
		void FreeObj(ObjType *pObj)
		{
#ifdef DEBUG
			ASSERT(m_objSet.find(pObj)!=m_objSet.end());//������������������ȥ��
#endif
			m_objs.push_back(pObj);
			m_numAlloced--;
		}
	
		/** 
		 * ������ʷ�����������������ڵ��Ի���ʾ
		 */
		int GetMaxAlloced() const	{	return m_maxAlloced;}
		/** 
		 * ���ض�����Ƿ�Ϊ��
		 */
		BOOL IsEmpty() const		{	return m_objs.empty();}

	protected:
		BYTE*					m_pData;
		SimpleVector<ObjType*>	m_objs;
		int						m_numAlloced;
		int						m_maxAlloced;
#ifdef DEBUG
		set<ObjType*>			m_objSet;
#endif
	};

}//namespace ECore