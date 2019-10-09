#include "stdafx.h"
#include "height_map.h"

namespace ECore
{
	Heightmap::Heightmap(void)
		: m_xScale(1.0f)
		, m_zScale(1.0f)
		, m_yScale(1.0f)
		, m_heightMap8(NULL)
		, m_heightMap16(NULL)
		, m_hmapWidth(0)
		, m_hmapHeight(0)
		, m_startPos(0,0)
		, m_format(EF_8Bit)
	{}

	Heightmap::~Heightmap(void)
	{
		Destroy();
	}

	void Heightmap::Destroy()
	{
		if( m_heightMap8 )
			MCFREE(m_heightMap8);
		if( m_heightMap16 )
			MCFREE(m_heightMap16);

		m_heightMap8=NULL;
		m_heightMap16=NULL;

		m_xScale=1.0f;	
		m_zScale=1.0f;
		m_yScale=1.0f;
		m_hmapWidth=0;
		m_hmapHeight=0;
		m_startPos=Vector2(0,0);
	}

	Heightmap *Heightmap::NewSubMap(Rect rect) const
	{
		Heightmap *pNewMap=new Heightmap;
		pNewMap->m_xScale=m_xScale;
		pNewMap->m_yScale=m_yScale;
		pNewMap->m_zScale=m_zScale;
		pNewMap->CreateData(rect.Width(),rect.Height(),m_format);
		pNewMap->m_startPos.x=rect.left*m_xScale;
		pNewMap->m_startPos.y=rect.top*m_zScale;

		for(int y=0;y<pNewMap->m_hmapHeight;y++)
		{
			for(int x=0;x<pNewMap->m_hmapWidth;x++)
			{
				int by=y+rect.top;
				int bx=x+rect.left;
				pNewMap->SetValue(x,y,GetValue(bx,by));
			}
		}//endof for
		return pNewMap;
	}

	void Heightmap::SetSubMap( Heightmap *pSubMap,Rect& rect )
	{
		for(int y=0;y<pSubMap->m_hmapHeight;y++)
		{
			for(int x=0;x<pSubMap->m_hmapWidth;x++)
			{
				int by=y+rect.top;
				int bx=x+rect.left;
				SetValue(bx,by,pSubMap->GetValue(x,y));
			}
		}
	}

	void Heightmap::AutoCenterStartPos()
	{
		m_startPos.x=-0.5f*m_hmapWidth*m_xScale;
		m_startPos.y=-0.5f*m_hmapHeight*m_zScale;
	}

	void Heightmap::CreateData(int width,int height,EFormat format)
	{
		ASSERT(m_heightMap8==NULL && m_heightMap16==NULL);
		ASSERT(width > 0);
		ASSERT(height > 0);

		m_hmapWidth=width;
		m_hmapHeight=height;
		m_format=format;

		int numPixel=m_hmapWidth*m_hmapHeight;
		if(format==EF_8Bit)
			m_heightMap8=(BYTE*)MCALLOC(numPixel*sizeof(BYTE));
		else
			m_heightMap16=(WORD*)MCALLOC(numPixel*sizeof(WORD));
	}

	bool Heightmap::WriteToFile(FILE *fp) const
	{
		ASSERT(fp!=NULL);

		fwrite(&m_format,sizeof(EFormat),1,fp);
		fwrite(&m_xScale,sizeof(float),1,fp);
		fwrite(&m_yScale,sizeof(float),1,fp);
		fwrite(&m_zScale,sizeof(float),1,fp);
		fwrite(&m_startPos,sizeof(m_startPos),1,fp);
		fwrite(&m_hmapWidth,sizeof(int),1,fp);
		fwrite(&m_hmapHeight,sizeof(int),1,fp);
		fwrite(&m_format,sizeof(EFormat),1,fp);
		if(m_format==EF_8Bit)
			fwrite(m_heightMap8,m_hmapWidth*m_hmapHeight*sizeof(BYTE),1,fp);
		else
			fwrite(m_heightMap16,m_hmapWidth*m_hmapHeight*sizeof(WORD),1,fp);

		return true;
	}

	bool Heightmap::WriteToFile( LPCSTR szPath ) const
	{
		SetDefaultDir();

		FILE *fp = fopen( szPath, "wb" );
		if(fp == NULL)
			return false;

		WriteToFile( fp );
		fclose(fp);

		return true;
	}

	bool Heightmap::ReadFromFile(VFS* pFS,HANDLE hFile)
	{
		ASSERT(pFS!=NULL && hFile!=NULL);

		Destroy();

		//--
		pFS->Read(hFile,&m_format,sizeof(EFormat));
		pFS->Read(hFile,&m_xScale,sizeof(float));
		pFS->Read(hFile,&m_yScale,sizeof(float));
		pFS->Read(hFile,&m_zScale,sizeof(float));
		pFS->Read(hFile,&m_startPos,sizeof(m_startPos));
		pFS->Read(hFile,&m_hmapWidth,sizeof(int));
		pFS->Read(hFile,&m_hmapHeight,sizeof(int));
		pFS->Read(hFile,&m_format,sizeof(EFormat));
		CreateData(m_hmapWidth,m_hmapHeight,m_format);
		if(m_format==EF_8Bit)
			pFS->Read(hFile,m_heightMap8,m_hmapWidth*m_hmapHeight*sizeof(BYTE));
		else
			pFS->Read(hFile,m_heightMap16,m_hmapWidth*m_hmapHeight*sizeof(WORD));

		//m_yScale=48;//test
		return true;
	}

	void Heightmap::Serialize(FArchive& ar)
	{
		ar<<m_xScale;
		ar<<m_yScale;
		ar<<m_zScale;
		ar<<m_startPos;
		ar<<m_hmapWidth;
		ar<<m_hmapHeight;
		ar<<m_format;
		if(m_format==EF_8Bit)
			ar.BufferWrite(m_heightMap8,m_hmapWidth*m_hmapHeight*sizeof(BYTE));
		else
			ar.BufferWrite(m_heightMap16,m_hmapWidth*m_hmapHeight*sizeof(WORD));
	}

	void Heightmap::Deserialize(FArchive& ar)
	{
		Destroy();
		ar>>m_xScale;
		ar>>m_yScale;
		ar>>m_zScale;
		ar>>m_startPos;
		ar>>m_hmapWidth;
		ar>>m_hmapHeight;
		int tmp;
		ar>>tmp;
		m_format=(EFormat)tmp;
		CreateData(m_hmapWidth,m_hmapHeight,m_format);
		if(m_format==EF_8Bit)
			ar.BufferRead(m_heightMap8,m_hmapWidth*m_hmapHeight*sizeof(BYTE));
		else
			ar.BufferRead(m_heightMap16,m_hmapWidth*m_hmapHeight*sizeof(WORD));
	}

	void Heightmap::Create(int initVal,int width,int height,float xScale,float zScale,float yScale,EFormat format, bool bSetInitVal)
	{
		CreateData(width,height,format);
		m_xScale=xScale;
		m_zScale=zScale;
		m_yScale=yScale;

		if( bSetInitVal )
		{
			int i=0;
			if(format==EF_8Bit)
			{
				for(int y=0;y<height;y++)
				{
					for(int x=0;x<width;x++)
					{
						m_heightMap8[i++]=initVal;
					}
				}
			}
			else
			{
				for(int y=0;y<height;y++)
				{
					for(int x=0;x<width;x++)
					{
						m_heightMap16[i++]=initVal;
					}
				}		
			}
		}
	}

	void Heightmap::GetVertNormal(int x,int z,Vector3& out) const
	{
		//--see <<Game Programming Gems 3>>,4.2,Fast Heightfield normal cal
		/*
		 0,0
					h2
				  |
				  |
		  h3______h______h1
				  |
				  |
				   h4
		*/
		out.x=(GetSafeHeight(x-4,z)-GetSafeHeight(x+4,z));//((h1-h3)
		out.z=(GetSafeHeight(x,z-4)-GetSafeHeight(x,z+4));//(h2-h4)
		out.y=8.0f*m_xScale;
		D3DXVec3Normalize(&out,&out);

		//out=Vector3(0,1,0);//test
	}

	void Heightmap::GetVertTangent( int x,int z,Vector3& out,const Vector3* pTangentRotate ) const
	{
		out.x=8.0f*m_xScale;
		out.y=(GetSafeHeight(x+4,z)-GetSafeHeight(x-4,z));
		out.z=0.0f;
		D3DXVec3Normalize(&out,&out);

		if( pTangentRotate )
		{
			Transform trans;
			trans.Rotate( pTangentRotate->y, pTangentRotate->x, pTangentRotate->z );

			trans.TransDir( out );
			Vec3Normalize( out );
		}

		//out=Vector3(1,0,0);//test
	}

	void Heightmap::GetHeightRange(float& minh,float &maxh,Rect rect) const
	{
		int minH=0;
		int maxH=0;
		switch( m_format )
		{
		case EF_8Bit:
			minH=255;
			maxH=-255;
			break;
		case EF_16Bit:
			minH=65535;
			maxH=-65535;
			break;
		default:
			ASSERT("Invalid EFormat!");
		}

		for(int z=rect.top;z<rect.bottom;z++)
		{
			for(int x=rect.left;x<rect.right;x++)
			{
				int h=GetValue(x,z);
				if(h<minH)
					minH=h;
				if(h>maxH)
					maxH=h;
			}
		}
		minh=minH*m_yScale;
		maxh=maxH*m_yScale;
	}

	void Heightmap::GetHeightRange( int& minh,int &maxh,Rect rect ) const
	{
		switch( m_format )
		{
		case EF_8Bit:
			minh=255;
			maxh=-255;
			break;
		case EF_16Bit:
			minh=65535;
			maxh=-65535;
			break;
		default:
			ASSERT("Invalid EFormat!");
		}

		for(int z=rect.top;z<rect.bottom;z++)
		{
			for(int x=rect.left;x<rect.right;x++)
			{
				int h=GetValue(x,z);
				if(h<minh)
					minh=h;
				if(h>maxh)
					maxh=h;
			}
		}
	}

	void Heightmap::BuildAABBox(AABBox *pBox,const Rect* pRect) const
	{
		ASSERT(pBox!=NULL);
		if(pRect==NULL)
		{
			Rect rect;
			rect.left=rect.top=0;
			rect.right=m_hmapWidth;
			rect.bottom=m_hmapHeight;
			pBox->_min.x=m_startPos.x;
			pBox->_min.z=m_startPos.y;
			pBox->_max.x=m_startPos.x+m_xScale*m_hmapWidth;
			pBox->_max.z=m_startPos.y+m_zScale*m_hmapHeight;
			GetHeightRange(pBox->_min.y,pBox->_max.y,rect);
		}
		else
		{
			pBox->_min.x=m_startPos.x+m_xScale*pRect->left;
			pBox->_min.z=m_startPos.y+m_zScale*pRect->top;
			pBox->_max.x=pBox->_min.x+m_xScale*(pRect->right-pRect->left);
			pBox->_max.z=pBox->_min.z+m_zScale*(pRect->bottom-pRect->top);
			GetHeightRange(pBox->_min.y,pBox->_max.y,*pRect);
		}
	}

	int Heightmap::GetSafeVal(int x,int z) const
	{
		if(x<0)
			x=0;
		if(x>(int)m_hmapWidth-1)
			x=m_hmapWidth-1;
		if(z<0)
			z=0;
		if(z>(int)m_hmapHeight-1)
			z=m_hmapHeight-1;
		return GetValue(x,z);
	}

	void Heightmap::SafeAddValue(int x,int z,int& val)
	{
		if(!IfIndex(x,z))
			return;
		int index=GetIndex(x,z);
		if(m_format==EF_8Bit)
		{
			int oldVal=m_heightMap8[index];
			int newVal=oldVal+val;
			if(newVal<0)
				newVal=0;
			if(newVal>255)
				newVal=255;

			val=newVal-oldVal;
			m_heightMap8[index]=newVal;
		}
		else
		{
			int oldVal=m_heightMap16[index];
			int newVal=oldVal+val;
			if(newVal<0)
				newVal=0;
			if(newVal>65535)
				newVal=65535;

			val=newVal-oldVal;
			m_heightMap16[index]=newVal;
		}
	}
	void Heightmap::SafeSetVal(int x,int z,int& val)
	{
		if(!IfIndex(x,z))
			return;
		if(m_format==EF_8Bit)
		{
			if(val<0)
				val=0;
			if(val>255)
				val=255;
			m_heightMap8[GetIndex(x,z)]=val;
		}
		else
		{
			if(val<0)
				val=0;
			if(val>65535)
				val=65535;
			m_heightMap16[GetIndex(x,z)]=val;
		}
	}

	bool Heightmap::RayCollision(const Ray& ray,Vector3& pt,float maxLength,bool bAutoGroundHeight) const
	{
		float step=EMin(m_xScale,m_zScale);
		step*=0.5f;

		Vector3 linePt;
		Point mapPt=World2Tile(ray.origin);
		if(IfIndex(mapPt.x,mapPt.y) &&
			GetHeight(mapPt.x,mapPt.y) > ray.origin.y)//如果ray起点位于地形下面,那么返回false
			step=-step;
		float h;
		for(float len=0;len<maxLength&&len>-maxLength;len+=step)
		{
			linePt=ray.dir*len+ray.origin;
			mapPt=World2Tile(linePt);
			if(IfIndex(mapPt.x,mapPt.y))//如果在heightmap范围内
			{
				h=GetHeight(mapPt.x,mapPt.y);
				if(linePt.y<h && step>0)//如果射线上这个点已经低于地面了
				{
					pt=linePt;
					if(bAutoGroundHeight)
						pt.y=h;
					return true;
				}
				if (linePt.y>h && step<0)
				{
					pt=linePt;
					if(bAutoGroundHeight)
						pt.y=h;
					return true;
				}
			}
		}
		return false;
	}

	void Heightmap::CopyTo(Heightmap& out) const
	{
		ASSERT( IsCreated() );

		out.Create(0,m_hmapWidth,m_hmapHeight,m_xScale,m_zScale,m_yScale,m_format,false);
		out.m_startPos=m_startPos;
		if(m_format==EF_8Bit)
		{
			ASSERT(out.m_heightMap8!=NULL);
			memcpy(out.m_heightMap8,m_heightMap8,sizeof(BYTE)*m_hmapWidth*m_hmapHeight);
		}
		else
		{
			ASSERT(out.m_heightMap16!=NULL);
			memcpy(out.m_heightMap16,m_heightMap16,sizeof(WORD)*m_hmapWidth*m_hmapHeight);
		}
	}

	void Heightmap::ConvertTo16Bit()
	{
		if(m_format!=EF_8Bit)
			return;

		ASSERT(m_heightMap8!=NULL);
		ASSERT(m_heightMap16==NULL);

		int numPixel=m_hmapWidth*m_hmapHeight;
		m_heightMap16=(WORD*)MCALLOC(numPixel*sizeof(WORD));
		for(int i=0;i<numPixel;i++)
		{
			m_heightMap16[i]=WORD(m_heightMap8[i]*m_yScale);
		}

		m_yScale=1;
		m_format=EF_16Bit;

		SAFE_MCFREE(m_heightMap8);
	}

	void Heightmap::ClampRect( Rect& rc )
	{
		if(rc.left<0)rc.left=0;
		if(rc.right>=m_hmapWidth)rc.right=m_hmapWidth-1;
		if(rc.top<0)rc.top=0;
		if(rc.bottom>=m_hmapHeight)rc.bottom=m_hmapHeight-1;
	}

	void Heightmap::Fill( int val )
	{
		if(m_format==EF_8Bit)
		{
			int numByte=m_hmapWidth*m_hmapHeight;
			memset(m_heightMap8,val,numByte);
		}
		else if(m_format==EF_16Bit)
		{
			int numByte=m_hmapWidth*m_hmapHeight*sizeof(WORD);
			memset(m_heightMap16,val,numByte);
		}
	}

	//-----------------------------------------------------------------------------
	void Heightmap::BuildHeightField( INT gridSize, Heightmap& out )
	{
		ASSERT( IsCreated() );

		ASSERT( Width()  % gridSize == 0 );
		ASSERT( Height() % gridSize == 0 );
		INT w = Width()	 / gridSize;
		INT h = Height() / gridSize;
		out.Create( 0, w, h, 
			m_xScale * (FLOAT)gridSize, 
			m_zScale * (FLOAT)gridSize,
			m_yScale,
			m_format);

		for( INT y=0; y<h; y++ )
		{
			for( INT x=0; x<w; x++ )
			{
				INT val = GetSafeVal( x * gridSize, y * gridSize );
				out.SafeSetVal( x, y, val );
			}
		}
	}
}//namespace ECore
