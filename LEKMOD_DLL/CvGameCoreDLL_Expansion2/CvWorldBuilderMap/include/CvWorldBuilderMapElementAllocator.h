






















#pragma once
#ifndef CvWorldBuilderMapElementAllocator_h
#define CvWorldBuilderMapElementAllocator_h

template<class T, uint TInitialSize>
class CvWorldBuilderMapElementAllocator
{

	template<class TOther, uint TOtherInitialSize> friend class CvWorldBuilderMapElementAllocator;

public:
	class Handle
	{

		friend CvWorldBuilderMapElementAllocator;

	public:



		Handle() : m_wIndex(word(-1)) {}
		inline const bool Valid() const { return m_wIndex != word(-1); }



		inline static Handle InvalidHandle() { return Handle(word(-1)); }

		inline const bool operator ==(const Handle kRhs) const { return m_wIndex == kRhs.m_wIndex; }
		inline const bool operator !=(const Handle kRhs) const { return m_wIndex != kRhs.m_wIndex; }

		inline const word ToWord() const { return m_wIndex; }
		inline const int ToInt() const { return m_wIndex; }

	private:
		explicit Handle(word wIndex) : m_wIndex(wIndex) {}






		word m_wIndex;
	};

	CvWorldBuilderMapElementAllocator() : m_aData(NULL), m_iFreeList(-1), m_uiSize(0) {}
	~CvWorldBuilderMapElementAllocator() { Release(); }

	void Release()
	{

		if( m_aData ) FFREE(m_aData);


		new(this)CvWorldBuilderMapElementAllocator();
	}

	const Handle Alloc()
	{

		if( m_iFreeList < 0 )
		{
			if( !Grow((m_uiSize == 0)? TInitialSize : m_uiSize * 2) )
				return Handle::InvalidHandle();
		}


		const int iFree = m_iFreeList;


		m_iFreeList = m_aData[iFree].iNextFreeEntry;


		new(&(m_aData[iFree]))T();


		return Handle((word)iFree);
	}


	void Free(Handle &h)
	{
		if( h.m_wIndex < m_uiSize )
		{


			m_aData[h.m_wIndex].iNextFreeEntry = m_iFreeList;
			m_iFreeList = h.m_wIndex;
		}

		h.m_wIndex = (word)-1;
	}

	_Ret_opt_ inline T *operator[](Handle h)
	{
		if( h.m_wIndex < m_uiSize ) return (T*)&(m_aData[h.m_wIndex]);
		else return NULL;
	}

	_Ret_opt_ inline const T *operator[](Handle h) const
	{
		if( uint(h.m_wIndex) < m_uiSize ) return (const T*)&(m_aData[h.m_wIndex]);
		else return NULL;
	}

	uint GetSerializedSize() const
	{
		if( m_uiSize == 0 )
			return 0;
		else
			return (uint)(m_uiSize * sizeof(Entry) + sizeof(m_iFreeList));
	}



	void Serialize(void *pvDest) const
	{
		FAssert(pvDest);

		if( m_uiSize == 0 )
			return;

		if( pvDest != NULL )
		{
			memcpy(pvDest, &m_iFreeList, sizeof(m_iFreeList));
			memcpy((byte*)pvDest + sizeof(m_iFreeList), m_aData, m_uiSize * sizeof(Entry));
		}
	}

	void Deserialize(const void *pvBuffer, uint uiSize)
	{
		Release();

		if( pvBuffer == NULL || uiSize == 0 )
			return;

		memcpy(&m_iFreeList, pvBuffer, sizeof(m_iFreeList));

		const uint uiRemaning = (uint)(uiSize - sizeof(m_iFreeList));
		m_aData = (Entry*)FMALLOC(uiRemaning, c_eMPoolTypeGame, 0);
		FAssertMsg(m_aData != NULL, "Failed to allocate!");
		if( m_aData != NULL )
		{
			memcpy(m_aData, (byte*)pvBuffer + sizeof(m_iFreeList), uiRemaning);
			m_uiSize = uiRemaning / sizeof(Entry);
		}
		else
		{
			m_iFreeList = -1;
		}
	}



	template<class TargetType, uint TargetInitialSize>
	void ConvertTo(CvWorldBuilderMapElementAllocator<TargetType, TargetInitialSize> &kTarget) const
	{
		typedef CvWorldBuilderMapElementAllocator<TargetType, TargetInitialSize>::Entry TargetEntry;

		kTarget.Release();

		if( m_aData != NULL && m_uiSize != 0 )
		{

			kTarget.m_aData = (TargetEntry*)FMALLOC(sizeof(TargetEntry) * m_uiSize, c_eMPoolTypeGame, 0);
			FAssertMsg(kTarget.m_aData != NULL, "Failed to allocate!");
			if( kTarget.m_aData == NULL )
				return;


			kTarget.m_iFreeList = m_iFreeList;
			kTarget.m_uiSize = m_uiSize;


			const uint uiFreeEntriesSize = m_uiSize * sizeof(bool);
			bool *abFreeEntries = (bool*)FTempHeapFastVectorAllocator::AllocAligned(uiFreeEntriesSize, 4, c_eMPoolTypeGame, 0);
			ZeroMemory(abFreeEntries, uiFreeEntriesSize);

			{
				int iFree = m_iFreeList;
				while( iFree > 0 && iFree < (int)m_uiSize )
				{
					abFreeEntries[iFree] = true;
					iFree = m_aData[iFree].iNextFreeEntry;
				}
			}


			for( uint i = 0; i < m_uiSize; ++i )
			{
				if( abFreeEntries[i] )
				{
					kTarget.m_aData[i].iNextFreeEntry = m_aData[i].iNextFreeEntry;
				}
				else
				{
					const T *pSource = (T*)&(m_aData[i]);
					TargetType *pTarget = (TargetType*)&(kTarget.m_aData[i]);
					new(pTarget)TargetType(*pSource);
				}
			}

			FTempHeapFastVectorAllocator::FreeAligned(abFreeEntries);
		}
	}

private:
	bool Grow(uint uiSize)
	{
		if( m_uiSize >= uiSize )
			return true;


		Entry *aNew = (Entry*)FMALLOC(sizeof(Entry) * uiSize, c_eMPoolTypeGame, 0);
		FAssertMsg(aNew != NULL, "Failed to allocate!");
		if( aNew == NULL )
			return false;


		if( m_uiSize > 0 )
		{

			memcpy(aNew, m_aData, m_uiSize * sizeof(T));
			FFREE(m_aData);
		}


		for( uint i = m_uiSize; i < uiSize - 1; ++i )
			aNew[i].iNextFreeEntry = i + 1;


		if( m_iFreeList >= 0 )
			aNew[uiSize - 1].iNextFreeEntry = m_iFreeList;
		else
			aNew[uiSize - 1].iNextFreeEntry = -1;


		m_iFreeList = m_uiSize;


		m_aData = aNew;
		m_uiSize = uiSize;

		return true;
	}






	struct Entry
	{
		union
		{
			byte mem[sizeof(T)];
			int iNextFreeEntry;
		};
	};

	Entry *m_aData;
	int m_iFreeList;
	uint m_uiSize;
};

#endif
