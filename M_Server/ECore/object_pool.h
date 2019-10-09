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
	 * 对象池,在创建时一次分配n个对象供运行时使用
	 */
	template< class ObjType >
	class ObjectPool : public MemCacheObj
	{
	public:
		/** 
		 * 构造，分配所有对象
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
		 * 析构，释放所有对象
		 */
		virtual ~ObjectPool(void) {	Destroy(); }

		/** 
		 * 释放所有对象
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
		 * 取走一个对象
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
		 * 归还一个对象
		 */
		void FreeObj(ObjType *pObj)
		{
#ifdef DEBUG
			ASSERT(m_objSet.find(pObj)!=m_objSet.end());//必须是我们这里分配出去的
#endif
			m_objs.push_back(pObj);
			m_numAlloced--;
		}
	
		/** 
		 * 返回历史最大分配数量，仅用于调试或显示
		 */
		int GetMaxAlloced() const	{	return m_maxAlloced;}
		/** 
		 * 返回对象池是否为空
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