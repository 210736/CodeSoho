/*
---------------------------------------------------------------------------------
Auth: wb
Date: 2012/11/20
---------------------------------------------------------------------------------
*/
#pragma once

#include "height_field_data.h"

namespace ECore
{
	/*
	-----------------------------------------------------------------------------
	高度场
	-----------------------------------------------------------------------------
	*/
	class ECORE_API HeightField
	{
	public:
		/*
		--------------------------------
		构造/析构
		--------------------------------
		*/
		INLINE				HeightField(void)	{}
		INLINE				~HeightField(void)	{}

		/*
		--------------------------------
		加载/释放
		--------------------------------
		*/
		VOID				LoadFromFile(VFS* pFS, LPCSTR szPath);
		VOID				Destroy();

		/*
		--------------------------------
		获取高度/法线
		--------------------------------
		*/
		INLINE FLOAT		GetHeight(FLOAT x, FLOAT z);
		INLINE VOID			GetNormal(FLOAT x, FLOAT z, OUT Vector3& out);

		/*
		--------------------------------
		
		--------------------------------
		*/
		VOID				Build(const Heightmap& hmap, INT gridSize);
		VOID				CopyTo(HeightField& out) const;

	protected:
		/*
		--------------------------------
		
		--------------------------------
		*/
		INT					GetGridTypeAndVerts(FLOAT x, FLOAT z, OUT Vector3* pVerts);

		FLOAT				GetHeightInGridType0(FLOAT x, FLOAT z, Vector3* pVerts);
		FLOAT				GetHeightInGridType1(FLOAT x, FLOAT z, Vector3* pVerts);

		VOID				GetNormalInGridType0(FLOAT x, FLOAT z, Vector3* pVerts, OUT Vector3& out);
		VOID				GetNormalInGridType1(FLOAT x, FLOAT z, Vector3* pVerts, OUT Vector3& out);

	protected:
		HeightFieldData		m_data;
	};

	//---------------------------------------------------------------------------
	INLINE FLOAT HeightField::GetHeight( FLOAT x, FLOAT z )
	{
		Vector3 verts[4];
		if( GetGridTypeAndVerts( x, z, verts ) == 0 )
			return GetHeightInGridType0( x, z, verts );
		else 
			return GetHeightInGridType1( x, z, verts );
	}

	//---------------------------------------------------------------------------
	INLINE VOID HeightField::GetNormal( FLOAT x, FLOAT z, OUT Vector3& out )
	{
		Vector3 verts[4];
		if( GetGridTypeAndVerts( x, z, verts ) == 0 )
			GetNormalInGridType0( x, z, verts, out ) ;
		else 
			GetNormalInGridType1( x, z, verts, out );
	}
}//namespace ECore