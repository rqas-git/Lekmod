













#ifndef		FFREELISTARRAYBASE_H
#define		FFREELISTARRAYBASE_H
#pragma		once

namespace FFreeList
{
	enum
	{
		INVALID_INDEX	= -1,
		FREE_LIST_INDEX	= -2
	};
};

template <class T>
class FFreeListArrayBase
{
public:

	FFreeListArrayBase();
	virtual ~FFreeListArrayBase() {}
	virtual void Init(int iNumSlots = 8) = 0;
	virtual void Uninit() = 0;
	virtual T* GetAt(int iIndex) = 0;
	virtual const T* GetAt(int iIndex) const = 0;

	T* operator[]( int iIndex );
	const T * operator[](int index) const;


	T* BeginIter(int* pIterIdx);


	T* NextIter(int* pIterIdx);


	T* EndIter(int* pIterIdx);


	T* PrevIter(int* pIterIdx);


	int GetIndexAfterLast()	{	return m_iLastIndex + 1;	}


	const T* BeginIter(int* pIterIdx) const;


	const T* NextIter(int* pIterIdx) const;


	const T* EndIter(int* pIterIdx) const;


	const T* PrevIter(int* pIterIdx) const;


	int GetIndexAfterLast()	const {	return m_iLastIndex + 1;	}


	int GetCount()	const {	return m_iLastIndex - m_iFreeListCount + 1;	}

	virtual void RemoveAll() = 0;
protected:
	int m_iFreeListHead;
	int m_iFreeListCount;
	int m_iLastIndex;
	int m_iNumSlots;

	virtual void GrowArray() = 0;
};

template <class T>
FFreeListArrayBase<T>::FFreeListArrayBase()
{
	m_iFreeListHead = FFreeList::FREE_LIST_INDEX;
	m_iFreeListCount = 0;
	m_iLastIndex = FFreeList::INVALID_INDEX;
	m_iNumSlots = 0;
}






template < class T >
inline T* FFreeListArrayBase< T >::operator[]( int iIndex )
{
	return ( GetAt( iIndex ) );
}

template < class T >
inline const T* FFreeListArrayBase< T >::operator[]( int iIndex ) const
{
	return ( GetAt( iIndex ) );
}






template <class T>
T* FFreeListArrayBase<T>::BeginIter(int* pIterIdx)
{
	*pIterIdx = 0;
	return NextIter(pIterIdx);
}


template <class T>
T* FFreeListArrayBase<T>::NextIter(int* pIterIdx)
{
	for( ; (*pIterIdx)<GetIndexAfterLast(); (*pIterIdx)++)
	{
		T* pObj = GetAt((*pIterIdx));
		if (pObj)
		{
			(*pIterIdx)++;
			return pObj;
		}
	}
	return NULL;
}


template <class T>
T* FFreeListArrayBase<T>::EndIter(int* pIterIdx)
{
	*pIterIdx = GetIndexAfterLast()-1;
	return PrevIter(pIterIdx);
}


template <class T>
T* FFreeListArrayBase<T>::PrevIter(int* pIterIdx)
{
	for( ; (*pIterIdx)>=0; (*pIterIdx)--)
	{
		T* pObj = GetAt((*pIterIdx));
		if (pObj)
		{
			(*pIterIdx)--;
			return pObj;
		}
	}
	return NULL;
}




template <class T>
const T* FFreeListArrayBase<T>::BeginIter(int* pIterIdx) const
{
	*pIterIdx = 0;
	return NextIter(pIterIdx);
}


template <class T>
const T* FFreeListArrayBase<T>::NextIter(int* pIterIdx) const
{
	for( ; (*pIterIdx)<GetIndexAfterLast(); (*pIterIdx)++)
	{
		const T* pObj = GetAt((*pIterIdx));
		if (pObj)
		{
			(*pIterIdx)++;
			return pObj;
		}
	}
	return NULL;
}


template <class T>
const T* FFreeListArrayBase<T>::EndIter(int* pIterIdx) const
{
	*pIterIdx = GetIndexAfterLast()-1;
	return PrevIter(pIterIdx);
}


template <class T>
const T* FFreeListArrayBase<T>::PrevIter(int* pIterIdx) const
{
	for( ; (*pIterIdx)>=0; (*pIterIdx)--)
	{
		const T* pObj = GetAt((*pIterIdx));
		if (pObj)
		{
			(*pIterIdx)--;
			return pObj;
		}
	}
	return NULL;
}


#endif
