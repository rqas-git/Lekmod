



















#pragma once
#ifndef FRelationshipBitset_h
#define FRelationshipBitset_h



template<uint TElementCount, bool TSelfRelation>
class FRelationshipBitset
{
public:
	FRelationshipBitset()
	{
		ZeroMemory(m_Mem, sizeof(m_Mem));
	}

	const bool Get(uint uiFirst, uint uiSecond) const
	{
		if( uiFirst == uiSecond )
		{
			return TSelfRelation;
		}
		else
		{
			const uint uiBitIndex = GetBitIndex(uiFirst, uiSecond);
			const uint uiByte = uiBitIndex / 8;
			const uint uiBit = uiBitIndex % 8;
			const byte byMask = (1 << uiBit);
			return (m_Mem[uiByte] & byMask) != 0;
		}
	}

	void Set(uint uiFirst, uint uiSecond, bool bVal)
	{
		FAssertMsg(uiFirst != uiSecond, "Cannot set self-relationships");
		if( uiFirst != uiSecond )
		{
			const uint uiBitIndex = GetBitIndex(uiFirst, uiSecond);
			const uint uiByte = uiBitIndex / 8;
			const uint uiBit = uiBitIndex % 8;
			const byte byMask = (1 << uiBit);
			if( bVal ) m_Mem[uiByte] |= byMask;
			else m_Mem[uiByte] &= ~byMask;
		}
	}

	static uint GetSerializedSize(uint uiElementCount)
	{
		const uint uiBitCount = GetTotalBitCount(uiElementCount);
		return uiBitCount / 8 + ((uiBitCount % 8)? 1 : 0);
	}

	void Serialize(void *pvBuffer, uint uiElementCount) const
	{
		FAssert(uiElementCount < TElementCount);
		memcpy(pvBuffer, m_Mem, GetSerializedSize(uiElementCount));
	}

	void Deserialize(const void *pvBuffer, uint uiElementCount)
	{
		FAssert(uiElementCount < TElementCount);
		memcpy(m_Mem, pvBuffer, GetSerializedSize(uiElementCount));
	}

private:

	static const uint BitCount  = ((TElementCount - 1) * TElementCount) / 2;
	static const uint ByteCount = BitCount / 8 + ((BitCount % 8)? 1 : 0);


	static const uint GetTotalBitCount(uint uiElementCount)
	{

		return ((uiElementCount - 1) * uiElementCount) / 2;
	}

	static const uint GetBitIndex(uint uiFirst, uint uiSecond)
	{
		FAssertMsg(uiFirst != uiSecond, "There are no self relations in this bitset");

		uint uiLarge, uiSmall;
		if( uiFirst > uiSecond )
		{
			uiLarge = uiFirst;
			uiSmall = uiSecond;
		}
		else
		{
			uiLarge = uiSecond;
			uiSmall = uiFirst;
		}

		FAssertMsg(uiLarge < TElementCount, "Out of range!");


		const uint uiStart = GetTotalBitCount(uiLarge);
		return uiStart + uiSmall;
	}

	byte m_Mem[ByteCount];
};

#endif
