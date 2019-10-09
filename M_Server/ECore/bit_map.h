#pragma once
#include "FArchive.h"
#include "GeomDefine.h"

namespace ECore
{
	/**	\class BitMap
		\biref 每个象素为1bit的map, 主要是为了处理Height map的visibility
	*/
	class ECORE_API BitMap : public MemCacheObj
	{
	public:
		BitMap(): m_height(0), m_width(0), m_pData(0), m_numBytes(0) {}
		virtual ~BitMap();

		/** 基本属性
		*/
		INLINE int Width() const	{ return m_width; }
		INLINE int Height() const	{ return m_height; }

		/** 判断格子坐标是否超出边界 
		*/
		INLINE bool IfIndex(int x,int y) const { return x>=0 && x<m_width && y>=0 && y<m_height; }
		INLINE bool IfIndex(Point pt) const { return IfIndex( pt.x, pt.y );  }

		/** 创建
		*/
		void Create(int w,int h,bool initVal, bool bSetInitVal = true);
		/** 释放
		*/
		void Destroy();
		/** 返回值
		*/
		INLINE bool GetValue(int x,int y) const;
		INLINE bool GetValue(const POINT& pt) const { return GetValue( pt.x, pt.y ); }
		INLINE bool GetValue(const Point& pt) const { return GetValue( pt.x, pt.y ); }
		/** 设置值
		*/
		void SetValue(int x,int y,bool val);
		/** 进行坐标clip,然后再返回值
		*/
		bool SafeGetValue(int x,int y) const;
		INLINE bool SafeGetValue(const POINT& pt) const { return SafeGetValue( pt.x, pt.y ); }
		INLINE bool SafeGetValue(const Point& pt) const { return SafeGetValue( pt.x, pt.y ); }
		/** 进行坐标clip,然后再设置值
		*/
		void SafeSetValue(int x,int y,bool val);

		/** 文件IO
		*/
		void WriteToFile(FILE *fp);
		bool WriteToFile(LPCSTR szPath);
		void ReadFromFile(VFS *pFS,HANDLE hFile);
		void ReadFromFile(VFS *pFS, LPCSTR szPath);

		/** clone到指定对象
		*/
		void Clone(BitMap *pTarget);
		/** 将自己的内容copy到指定的对象中
		*/
		void CopyTo(BitMap& out,int destx,int desty) const;
		void CopyTo(BitMap& out,int srcx,int srcy,int w,int h,int destx,int desty) const;

		/** 全部清0
		*/
		void Zero();

	protected:
		int		m_width,m_height;
		DWORD	*m_pData;//使用4字节指针,可以提高寻址速度
		int		m_numBytes;//buffer size
	};
}//namespace ECore