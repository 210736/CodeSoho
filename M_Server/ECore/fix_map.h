//-----------------------------------------------------------------------------
// author Lyp
// date 2011-04-18
// last 2011-04-18
//-----------------------------------------------------------------------------
#pragma once

namespace ECore {

template<typename T>
int FixMapSortCompare(const void* p1, const void* p2)
{
	if( *(T*)p1 < *(T*)p2 )
		return -1;
	else if( *(T*)p1 > *(T*)p2 )
		return 1;
	else 
		return 0;
}
//-----------------------------------------------------------------------------
// 固定容量的map,内部用固定大小数组来存储
//-----------------------------------------------------------------------------
template<typename KEY_TYPE, typename VALUE_TYPE, SIZE_T ARRAY_SIZE>
class FixMap : public MemCacheObj
{
public:
	struct tagItem
	{
		KEY_TYPE	key;
		VALUE_TYPE	value;
	};

	//-------------------------------------------------------------------------
	// 添加元素，如果从一个大型map导入数据建议用更高效的LoadMap
	//-------------------------------------------------------------------------
	BOOL Add(KEY_TYPE key, VALUE_TYPE value)
	{
		if( m_nItem >= ARRAY_SIZE )
			return ERR("Out of array");

		if( m_nItem == 0 || m_map[m_nItem-1].key < key ) // 所给的key是最大的
		{
			m_map[m_nItem].key = key;
			m_map[m_nItem].value = value;
			++m_nItem;
			return TRUE;
		}

		if( m_nItem == 1 )	// 至少要两个元素才能进行下面的二分插入
		{
			m_map[1].key = m_map[0].key;
			m_map[1].value = m_map[0].value;
			m_map[0].key = key;
			m_map[0].value = value;
			++m_nItem;
			return TRUE;
		}
		
		// 二分插入
		SIZE_T nStart = 0, nEnd = m_nItem + 1, nPos = 0;
		while( nEnd != nStart )	
		{
			nPos = nStart + (nEnd-nStart)/2;
			if( key >  m_map[nPos-1].key && key >  m_map[nPos].key )
				nStart = nPos;
			else if( key <  m_map[nPos-1].key && key <  m_map[nPos].key )
				nEnd = nPos;
			else if( key == m_map[nPos-1].key || key == m_map[nPos].key)
				return FALSE;
			else
				break;
		}
		
		// 将nPos后的Table向后移动
		memmove(&m_map[nPos+1], &m_map[nPos], sizeof(tagItem)*(m_nItem-nPos));
		m_map[nPos].key = key;
		m_map[nPos].value = value;
		++m_nItem;
		return TRUE;
	}


	//-------------------------------------------------------------------------
	// 添加元素到末尾，但暂时不排序
	//-------------------------------------------------------------------------
	BOOL PushBack(KEY_TYPE key, VALUE_TYPE value)
	{
		if( m_nItem >= ARRAY_SIZE )
			return ERR("Out of array");

		m_map[m_nItem].key = key;
		m_map[m_nItem].value = value;
		++m_nItem;
		return TRUE;
	}

	//-------------------------------------------------------------------------
	// 排序
	//-------------------------------------------------------------------------
	VOID Sort() { qsort(m_map, m_nItem, sizeof(tagItem), FixMapSortCompare<KEY_TYPE>); }

	//--------------------------------------------------------------------------
	// 读取元素
	//--------------------------------------------------------------------------
	VALUE_TYPE Peek(KEY_TYPE key)
	{
		SIZE_T nStart = 0, nEnd = m_nItem, nPos = 0;

		while( nEnd != nStart )	// 二分查找
		{
			nPos = nStart + (nEnd-nStart)/2;
			if( key == m_map[nPos].key ) 
				return m_map[nPos].value;
			if( nEnd == nStart+1 )
				return VALUE_TYPE(-1);
			if( key >=  m_map[nPos].key )
				nStart = nPos;
			else
				nEnd = nPos;
		}
		return VALUE_TYPE(-1);
	}

	//--------------------------------------------------------------------------
	// 删除元素
	//--------------------------------------------------------------------------
	BOOL Erase(KEY_TYPE key)
	{
		SIZE_T nStart = 0, nEnd = m_nItem, nPos = 0;
		while( nEnd != nStart )	// 二分查找
		{
			nPos = nStart + (nEnd-nStart)/2;
			if( key == m_map[nPos].key ) 
			{
				// 将nPos后的Table向前移动
				if( nPos < (m_nItem - 1) )
					memmove(&m_map[nPos], &m_map[nPos+1], sizeof(tagItem)*(m_nItem-nPos-1));
				--m_nItem;
				return TRUE;
			}
			if( nEnd == nStart+1 )
				return FALSE;
			if( key >=  m_map[nPos].key )
				nStart = nPos;
			else
				nEnd = nPos;
		}
		return FALSE;
	}


	//-------------------------------------------------------------------------
	// 通过下标获取元素
	//-------------------------------------------------------------------------
	VALUE_TYPE PeekViaIndex(SIZE_T n) { return m_map[n].value; }


	//-------------------------------------------------------------------------------
	// 得到元素个数
	//-------------------------------------------------------------------------------
	SIZE_T	Size()	{ return m_nItem; }

	//-------------------------------------------------------------------------------
	// 得到容器大小
	//-------------------------------------------------------------------------------
	SIZE_T GetCapcity() { return ARRAY_SIZE; }

	//-------------------------------------------------------------------------------
	// 清除所有元素
	//-------------------------------------------------------------------------------
	VOID Clear() { m_nItem = 0; }

	FixMap():m_nItem(0) {}

private:
	tagItem		m_map[ARRAY_SIZE];
	SIZE_T		m_nItem;
};

}