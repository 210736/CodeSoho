#pragma once
#include "FArchive.h"
#include "GeomDefine.h"
#include "Ray.h"

namespace ECore
{
	/**	\class Heightmap
		\brief �߶�ͼ
	*/
	class ECORE_API Heightmap : public MemCacheObj
	{
	public:
		/** ��ʽ����
		*/
		enum EFormat
		{
			EF_8Bit,
			EF_16Bit
		};
	public:
		Heightmap(void);
		virtual ~Heightmap(void);

		/** ��������
		*/
		INLINE int Width() const	{ return m_hmapWidth;  }
		INLINE int Height() const	{ return m_hmapHeight; }
		INLINE float GetXScale() const	{ return m_xScale; }
		INLINE float GetYScale() const	{ return m_yScale; }
		INLINE float GetZScale() const	{ return m_zScale; }
		INLINE Vector2 GetStartPos() const { return m_startPos;}
		INLINE EFormat GetFormat() const { return m_format;}
		INLINE void SetXScale(float s)		{	m_xScale=s;}
		INLINE void SetYScale(float s)		{	m_yScale=s;}
		INLINE void SetZScale(float s)		{	m_zScale=s;}
		INLINE void SetStartPos(Vector2& pos) { m_startPos=pos;}

		/** �жϸ��������Ƿ񳬳��߽� 
		*/
		INLINE bool IfIndex(int x,int z) const
		{ return x>=0 && z>=0 && x<(int)m_hmapWidth && z<(int)m_hmapHeight; }
		/** ��������->����
		*/
		INLINE int GetIndex(int x,int z) const
		{
			return z*m_hmapWidth+x;
		}

		/** ��������<->��������
		*/
		INLINE Point World2Tile(const Vector3& pt) const
		{
			return Point(long((pt.x-m_startPos.x)/m_xScale),
				long((pt.z-m_startPos.y)/m_zScale));
		}
		INLINE Point World2Tile(FLOAT x, FLOAT z) const
		{
			return Point(long((x-m_startPos.x)/m_xScale),
				long((z-m_startPos.y)/m_zScale));
		}		
		INLINE Vector3 Tile2World(int x,int z) const
		{
			return Vector3(x*m_xScale+m_startPos.x,GetHeight(x,z),
				z*m_zScale+m_startPos.y);
		}
		INLINE Vector3 SafeTile2World(int x,int z) const
		{
			return Vector3(x*m_xScale+m_startPos.x,GetSafeHeight(x,z),
				z*m_zScale+m_startPos.y);
		}

		/** ���ظ߶�ֵ
		*/
		INLINE float GetHeight(int x,int z) const
		{
			ASSERT(x>=0 && x<m_hmapWidth);
			ASSERT(z>=0 && z<m_hmapHeight);
			if(m_format==EF_8Bit)
				return m_heightMap8[GetIndex(x,z)]*m_yScale;
			else
				return m_heightMap16[GetIndex(x,z)]*m_yScale;
		}
		
		/** ���ظ߶�
		*/
		INLINE int GetValue(int x,int z) const
		{
			ASSERT(x>=0 && x<m_hmapWidth);
			ASSERT(z>=0 && z<m_hmapHeight);
			if(m_format==EF_8Bit)
				return m_heightMap8[GetIndex(x,z)];
			else
				return m_heightMap16[GetIndex(x,z)];
		}

		/**	��������clip,Ȼ���ڵõ��߶�
		*/
		INLINE float GetSafeHeight(int x,int z) const;
		int GetSafeVal(int x,int z) const;

		/** ���ø߶�ֵ
		*/
		INLINE void SetValue(int x,int z,int val)
		{
			ASSERT(x>=0 && x<m_hmapWidth);
			ASSERT(z>=0 && z<m_hmapHeight);
			if(m_format==EF_8Bit)
				m_heightMap8[GetIndex(x,z)]=val;
			else
				m_heightMap16[GetIndex(x,z)]=val;
		}

		/** ��������clip,Ȼ�������Ӹ߶�
		*/
		void SafeAddValue(int x,int z,int& val);
		/** ��������clip,Ȼ�������ø߶�
		*/
		void SafeSetVal(int x,int z,int& val);

		/** ���㶥��λ��
		*/
		INLINE void GetVertPos(int x,int z,Vector3& out) const
		{
			out.x=m_startPos.x+x*m_xScale;
			out.z=m_startPos.y+z*m_zScale;
			out.y=GetHeight(x,z);
		}
		INLINE void GetVertPos(int index,Vector3& out) const
		{
			int x=index%m_hmapWidth;
			int z=index/m_hmapWidth;
			GetVertPos(x,z,out);
		}

		/** ���㶥�㷨��
		*/
		void GetVertNormal(int x,int z,Vector3& out) const;
		void GetVertNormal(int index,Vector3& out) const
		{
			int x=index%m_hmapWidth;
			int z=index/m_hmapWidth;
			GetVertNormal(x,z,out);
		}

		/** ���㶥������
		*/
		void GetVertTangent(int x,int z,Vector3& out,const Vector3* pTangentRotate=NULL) const;

		/** �����Ƿ��Ѵ���
		*/
		INLINE bool IsCreated() const	{ return m_heightMap8!=0 || m_heightMap16!=0;}
		/** �ͷ�����
		*/
		void Destroy();
		/** �����߶�ͼ
		*/
		void Create(int initVal,int width,int height,float xScale,float zScale,float yScale,EFormat format=EF_8Bit, bool bSetInitVal=true);

		/**	�����������εĽ���
		*/
		bool RayCollision(const Ray& ray,Vector3& pt,float maxLength=100000.0f,bool bAutoGroundHeight=true) const;

		/** ���������
		*/
		bool WriteToFile(FILE *fp) const;
		bool WriteToFile(LPCSTR szPath) const;
		bool ReadFromFile(VFS* pFS,HANDLE hFile);
		void Serialize(FArchive& ar);
		void Deserialize(FArchive& ar);

		/** ����AABBox
		*/
		void BuildAABBox(AABBox *pBox,const Rect* pRect=NULL) const;
		/** ����߶ȷ�Χ
		*/
		void GetHeightRange(float& minh,float &maxh,Rect rect) const;
		void GetHeightRange(int& minh,int &maxh,Rect rect) const;

		/** StartPos���õ�����λ��
		*/
		void AutoCenterStartPos();

		/** �����������Heightmap
		*/
		Heightmap *NewSubMap(Rect rect) const;
		void SetSubMap(Heightmap *pSubMap,Rect& rect);

		/** ���Լ�������copy��ָ���Ķ�����
		*/
		void CopyTo(Heightmap& out) const;
		/** �����ڲ��߶����ݻ�����
		*/
		BYTE *GetRawBuffer() const	{	
			if(m_format==EF_8Bit)
				return m_heightMap8;
			else
				return (BYTE*)m_heightMap16;
		}
		/** �������
		*/
		void Fill(int val);

		/** 8bitת����16bit
		*/
		void ConvertTo16Bit();

		/** ��������
		*/
		INLINE void ClampPos(int& x,int& z);
		INLINE void ClampPos(Point& t);
		INLINE void ClampRect(Rect& rc);

		/** ���ɸ߶ȳ�
		*/
		void BuildHeightField(INT gridSize, Heightmap& out);

	protected:
		void CreateData(int width,int height,EFormat format);
	protected:
		/** ���Ӵ�С
		*/
		float	m_xScale,
				m_zScale,
				m_yScale;
		/** ��ʼ����,map��(0,0)���ض�Ӧ��vert�������е�λ��
		*/
		Vector2 m_startPos;	
		/** map�ߴ�
		*/
		int		m_hmapWidth,
				m_hmapHeight;
		/** �߶����ݻ�����
		*/
		BYTE*	m_heightMap8;
		WORD*	m_heightMap16;
		/** ��ʽ
		*/
		EFormat m_format;
	};

	//---------------------------------------------------------------------------
	INLINE void Heightmap::ClampPos(Point& t)
	{
		if( t.x < 0 ) 
			t.x = 0;
		if( t.x >= m_hmapWidth )
			t.x = m_hmapWidth - 1;
		if( t.y < 0 )
			t.y = 0;
		if( t.y >= m_hmapHeight )
			t.y = m_hmapHeight - 1;
	}

	//---------------------------------------------------------------------------
	INLINE void Heightmap::ClampPos(int& x,int& z)
	{
		if( x < 0 ) 
			x = 0;
		if( x >= m_hmapWidth )
			x = m_hmapWidth - 1;
		if( z < 0 )
			z = 0;
		if( z >= m_hmapHeight )
			z = m_hmapHeight - 1;
	}

	//---------------------------------------------------------------------------
	INLINE float Heightmap::GetSafeHeight(int x,int z) const
	{
		if(x<0)
			x=0;
		if(x>(int)m_hmapWidth-1)
			x=m_hmapWidth-1;
		if(z<0)
			z=0;
		if(z>(int)m_hmapHeight-1)
			z=m_hmapHeight-1;
		return GetHeight(x,z);
	}
}//namespace ECore