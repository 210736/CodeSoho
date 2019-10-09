#pragma once

namespace ECore
{
	/**	\class FreePtrVector
		\brief delete一个vector中的所有指针, 并且清空此vector
	*/
	template<class T>
		class FreePtrVector
	{
	public:
		void operator()(vector<T*>& array)
		{
			for(size_t i=0;i<array.size();i++)
			{
				T* pObj=array[i];
				delete pObj;
			}
			array.clear();
		}
	};

	/**	\class FreePtrList
		\brief delete一个list中的所有指针, 并且清空此list
	*/
	template<class T>
	class FreePtrList
	{
	public:
		void operator()(list<T*>& _list)
		{
			for(list<T*>::iterator iter=_list.begin();
				iter!=_list.end();++iter)
			{
				T* pObj=*iter;
				delete pObj;
			}
			_list.clear();
		}
	};

	/**	\class FreePtrArray
		\brief delete一个array中的所有指针, 并且清空此array
	*/
	template<class T>
	class FreePtrArray
	{
	public:
		void operator()(DynaArray<T*>& array)
		{
			for(size_t i=0;i<array.size();i++)
			{
				T* pObj=array[i];
				delete pObj;
			}
			array.clear();
		}
	};

	/**	\class FreePtrSimpleVector
		\brief delete一个array中的所有指针, 并且清空此array
	*/
	template<class T>
	class FreePtrSimpleVector
	{
	public:
		void operator()(SimpleVector<T*>& array)
		{
			for(size_t i=0;i<array.size();i++)
			{
				T* pObj=array[i];
				delete pObj;
			}
			array.clear();
		}
	};

	/**	\class FreePtrMap
		\brief delete一个map中的所有指针, 并且清空此map
	*/
	template<class K,class T>
	class FreePtrMap
	{
	public:
		void operator()(map<K,T*>& _map)
		{
			for(map<K,T*>::iterator iter=_map.begin();
				iter!=_map.end();++iter)
			{
				T* pObj=iter->second;
				delete pObj;
			}
			_map.clear();
		}
	};
}//namespace ECore