/*
---------------------------------------------------------------------------------
Auth: wb
Date: 2012/11/20
---------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "height_field.h"

namespace ECore
{
	//---------------------------------------------------------------------------
	VOID HeightField::LoadFromFile( VFS* pFS, LPCSTR szPath )
	{
		m_data.LoadFromFile( pFS, szPath );
	}

	//---------------------------------------------------------------------------
	VOID HeightField::Destroy()
	{
		m_data.Destroy();
	}

	//---------------------------------------------------------------------------
	//	+-------------+
	//  | \    |    / |
	//  | Type1| Type0|
	//  |   \  |  /   |
	//  |------+------|   Z
	//  |   /  |  \   |   /\
	//  | Type0| Type1|   |
	//  | /    |    \ |   |
	//  +-------------+   +---->X
	//---------------------------------------------------------------------------
	INT HeightField::GetGridTypeAndVerts( FLOAT x, FLOAT z, OUT Vector3* pVerts )
	{
		Point tile = m_data.World2Tile(x, z);
		m_data.ClampPos( tile );

		INT xType = tile.x % 2;
		INT zType = tile.y % 2;

		// 	2--------3    
		//  |     /  |    Z
		//  |    /   |    /\
		//  |   /    |    |
		//  |  /     |    |
		//  0--------1    +---->X	
		pVerts[0] = m_data.SafeTile2World( tile.x,		tile.y	 );
		pVerts[1] = m_data.SafeTile2World( tile.x+1,	tile.y	 );
		pVerts[2] = m_data.SafeTile2World( tile.x,		tile.y+1 );
		pVerts[3] = m_data.SafeTile2World( tile.x+1,	tile.y+1 );

		if( xType == 0 )
		{
			if( zType == 0 ) 
			{
				//	+-------------+
				//  | \    |    / |
				//  |  \   |   /  |
				//  |   \  |  /   |
				//  2------3------|   Z
				//  |   /  |  \   |   /\
				//  | Type0|   \  |   |
				//  | /    |    \ |   |
				//  0------1------+   +---->X
				pVerts[1].y = ( pVerts[0].y + m_data.SafeGetHeight( tile.x + 2, tile.y     ) ) * 0.5f;
				pVerts[2].y = ( pVerts[0].y + m_data.SafeGetHeight( tile.x,		tile.y + 2 ) ) * 0.5f;
				return 0;	
			}
			else			
			{
				//	2------3------+
				//  | \    |    / |
				//  | Type1|   /  |
				//  |   \  |  /   |
				//  0------1------|   Z
				//  |   /  |  \   |   /\
				//  |  /   |   \  |   |
				//  | /    |    \ |   |
				//  +-------------+   +---->X
				pVerts[0].y = ( pVerts[2].y + m_data.SafeGetHeight( tile.x,		tile.y - 1 ) ) * 0.5f;
				pVerts[3].y = ( pVerts[2].y + m_data.SafeGetHeight( tile.x + 2, tile.y + 1 ) ) * 0.5f;
				return 1;	
			}
		}
		else
		{
			if( zType == 0 ) 
			{
				//	+-------------+
				//  | \    |    / |
				//  |  \   |   /  |
				//  |   \  |  /   |
				//  |------2------3   Z
				//  |   /  |  \   |   /\
				//  |  /   | Type1|   |
				//  | /    |    \ |   |
				//  +------0------1   +---->X
				pVerts[0].y = ( pVerts[1].y + m_data.SafeGetHeight( tile.x - 1, tile.y	   ) ) * 0.5f;
				pVerts[3].y = ( pVerts[1].y + m_data.SafeGetHeight( tile.x + 1, tile.y + 2 ) ) * 0.5f;
				return 1;	
			}
			else			
			{
				//	+------2------3
				//  | \    |    / |
				//  |  \   | Type0|
				//  |   \  |  /   |
				//  |------0------1   Z
				//  |   /  |  \   |   /\
				//  |  /   |   \  |   |
				//  | /    |    \ |   |
				//  +-------------+   +---->X
				pVerts[1].y = ( pVerts[3].y + m_data.SafeGetHeight( tile.x + 1, tile.y - 1 ) ) * 0.5f;
				pVerts[2].y = ( pVerts[3].y + m_data.SafeGetHeight( tile.x - 1, tile.y + 1 ) ) * 0.5f;
				return 0;	
			}
		}
	}

	//---------------------------------------------------------------------------
	// Type0:
	// 	2--------3    
	//  | x<z /  |    Z
	//  |    /   |    /\
	//  |   /    |    |
	//  |  / x>z |    |
	//  0--------1    +---->X	
	//---------------------------------------------------------------------------
	FLOAT HeightField::GetHeightInGridType0( FLOAT x, FLOAT z, Vector3* pVerts )
	{
		x = SmoothStep( x, pVerts[0].x, pVerts[1].x );
		z = SmoothStep( z, pVerts[0].z, pVerts[2].z );
		
		if( x < z )
		{
			FLOAT v1h = pVerts[0].y + ( pVerts[3].y - pVerts[2].y );

			FLOAT h0 = LinearLerp( pVerts[0].y, v1h,		 x );
			FLOAT h1 = LinearLerp( pVerts[2].y, pVerts[3].y, x );
			FLOAT h	 = LinearLerp( h0, h1, z );
			return h;
		}
		else
		{
			FLOAT v2h = pVerts[3].y + ( pVerts[0].y - pVerts[1].y );

			FLOAT h0 = LinearLerp( pVerts[0].y, pVerts[1].y, x );
			FLOAT h1 = LinearLerp( v2h,			pVerts[3].y, x );
			FLOAT h	 = LinearLerp( h0, h1, z );
			return h;
		}
	}

	//---------------------------------------------------------------------------
	// Type1:
	// 	2--------3    
	//  |  \x>z' |    Z
	//  |   \    |    /\
	//  |    \   |    |
	//  |x<z' \  |    |
	//  0--------1    +---->X
	//---------------------------------------------------------------------------
	FLOAT HeightField::GetHeightInGridType1( FLOAT x, FLOAT z, Vector3* pVerts )
	{
		x = SmoothStep( x, pVerts[0].x, pVerts[1].x );
		z = SmoothStep( z, pVerts[0].z, pVerts[2].z );

		if( x < 1.f - z )
		{
			FLOAT v3h = pVerts[2].y + ( pVerts[1].y - pVerts[0].y );

			FLOAT h0 = LinearLerp( pVerts[0].y, pVerts[1].y, x );
			FLOAT h1 = LinearLerp( pVerts[2].y, v3h,		 x );
			FLOAT h	 = LinearLerp( h0, h1, z );
			return h;
		}
		else
		{
			FLOAT v0h = pVerts[1].y + ( pVerts[2].y - pVerts[3].y );

			FLOAT h0 = LinearLerp( v0h,			pVerts[1].y, x );
			FLOAT h1 = LinearLerp( pVerts[2].y, pVerts[3].y, x );
			FLOAT h	 = LinearLerp( h0, h1, z );
			return h;
		}
	}

	//---------------------------------------------------------------------------
	// Type0:
	// 	2--------3    
	//  | x<z /  |    Z
	//  |    /   |    /\
	//  |   /    |    |
	//  |  / x>z |    |
	//  0--------1    +---->X	
	//---------------------------------------------------------------------------
	VOID HeightField::GetNormalInGridType0( FLOAT x, FLOAT z, Vector3* pVerts, OUT Vector3& out )
	{
		x = Clamp( x, pVerts[0].x, pVerts[1].x ) - pVerts[0].x;
		z = Clamp( z, pVerts[0].z, pVerts[2].z ) - pVerts[0].z;

		if( x < z )
		{
			Vector3 v1 = pVerts[3] - pVerts[2];
			Vector3 v2 = pVerts[0] - pVerts[2];
			Vec3Cross( &out, &v1, &v2 );
		}
		else
		{
			Vector3 v1 = pVerts[0] - pVerts[1];
			Vector3 v2 = pVerts[3] - pVerts[1];
			Vec3Cross( &out, &v1, &v2 );
		}

		Vec3Normalize( out );
	}

	//---------------------------------------------------------------------------
	// Type1:
	// 	2--------3    
	//  |  \x>z' |    Z
	//  |   \    |    /\
	//  |    \   |    |
	//  |x<z' \  |    |
	//  0--------1    +---->X
	//---------------------------------------------------------------------------
	VOID HeightField::GetNormalInGridType1( FLOAT x, FLOAT z, Vector3* pVerts, OUT Vector3& out )
	{
		x = Clamp( x, pVerts[0].x, pVerts[1].x ) - pVerts[0].x;
		z = Clamp( z, pVerts[0].z, pVerts[2].z ) - pVerts[0].z;
	
		z = ( pVerts[2].z - pVerts[0].z ) - z;

		if( x < z )
		{
			Vector3 v1 = pVerts[2] - pVerts[0];
			Vector3 v2 = pVerts[1] - pVerts[0];
			Vec3Cross( &out, &v1, &v2 );
		}
		else
		{
			Vector3 v1 = pVerts[1] - pVerts[3];
			Vector3 v2 = pVerts[2] - pVerts[3];
			Vec3Cross( &out, &v1, &v2 );
		}

		Vec3Normalize( out );
	}

	//---------------------------------------------------------------------------
	VOID HeightField::CopyTo( HeightField& out ) const
	{
		m_data.CopyTo( out.m_data );
	}

	//---------------------------------------------------------------------------
	VOID HeightField::Build( const Heightmap& hmap, INT gridSize )
	{
		m_data.Build( hmap, gridSize );
	}
}//namespace ECore