#pragma once
#include "FArchive.h"
#include "GeomDefine.h"

namespace ECore
{
	/**	\class BitMap
		\biref ÿ������Ϊ1bit��map, ��Ҫ��Ϊ�˴���Height map��visibility
	*/
	class ECORE_API BitMap : public MemCacheObj
	{
	public:
		BitMap(): m_height(0), m_width(0), m_pData(0), m_numBytes(0) {}
		virtual ~BitMap();

		/** ��������
		*/
		INLINE int Width() const	{ return m_width; }
		INLINE int Height() const	{ return m_height; }

		/** �жϸ��������Ƿ񳬳��߽� 
		*/
		INLINE bool IfIndex(int x,int y) const { return x>=0 && x<m_width && y>=0 && y<m_height; }
		INLINE bool IfIndex(Point pt) const { return IfIndex( pt.x, pt.y );  }

		/** ����
		*/
		void Create(int w,int h,bool initVal, bool bSetInitVal = true);
		/** �ͷ�
		*/
		void Destroy();
		/** ����ֵ
		*/
		INLINE bool GetValue(int x,int y) const;
		INLINE bool GetValue(const POINT& pt) const { return GetValue( pt.x, pt.y ); }
		INLINE bool GetValue(const Point& pt) const { return GetValue( pt.x, pt.y ); }
		/** ����ֵ
		*/
		void SetValue(int x,int y,bool val);
		/** ��������clip,Ȼ���ٷ���ֵ
		*/
		bool SafeGetValue(int x,int y) const;
		INLINE bool SafeGetValue(const POINT& pt) const { return SafeGetValue( pt.x, pt.y ); }
		INLINE bool SafeGetValue(const Point& pt) const { return SafeGetValue( pt.x, pt.y ); }
		/** ��������clip,Ȼ��������ֵ
		*/
		void SafeSetValue(int x,int y,bool val);

		/** �ļ�IO
		*/
		void WriteToFile(FILE *fp);
		bool WriteToFile(LPCSTR szPath);
		void ReadFromFile(VFS *pFS,HANDLE hFile);
		void ReadFromFile(VFS *pFS, LPCSTR szPath);

		/** clone��ָ������
		*/
		void Clone(BitMap *pTarget);
		/** ���Լ�������copy��ָ���Ķ�����
		*/
		void CopyTo(BitMap& out,int destx,int desty) const;
		void CopyTo(BitMap& out,int srcx,int srcy,int w,int h,int destx,int desty) const;

		/** ȫ����0
		*/
		void Zero();

	protected:
		int		m_width,m_height;
		DWORD	*m_pData;//ʹ��4�ֽ�ָ��,�������Ѱַ�ٶ�
		int		m_numBytes;//buffer size
	};
}//namespace ECore