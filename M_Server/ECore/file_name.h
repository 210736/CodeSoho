#pragma once

namespace ECore
{
	/** \class Filename
		\brief 文件名路径解析
	*/
	class Filename : public string
	{
	public:
		Filename()                                        { }
		Filename(LPCSTR pFilename) : string(pFilename)    { }
		Filename(string strFilename) :string(strFilename) { }

		INLINE Filename GetExtension(void);      // if filename is "/idv/code/file.cpp", it returns "cpp"
		INLINE Filename GetPath(void);           // if filename is "/idv/code/file.cpp", it returns "/idv/code"
		INLINE Filename NoExtension(void);       // if filename is "/idv/code/file.cpp", it returns "/idv/code/file"
		INLINE Filename NoPath(void);            // if filename is "/idv/code/file.cpp", it returns "file.cpp"

		INLINE LPCSTR	GetExtension(LPSTR pExt, INT size);      

	};


	///////////////////////////////////////////////////////////////////////  
	//  Filename::GetExtension
	Filename Filename::GetExtension(void)
	{
		int nLength = (int)length( );
		for (int i = nLength - 1; i > 0 && (*this)[i] != '/' && (*this)[i] != '\\'; i--)
		{
			if ((*this)[i] == '.')
			{
				return Filename( c_str( ) + i + 1 );
			}
		}

		return Filename( "" );
	}

	///////////////////////////////////////////////////////////////////////  
	//  Filename::GetExtension
	LPCSTR Filename::GetExtension(LPSTR pExt, INT size)
	{
		int nLength = (int)length( );
		for (int i = nLength - 1; i > 0 && (*this)[i] != '/' && (*this)[i] != '\\'; i--)
		{
			if ((*this)[i] == '.')
			{
				strcpy_s( pExt, size, c_str( ) + i + 1 );
				return pExt;
			}
		}

		*pExt = '\0';
		return pExt;
	}

	///////////////////////////////////////////////////////////////////////  
	//  Filename::GetPath
	Filename Filename::GetPath(void)
	{
		CHAR szPath[HUGE_STRING];

		szPath[0] = '\0';
		int nLength = (int)length( );
		for (int i = nLength - 1; i >= 0; i--)
		{
			if ((*this)[i] == '/' || (*this)[i] == '\\')
			{
				int j = 0;
				for (j = 0; j < i + 1; j++)
					szPath[j] = (*this)[j];
				szPath[j] = '\0';
				break;
			}
		}

		return Filename(szPath);
	}

	///////////////////////////////////////////////////////////////////////  
	//  Filename::NoExtension
	Filename Filename::NoExtension(void)
	{
		CHAR szExtension[HUGE_STRING];
		strcpy_s(szExtension, sizeof(szExtension)/sizeof(CHAR), c_str( ));

		int nLength = (int)length( );
		for( int i = nLength - 1; i >= 0 && szExtension[i] != '/' && szExtension[i] != '\\'; i-- )
		{
			if( szExtension[i] == '.')
			{
				szExtension[i] = '\0';
				break;
			}
		}

		return Filename(szExtension);
	}


	///////////////////////////////////////////////////////////////////////  
	//  Filename::NoPath
	Filename Filename::NoPath(void)
	{
		CHAR szPath[HUGE_STRING];
		int nLength = (int)length( );

		for( int i = nLength - 1; i >= 0; i-- )
		{
			if ((*this)[i] == '/' || (*this)[i] == '\\')
			{
				int j, k;
				for (j = i + 1, k = 0; j < nLength; j++, k++)
					szPath[k] = (*this)[j];
				szPath[k] = '\0';
				break;
			}
		}

		return string(szPath);
	}

}//endof namespace ECore