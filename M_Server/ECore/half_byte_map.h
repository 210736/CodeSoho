#pragma once

namespace ECore
{
	/** °ë×Ö½ÚÍ¼
	*/
	class ECORE_API HalfByteMap : public MemCacheObj
	{
	public:
		HalfByteMap(void);
		~HalfByteMap(void);

		void Create(int w,int h,int initVal, bool bSetInitVal=true);
		void Destroy();
		INLINE int GetValue(int x,int y) const;
		int SafeGetValue(int x,int y) const;
		void SetValue(int x,int y,int val);

		INLINE UINT Width() const	{ return m_width;}
		INLINE UINT Height() const	{ return m_height; }
		INLINE bool IfIndex(int x,int y) const { return x>=0 && x<m_width 
			&& y>=0 && y<m_height;}

		void WriteFile(FILE *fp);
		void ReadFile(VFS *pFS,HANDLE hFile);

		void Clone(HalfByteMap* pTarget);

	protected:
		int		m_width,
				m_height;
		BYTE	*m_pData;
		int		m_numBytes;//buffer size
	};
}//namespace ECore

