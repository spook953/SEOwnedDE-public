#pragma once

#pragma warning (push)
#pragma warning (disable : 4554)

#define LittleDWord( val ) ( val )

inline int BitByte(int bits)
{
	return (bits + 7) >> 3;
}

inline unsigned long LoadLittleDWord(const unsigned long *base, unsigned int dwordIndex)
{
	return LittleDWord(base[dwordIndex]);
}

class bf_write
{
public:
	void StartWriting(void *pData, int nBytes, int iStartBit = 0, int nMaxBits = -1)
	{
		nBytes &= ~3;

		m_pData = (unsigned long *)pData;
		m_nDataBytes = nBytes;

		if (nMaxBits == -1)
		{
			m_nDataBits = nBytes << 3;
		}

		else
		{
			m_nDataBits = nMaxBits;
		}

		m_iCurBit = iStartBit;
		m_bOverflow = false;
	}

public:
	unsigned long *__restrict m_pData;
	int m_nDataBytes;
	int m_nDataBits;
	int m_iCurBit;
	bool m_bOverflow;
	bool m_bAssertOnOverflow;
	const char *m_pDebugName;
};

class bf_read
{
public:
	const unsigned char *m_pData;
	int m_nDataBytes;
	int m_nDataBits;
	int m_iCurBit;
	bool m_bOverflow;
	bool m_bAssertOnOverflow;
	const char *m_pDebugName;

public:
	void SetOverflowFlag()
	{
		if (m_bAssertOnOverflow)
		{
			//Assert(false);
		}
		m_bOverflow = true;
	}

	inline int GetNumBytesRead()
	{
		return BitByte(m_iCurBit);
	}

	inline int GetNumBitsLeft()
	{
		return m_nDataBits - m_iCurBit;
	}

	inline int GetNumBytesLeft()
	{
		return GetNumBitsLeft() >> 3;
	}

	inline int GetNumBitsRead() const
	{
		return m_iCurBit;
	}

	inline bool Seek(int iBit)
	{
		if (iBit < 0 || iBit > m_nDataBits)
		{
			SetOverflowFlag();
			m_iCurBit = m_nDataBits;
			return false;
		}
		else
		{
			m_iCurBit = iBit;
			return true;
		}
	}

	unsigned int ReadUBitLong(int numbits)
	{
		//Assert(numbits > 0 && numbits <= 32);

		if (GetNumBitsLeft() < numbits)
		{
			m_iCurBit = m_nDataBits;
			SetOverflowFlag();
			//CallErrorHandler(BITBUFERROR_BUFFER_OVERRUN, GetDebugName());
			return 0;
		}

		unsigned int iStartBit = m_iCurBit & 31u;
		int iLastBit = m_iCurBit + numbits - 1;
		unsigned int iWordOffset1 = m_iCurBit >> 5;
		unsigned int iWordOffset2 = iLastBit >> 5;
		m_iCurBit += numbits;

		unsigned int bitmask = (2 << (numbits - 1)) - 1;

		unsigned int dw1 = LoadLittleDWord((unsigned long *)m_pData, iWordOffset1) >> iStartBit;
		unsigned int dw2 = LoadLittleDWord((unsigned long *)m_pData, iWordOffset2) << (32 - iStartBit);

		return (dw1 | dw2) & bitmask;
	}

	int	ReadChar()
	{
		return (char)ReadUBitLong(8);
	}

	bool ReadString(char *pStr, int maxLen, bool bLine = false, int *pOutNumChars = NULL)
	{
		//Assert(maxLen != 0);

		bool bTooSmall = false;
		int iChar = 0;
		while (1)
		{
			char val = ReadChar();
			if (val == 0)
				break;
			else if (bLine && val == '\n')
				break;

			if (iChar < (maxLen - 1))
			{
				pStr[iChar] = val;
				++iChar;
			}
			else
			{
				bTooSmall = true;
			}
		}

		// Make sure it's null-terminated.
		//Assert(iChar < maxLen);
		pStr[iChar] = 0;

		if (pOutNumChars)
			*pOutNumChars = iChar;

		return !m_bOverflow && !bTooSmall;
	}
};

#pragma warning (pop)