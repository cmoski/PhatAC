
#include "StdAfx.h"
#include "BinaryReader.h"

BinaryReader::BinaryReader(BYTE *pData, DWORD dwSize)
{
	m_dwErrorCode = 0;

	m_pData = pData;
	m_pStart = m_pData;
	m_pEnd = m_pStart + dwSize;
}

BinaryReader::~BinaryReader()
{
	for (std::list<char *>::iterator it = m_lStrings.begin(); it != m_lStrings.end(); it++)
		delete[](*it);

	m_lStrings.clear();
}

void BinaryReader::ReadAlign()
{
	DWORD dwOffset = (DWORD)(m_pData - m_pStart);
	if ((dwOffset % 4) != 0)
		m_pData += (4 - (dwOffset % 4));
}

void *BinaryReader::ReadArray(size_t size)
{
	static BYTE dummyData[10000];

	void *retval = m_pData;
	m_pData += size;

	if (m_pData > m_pEnd)
	{
		m_dwErrorCode = 2;
		return dummyData;
	}

	return retval;
}

char *BinaryReader::ReadString(void)
{
	WORD wLen = ReadWORD();

	if (m_dwErrorCode || wLen > MAX_MEALSTRING_LEN)
	{
		DEBUG_BREAK();
		return NULL;
	}

	char *szArray = (char *)ReadArray(wLen);
	if (!szArray)
	{
		DEBUG_BREAK();
		return NULL;
	}

	char *szString = new char[wLen + 1];
	szString[wLen] = 0;
	memcpy(szString, szArray, wLen);
	m_lStrings.push_back(szString);

	ReadAlign();

	return szString;
}
char* BinaryReader::ReadWStringToString(void)
{
	BYTE wLen_sent = (ReadByte())+2; //We get 2 additional charecters
	
	if (m_dwErrorCode)
	{
#if _DEBUG
		__asm int 3;
#endif
		return NULL;
	}
	wchar_t *szArray = (wchar_t *)ReadArray(wLen_sent*sizeof(wchar_t));
	char *szwString = new char[wLen_sent-1];
	int len = wcstombs(szwString, szArray, wLen_sent-2);
	szwString[wLen_sent-2] = 0;
	return szwString;
}
BYTE *BinaryReader::GetDataStart()
{
	return m_pStart;
}

BYTE *BinaryReader::GetDataPtr()
{
	return m_pData;
}

BYTE *BinaryReader::GetDataEnd()
{
	return m_pEnd;
}

DWORD BinaryReader::GetDataLen()
{
	return (DWORD)(m_pEnd - m_pStart);
}

DWORD BinaryReader::GetOffset()
{
	return (DWORD)(m_pData - m_pStart);
}

void BinaryReader::SetOffset(DWORD offset)
{
	m_pData = m_pStart + offset;
}

DWORD BinaryReader::GetLastError()
{
	return m_dwErrorCode;
}

DWORD BinaryReader::GetDataRemaining()
{
	return m_pEnd - m_pData;
}



