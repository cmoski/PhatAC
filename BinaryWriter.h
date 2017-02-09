
#pragma once

class BinaryWriter
{
public:
	BinaryWriter();
	~BinaryWriter();

	void ExpandBuffer(size_t len);

	void AppendString(const char *szString);
	void AppendStringW(const wchar_t * szString);
	void AppendData(const void *pData, size_t len);
	void Align(void);

	template <class DataT> void AppendData(DataT pData)
	{
		AppendData(&pData, sizeof(DataT));
	}

	__forceinline void WriteChar(char f00d) { AppendData(f00d); }
	__forceinline void WriteShort(short f00d) { AppendData(f00d); }
	__forceinline void WriteLong(long f00d) { AppendData(f00d); }
	__forceinline void WriteFloat(float f00d) { AppendData(f00d); }
	__forceinline void WriteDouble(double f00d) { AppendData(f00d); }

	__forceinline void WriteBYTE(BYTE f00d) { AppendData(f00d); }
	__forceinline void WriteWORD(WORD f00d) { AppendData(f00d); }
	__forceinline void WriteDWORD(DWORD f00d) { AppendData(f00d); }

	__forceinline void WritePackedDWORD(DWORD f00d) {
		if (f00d < 0x8000)
		{
			WriteWORD((WORD)f00d);
		}
		else
		{
			WriteDWORD((f00d << 16) | ((f00d >> 16) | 0x8000));
		}
	}

	__forceinline void WriteString(const char *f00d) { AppendString(f00d); }
	__forceinline void WriteStringW(const wchar_t *f00d) { AppendStringW(f00d); }
	__forceinline void WriteData(const void *data, size_t len) { AppendData(data, len); }

	template<typename A, typename B> void WriteMap(std::map<A, B> &table)
	{
		WriteWORD((WORD) table.size());
		WriteWORD(0x40);
		for (std::map<A, B>::iterator keyValuePair = table.begin(); keyValuePair != table.end(); keyValuePair++)
		{
			AppendData((A)keyValuePair->first);
			AppendData((B)keyValuePair->second);
		}
	}

	template<typename A> void WriteMap(std::map<A, std::string> &table)
	{
		WriteWORD((WORD) table.size());
		WriteWORD(0x40);
		for (std::map<A, std::string>::iterator keyValuePair = table.begin(); keyValuePair != table.end(); keyValuePair++)
		{
			AppendData((A)keyValuePair->first);
			AppendString(keyValuePair->second.c_str());
		}
	}

	BYTE*	GetData();
	DWORD	GetSize();

protected:
	//BYTE	m_pbData[0x800];
	BYTE*	m_pbData;
	DWORD	m_dwDataSize;
	BYTE*	m_pbDataPos;
	DWORD	m_dwSize;
};
