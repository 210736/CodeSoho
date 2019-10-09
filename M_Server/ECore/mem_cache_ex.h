#pragma once

namespace ECore
{
// 内存池对象基类
class ECORE_API MemCacheObj
{
	public:
#ifndef MEM_TRACE
	INLINE void* operator new(size_t size) { return MCALLOC((DWORD)size); }
	INLINE void* operator new[](size_t size) { return MCALLOC((DWORD)size); }
	INLINE void operator delete(void* p) {	MCFREE(p); }
	INLINE void operator delete[](void* p) {	MCFREE(p); }
#endif
};

template <class T>
class SafeCacheAlloc
{

public:
	typedef T                value_type;
	typedef T*               pointer;
	typedef const T*         const_pointer;
	typedef T&               reference;
	typedef const T&         const_reference;
	typedef size_t           size_type;
	typedef ptrdiff_t        difference_type;

	// 基本的构造函数和析构函数
	SafeCacheAlloc() {}
	SafeCacheAlloc(const SafeCacheAlloc&) {}
	~SafeCacheAlloc() {}

	// rebind 相关
	template <class U>
	SafeCacheAlloc(const SafeCacheAlloc<U>& ) {}

	template <class U>
	struct rebind{typedef SafeCacheAlloc<U> other;};

	// 地址定位
	pointer address(value_type& x) { return &x; }

	const_pointer address(const value_type& x) { return &x; }

	// 分配内存和释放内存
	pointer allocate(size_type n, const_pointer = 0) { return static_cast<pointer>(MCALLOC(DWORD(n*sizeof(value_type)))); }
	void deallocate(pointer p, size_type) { MCFREE(p); }

	size_type max_size() const { return static_cast<size_type>( UINT_MAX/sizeof(value_type) ); }

	// 构建和销毁
	void construct(pointer p, const value_type& x) { ::new(p) value_type(x); }
	void destroy(pointer p) { p->~value_type(); }

	bool operator==(const SafeCacheAlloc<T>& other) const {	return true; }
};

typedef std::basic_string<char, char_traits<char>, SafeCacheAlloc<char>> estring;
typedef std::basic_stringstream<char, char_traits<char>, SafeCacheAlloc<char> > estringstream;


typedef basic_string<wchar_t, char_traits<wchar_t>, SafeCacheAlloc<wchar_t> > etstring;
typedef basic_stringstream<wchar_t, char_traits<wchar_t>, SafeCacheAlloc<wchar_t> > etstringstream;



}//namespace ECore