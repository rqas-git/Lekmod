






#ifndef CVBITFIELD_H
#define CVBITFIELD_H

#pragma once


class CvBitfield
{
public:

	CvBitfield() : m_pBits(NULL), m_uiByteSize(0) {};
	CvBitfield(uint uiSize) : m_pBits(NULL), m_uiByteSize(0) { SetSize(uiSize); };
	~CvBitfield() { SetSize(0); }


	bool GetBit(uint uiIndex) const
	{
		uint uiByteIndex = uiIndex >> 3;
		if (uiByteIndex < m_uiByteSize)
		{
			return (m_pBits[uiByteIndex] & (1 << (uiIndex&0x7))) != 0;
		}
		return false;
	}


	void SetBit(uint uiIndex, bool bValue)
	{
		uint uiByteIndex = uiIndex >> 3;
		if (uiByteIndex < m_uiByteSize)
		{
			if (bValue)
				m_pBits[uiByteIndex] |= (1 << (uiIndex&0x7));
			else
				m_pBits[uiByteIndex] &= ~(1 << (uiIndex&0x7));
		}
		else
			if (bValue)
			{
				SetSize(uiIndex + 1);
				m_pBits[uiByteIndex] |= (1 << (uiIndex&0x7));
			}
	}

	void Clear()
	{
		memset(m_pBits, 0, m_uiByteSize);
	}


	uint GetSize() const
	{
		return m_uiByteSize << 3;
	}

	void SetSize(uint uiSize)
	{
		uint uiByteSize = (uiSize >> 3) + ((uiSize&0x07)?1:0);
		if (uiByteSize == 0)
		{
			delete []m_pBits;
			m_uiByteSize = 0;
			m_pBits = NULL;
		}
		else
			if (uiByteSize > m_uiByteSize)
			{
				byte* pNewBits = new byte[uiByteSize];
				memset(pNewBits, 0, uiByteSize);
				if (m_uiByteSize)
				{
					memcpy(pNewBits, m_pBits, m_uiByteSize);
					delete []m_pBits;
				}
				m_pBits = pNewBits;
				m_uiByteSize = uiByteSize;
			}
			else
				if (uiByteSize < m_uiByteSize)
				{
					byte* pNewBits = new byte[uiByteSize];
					memset(pNewBits, 0, uiByteSize);
					if (m_uiByteSize)
					{
						memcpy(pNewBits, m_pBits, uiByteSize);
						delete []m_pBits;
					}
					m_pBits = pNewBits;
					m_uiByteSize = uiByteSize;
				}
	}

private:
	uint m_uiByteSize;
	byte* m_pBits;
};

#endif
