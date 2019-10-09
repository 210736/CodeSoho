#pragma once
#include "FArchive.h"
#include "GeomDefine.h"
#include "Ray.h"

namespace ECore
{
	/**	\class Heightmap
		\brief 高度图
	*/
	class ECORE_API Heightmap : public MemCacheObj
	{
	public:
		/** 格式定义
		*/
		enum EFormat
		{
			EF_8Bit,
			EF_16Bit
		};
	public:
		Heightmap(void);
		virtual ~Heightmap(void);

		/** 基本属性
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

		/** 判断格子坐标是否超出边界 
		*/
		INLINE bool IfIndex(int x,int z) const
		{ return x>=0 && z>=0 && x<(int)m_hmapWidth && z<(int)m_hmapHeight; }
		/** 格子坐标->索引
		*/
		INLINE int GetIndex(int x,int z) const
		{
			return z*m_hmapWidth+x;
		}

		/** 世界坐标<->格子坐标
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

		/** 返回高度值
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
		
		/** 返回高度
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

		/**	进行坐标clip,然后在得到高度
		*/
		INLINE float GetSafeHeight(int x,int z) const;
		int GetSafeVal(int x,int z) const;

		/** 设置高度值
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

		/** 进行坐标clip,然后再增加高度
		*/
		void SafeAddValue(int x,int z,int& val);
		/** 进行坐标clip,然后再设置高度
		*/
		void SafeSetVal(int x,int z,int& val);

		/** 计算顶点位置
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

		/** 计算顶点法线
		*/
		void GetVertNormal(int x,int z,Vector3& out) const;
		void GetVertNormal(int index,Vector3& out) const
		{
			int x=index%m_hmapWidth;
			int z=index/m_hmapWidth;
			GetVertNormal(x,z,out);
		}

		/** 计算顶点切线
		*/
		void GetVertTangent(int x,int z,Vector3& out,const Vector3* pTangentRotate=NULL) const;

		/** 返回是否已创建
		*/
		INLINE bool IsCreated() const	{ return m_heightMap8!=0 || m_heightMap16!=0;}
		/** 释放数据
		*/
		void Destroy();
		/** 创建高度图
		*/
		void Create(int initVal,int width,int height,float xScale,float zScale,float yScale,EFormat format=EF_8Bit, bool bSetInitVal=true);

		/**	检测射线与地形的交点
		*/
		bool RayCollision(const Ray& ray,Vector3& pt,float maxLength=100000.0f,bool bAutoGroundHeight=true) const;

		/** 读盘与存盘
		*/
		bool WriteToFile(FILE *fp) const;
		bool WriteToFile(LPCSTR szPath) const;
		bool ReadFromFile(VFS* pFS,HANDLE hFile);
		void Serialize(FArchive& ar);
		void Deserialize(FArchive& ar);

		/** 计算AABBox
		*/
		void BuildAABBox(AABBox *pBox,const Rect* pRect=NULL) const;
		/** 计算高度范围
		*/
		void GetHeightRange(float& minh,float &maxh,Rect rect) const;
		void GetHeightRange(int& minh,int &maxh,Rect rect) const;

		/** StartPos设置到中心位置
		*/
		void AutoCenterStartPos();

		/** 获得与设置子Heightmap
		*/
		Heightmap *NewSubMap(Rect rect) const;
		void SetSubMap(Heightmap *pSubMap,Rect& rect);

		/** 将自己的内容copy到指定的对象中
		*/
		void CopyTo(Heightmap& out) const;
		/** 返回内部高度数据缓存区
		*/
		BYTE *GetRawBuffer() const	{	
			if(m_format==EF_8Bit)
				return m_heightMap8;
			else
				return (BYTE*)m_heightMap16;
		}
		/** 填充数据
		*/
		void Fill(int val);

		/** 8bit转换成16bit
		*/
		void ConvertTo16Bit();

		/** 修正坐标
		*/
		INLINE void ClampPos(int& x,int& z);
		INLINE void ClampPos(Point& t);
		INLINE void ClampRect(Rect& rc);

		/** 生成高度场
		*/
		void BuildHeightField(INT gridSize, Heightmap& out);

	protected:
		void CreateData(int width,int height,EFormat format);
	protected:
		/** 格子大小
		*/
		float	m_xScale,
				m_zScale,
				m_yScale;
		/** 起始坐标,map的(0,0)象素对应的vert在世界中的位置
		*/
		Vector2 m_startPos;	
		/** map尺寸
		*/
		int		m_hmapWidth,
				m_hmapHeight;
		/** 高度数据缓存区
		*/
		BYTE*	m_heightMap8;
		WORD*	m_heightMap16;
		/** 格式
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