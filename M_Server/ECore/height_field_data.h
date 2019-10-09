/*
---------------------------------------------------------------------------------
Auth: wb
Date: 2012/12/3
---------------------------------------------------------------------------------
*/
#pragma once

#include "GeomDefine.h"

namespace ECore
{
	class Heightmap;

	/*
	-----------------------------------------------------------------------------
	�߶ȳ�����

	����ѹ����	�ڲ����ݻ����ѹ������ȥ���õĶ��㣬����ͼ��9�������У�ֻȡ��+��ʾ
				��5������

	�洢��ʽ��	ѹ���Ժ��������䣬�������룬�����ڲ�����ʱ��x���겻�䣬z�������2��
				���Ƕ��ⲿ��˵��͸����

	+-------------+
	| \    |    / |
	|  \   |   /  |
	|   \  |  /   |
	|------+------|   Z
	|   /  |  \   |   /\
	|  /   |   \  |   |
	| /    |    \ |   |
	+-------------+   +---->X
	-----------------------------------------------------------------------------
	*/
	class ECORE_API HeightFieldData
	{
		/*
		--------------------------------
		�ļ�ͷ
		--------------------------------
		*/
		struct tagFileHeader
		{
			INT				width;
			INT				height;
			FLOAT			xScale;
			FLOAT			yScale;
			FLOAT			zScale;
		};

	public:
		/*
		--------------------------------
		����/����
		--------------------------------
		*/
		INLINE				HeightFieldData(void) : m_pData(0) {}
		INLINE				~HeightFieldData(void) { Destroy(); }

		/*
		--------------------------------
		����/�ͷ�
		--------------------------------
		*/
		VOID				LoadFromFile(VFS* pFS, LPCSTR szPath);
		VOID				Destroy();

		/*
		--------------------------------
		���ù���
		--------------------------------
		*/
		INLINE INT			Width()		{ return m_width; }
		INLINE INT			Height()	{ return m_height; }

		INLINE FLOAT		SafeGetHeight(INT x, INT z);

		INLINE Point		World2Tile(FLOAT x, FLOAT z);
		INLINE Vector3		SafeTile2World(INT x, INT z);

		INLINE VOID			ClampPos(INT& x, INT& z);
		INLINE VOID			ClampPos(Point& p);

		/*
		--------------------------------
		
		--------------------------------
		*/
		VOID				CopyTo(HeightFieldData& out) const;

		/*
		--------------------------------
		Editor
		--------------------------------
		*/
		VOID				Build(const Heightmap& hmap, INT gridSize);
		BOOL				Save(LPCSTR szPath);

	protected:
		INLINE INT			GetIndex(INT x, INT z);

	protected:
		INT					m_width;
		INT					m_height;
		FLOAT				m_xScale;
		FLOAT				m_yScale;
		FLOAT				m_zScale;
		WORD*				m_pData;
	};

	//---------------------------------------------------------------------------
	INLINE FLOAT HeightFieldData::SafeGetHeight( INT x, INT z )
	{
		ClampPos( x, z );

		ASSERT( m_pData );
		return m_pData[GetIndex(x, z)] * m_yScale;
	}

	//---------------------------------------------------------------------------
	INLINE Point HeightFieldData::World2Tile(FLOAT x, FLOAT z)
	{
		return Point( LONG(x / m_xScale), LONG(z / m_zScale) );
	}

	//---------------------------------------------------------------------------
	INLINE Vector3 HeightFieldData::SafeTile2World(INT x, INT z)
	{
		return Vector3( x * m_xScale,
						SafeGetHeight( x, z ),
						z * m_zScale );
	}

	//---------------------------------------------------------------------------
	INLINE VOID HeightFieldData::ClampPos(INT& x, INT& z)
	{
		if( x < 0 )				x = 0;
		if( x >= m_width )		x = m_width - 1;
		if( z < 0 )				z = 0;
		if( z >= m_height )		z = m_height - 1;
	}

	//---------------------------------------------------------------------------
	INLINE VOID	HeightFieldData::ClampPos(Point& p)
	{
		if( p.x < 0 )			p.x = 0;
		if( p.x >= m_width )	p.x = m_width - 1;
		if( p.y < 0 )			p.y = 0;
		if( p.y >= m_height )	p.y = m_height - 1;
	}

	//---------------------------------------------------------------------------
	INLINE INT HeightFieldData::GetIndex(INT x, INT z)
	{
		return ( z >> 1 ) * m_width + x;
	}
}//namespace ECore