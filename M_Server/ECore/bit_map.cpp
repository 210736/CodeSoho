#include "stdafx.h"
#include "bit_map.h"

namespace ECore
{
	const INT US=32;//unit size in bits
	const DWORD HEX1=((unsigned) 0x00000001);

	BitMap::~BitMap()
	{
		Destroy();
	}

	void BitMap::Create( int w,int h,bool initVal, bool bSetInitVal )
	{
		Destroy();

		int numDWords=((w*h)/US)+1;
		m_numBytes=numDWords*US/8;
		m_pData=(DWORD*)MCALLOC(m_numBytes);

		if( bSetInitVal )
		{
			if( initVal )
			{
				memset(m_pData,0xFF,m_numBytes);
			}
			else
			{
				memset(m_pData,0,m_numBytes);
			}
		}

		m_width=w;
		m_height=h;
	}

	void BitMap::Destroy()
	{
		SAFE_MCFREE(m_pData);
		m_width=m_height=0;
		m_numBytes=0;
	}

	INLINE bool BitMap::GetValue( int x,int y ) const
	{
		if( !IfIndex( x, y ) )
			return false;

		int bit=m_width*y+x;
		return (*(m_pData+bit/US) & (HEX1 << (US-1-(bit % US))))!=0;
	}

	void BitMap::SetValue( int x,int y,bool val )
	{
		if( !IfIndex( x, y ) )
			return;

		int bit=m_width*y+x;
		if(val)
		{
			*(m_pData+bit/US) |= (HEX1 << (US-1-(bit % US)));//set bit to 1
		}
		else
		{
			*(m_pData+bit/US) &= (~(HEX1 << (US-1-(bit % US))));//set bit to 0
		}
	}

	bool BitMap::SafeGetValue( int x,int y ) const
	{
		if(x<0)			x=0;
		if(x>=m_width)	x=m_width-1;
		if(y<0)			y=0;
		if(y>=m_height)	y=m_height-1;

		int bit=m_width*y+x;
		return (*(m_pData+bit/US) & (HEX1 << (US-1-(bit % US))))!=0;
	}

	void BitMap::SafeSetValue( int x,int y,bool val )
	{
		if(x<0 || x>=m_width)	return;
		if(y<0 || y>=m_height)	return;

		SetValue(x,y,val);
	}

	void BitMap::WriteToFile( FILE *fp )
	{
		ASSERT(m_pData!=NULL);

		fwrite(&m_width,sizeof(int),1,fp);
		fwrite(&m_height,sizeof(int),1,fp);
		fwrite(m_pData,m_numBytes,1,fp);
	}

	bool BitMap::WriteToFile( LPCSTR szPath )
	{
		SetDefaultDir();

		FILE *fp = fopen( szPath, "wb" );
		if(fp == NULL)
			return false;

		WriteToFile( fp );

		fclose(fp);

		return true;
	}

	void BitMap::ReadFromFile( VFS *pFS,HANDLE hFile )
	{
		pFS->Read(hFile,&m_width,sizeof(int));
		pFS->Read(hFile,&m_height,sizeof(int));

		Create(m_width,m_height,false, false);
		pFS->Read(hFile,m_pData,m_numBytes);
	}

	void BitMap::ReadFromFile( VFS *pFS, LPCSTR szPath )
	{
		HANDLE hFile = pFS->Open( szPath );
		if( !VALID( hFile ) )
			THROW( _T("read file %s failed"), SS::ToW(szPath) );

		ReadFromFile( pFS, hFile );

		pFS->Close( hFile );
	}

	void BitMap::Clone( BitMap *pTarget )
	{
		pTarget->Create(m_width,m_height,1, false);
		ASSERT(m_numBytes==pTarget->m_numBytes);

		memcpy(pTarget->m_pData,m_pData,m_numBytes);
	}

	void BitMap::CopyTo( BitMap& out,int destx,int desty ) const
	{
		ASSERT(destx+m_width<=(int)out.Width());
		ASSERT(desty+m_height<=(int)out.Height());

		int x,y;
		for(y=0;y<m_height;++y)
		{
			for(x=0;x<m_width;++x)
			{
				out.SafeSetValue(x+destx,y+desty,GetValue(x,y));
			}
		}
	}

	void BitMap::CopyTo( BitMap& out,int srcx,int srcy,int w,int h,int destx,int desty ) const
	{
		ASSERT(destx+w<=(int)out.Width() && desty+h<=(int)out.Height());
		ASSERT(srcx+w<=m_width && srcy+h<=m_height);

		int x,y;
		for(y=0;y<h;++y)
		{
			for(x=0;x<w;++x)
			{
				out.SafeSetValue(x+destx,y+desty,SafeGetValue(srcx+x,srcy+y));
			}
		}
	}

	void BitMap::Zero()
	{
		ZeroMemory( m_pData, m_numBytes );
	}
}