/*
---------------------------------------------------------------------------------
Auth: wb
Date: 2012/12/3
---------------------------------------------------------------------------------
*/
#include "StdAfx.h"
#include "height_field_data.h"
#include "height_map.h"

namespace ECore
{
	//---------------------------------------------------------------------------
	VOID HeightFieldData::LoadFromFile( VFS* pFS, LPCSTR szPath )
	{
		HANDLE hFile = pFS->Open( szPath );
		if( !VALID(hFile) )
			THROW( _T("open file %s failed"), SS::To16(szPath) );

		// 文件头
		tagFileHeader header;
		pFS->Read( hFile, &header, sizeof(header) );
		m_width		= header.width;
		m_height	= header.height;
		m_xScale	= header.xScale;
		m_yScale	= header.yScale;
		m_zScale	= header.zScale;
		
		// 高度数据
		ASSERT( !m_pData );	

		INT bytes = m_width * m_height / 2 * sizeof(WORD);
		m_pData = (WORD*)MCALLOC( bytes );
		pFS->Read( hFile, m_pData, bytes );

		pFS->Close(hFile);
	}

	//---------------------------------------------------------------------------
	VOID HeightFieldData::Destroy()
	{
		SAFE_MCFREE(m_pData);
	}

	//---------------------------------------------------------------------------
	VOID HeightFieldData::CopyTo( HeightFieldData& out ) const
	{
		// 基础数据
		out.m_width  = m_width;
		out.m_height = m_height;
		out.m_xScale = m_xScale;
		out.m_yScale = m_yScale;
		out.m_zScale = m_zScale;

		// 高度数据
		ASSERT( m_pData );
		ASSERT( !out.m_pData );

		INT bytes = m_width * m_height / 2;
		out.m_pData = (WORD*)MCALLOC( bytes );
		memcpy( out.m_pData, m_pData, bytes );
	}

	//---------------------------------------------------------------------------
	VOID HeightFieldData::Build( const Heightmap& hmap, INT gridSize )
	{
		ASSERT( hmap.IsCreated() );
		ASSERT( hmap.Width()  % gridSize == 0 );
		ASSERT( hmap.Height() % gridSize == 0 );

		// 初始化基础数据
		{
			m_width  = hmap.Width()  / gridSize;
			m_height = hmap.Height() / gridSize;
			m_xScale = hmap.GetXScale() * (FLOAT)gridSize;
			m_zScale = hmap.GetZScale() * (FLOAT)gridSize;
			m_yScale = hmap.GetYScale();
		}

		// 拷贝高度数据
		{
			ASSERT( !m_pData );
			m_pData = (WORD*)MCALLOC( m_width * m_height * sizeof(WORD) );

			for( INT z = 0; z < m_height; z++ )
			{
				for( INT x = 0; x < m_width; x++ )
				{
					m_pData[ z * m_width + x ] = hmap.GetValue( x * gridSize, z * gridSize );
				}
			}
		}

		// 压缩高度数据
		{
			INT destH = m_height / 2;
			for( INT z = 0; z < destH; z++ )
			{
				for( INT x = 0; x < m_width; x++ )
				{
					INT srcZ = x % 2 ? z * 2 + 1 : z * 2;
					m_pData[ z * m_width + x ] = m_pData[ srcZ * m_width + x ];
				}
			}

			m_pData = (WORD*)MCREALLOC( m_pData, m_width * destH * sizeof(WORD) );
		}
	}

	//---------------------------------------------------------------------------
	BOOL HeightFieldData::Save( LPCSTR szPath )
	{
		// 打开文件
		FILE* fp = fopen( szPath, "wb" );
		if( !fp )
			return FALSE;

		// 文件头
		tagFileHeader header;
		header.width	= m_width;
		header.height	= m_height;
		header.xScale	= m_xScale;
		header.yScale	= m_yScale;
		header.zScale	= m_zScale;
		fwrite( &header, sizeof(header), 1, fp );

		// 高度数据
		ASSERT( m_pData );

		INT bytes = m_width * m_height / 2 * sizeof(WORD);
		fwrite( m_pData, bytes, 1, fp );

		fclose(fp);
		return TRUE;
	}
}//namespace ECore