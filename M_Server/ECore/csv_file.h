#pragma once

#include "simple_vector.h"

namespace ECore
{
	/** \class CSVFile
		\brief CSV�ı��ļ���ȡ��
	*/
	class ECORE_API CSVFile
	{
	public:
		CSVFile():m_pBuffer(0), m_nCharNum(0), m_nCharIndex(0) {}
		~CSVFile() { CloseFile(); }

		bool OpenFile(VFS *pFS, LPCWSTR szFileName);
		bool OpenFile(VFS *pFS, LPCSTR szFileName);
		
		void BeginReadLine();
		BOOL GetNextLine(vector<wstring>& vec);
		BOOL GetNextLine(vector<string>& vec);
		BOOL GetNextLine(SimpleVector<LPCSTR>& vec);
		void CloseFile();

	private:
		LPVOID	m_pBuffer;
		int		m_nCharNum;	// �ַ���(������0������)
		int		m_nCharIndex;
	};
}//namespace ECore