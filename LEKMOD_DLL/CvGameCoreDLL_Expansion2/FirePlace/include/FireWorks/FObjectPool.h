














#ifndef		FOBJECTPOOL_H
#define		FOBJECTPOOL_H
#pragma		once

#include	"FCriticalSection.h"

#define DEFAULT_FOBJECT_POOL_SIZE 8








template<class T>
class FObjectPool {
public:


	FObjectPool( uint uiSize = DEFAULT_FOBJECT_POOL_SIZE, bool bGrow = true );


	FObjectPool(const FObjectPool<T>& source);


	FObjectPool<T>& operator=(const FObjectPool<T>& source);


	virtual ~FObjectPool();


	T* GetFreeObject();


	void Release(T* pObject);

protected:


	uint GetNextFreeIndex();
	

	void Lock()
		{ m_Locker.Enter(); }
	void Unlock()
		{ m_Locker.Leave(); }


	struct FPoolNode
	{
		T* pObject;
		bool bFree;
	};


	FPoolNode * m_pStorage;


	FCriticalSection m_Locker;


	uint m_uiSize;
	bool m_bGrow;


	bool m_bFull;


	uint m_uiFirstFreeIndex;
};






template<class T> 
FObjectPool<T>::FObjectPool( uint uiSize, bool bGrow )
{

	m_pStorage = FNEW( FPoolNode[uiSize], c_eMPoolTypeContainer, 0 );

	#ifndef		_NDS
	if (!m_pStorage)
		throw(-1);
	#endif


	for (uint i = 0; i < uiSize; ++i)
	{
		m_pStorage[i].pObject = FNEW( T, c_eMPoolTypeContainer, 0 );
		m_pStorage[i].bFree = true;
	}

	m_uiSize = uiSize;
	m_bGrow = bGrow;
	m_bFull = false;
	m_uiFirstFreeIndex = 0;
}






template<class T> 
FObjectPool<T>::FObjectPool( const FObjectPool<T>& source )
{
	source.Lock(); 
	

	m_pStorage = FNEW( T(source.m_uiSize * sizeof(FPoolNode)), c_eMPoolTypeContainer, 0 );

	#ifndef		_NDS
	if (!m_pStorage)
		throw(-1);
	#endif


	for (int i = 0; i < source.m_uiSize; ++i)
	{
		m_pStorage[i].pObject = FNEW( T( *(source.m_pStorage[i].pObject) ), c_eMPoolTypeContainer, 0 );
		m_pStorage[i].bFree = source.m_pStorage[i].bFree;
	}
	
	m_uiSize = source.m_uiSize;
	m_bGrow = source.m_bGrow;
	m_bFull = source.m_bFull;
	m_uiFirstFreeIndex = source.m_uiFirstFreeIndex;

	source.Unlock();
}






template<class T> 
FObjectPool<T>& FObjectPool<T>::operator=( const FObjectPool<T>& source )
{

	Lock();
	source.Lock();


	if (m_uiSize < source.m_uiSize)
	{
		FPoolNode *pOldStorage = m_pStorage;
		m_pStorage = FNEW( FPoolNode[ source.m_uiSize ], c_eMPoolTypeFireWorks, 0);
		memmove( m_pStorage, pOldStorage, sizeof( FPoolNode ) * m_uiSize );
		SAFE_DELETE_ARRAY( pOldStorage );
	}

	#ifndef		_NDS
	if (!m_pStorage)
		throw(-1);
	#endif


	for (int i = 0; i < source.m_uiSize; ++i)
	{

		if (i >= m_uiSize)
			m_pStorage[i].pObject = FNEW( T(), c_eMPoolTypeContainer, 0 );
        

		m_pStorage[i].pObject = source.m_pStorage[i].pObject;
		m_pStorage[i].bFree = source.m_pStorage[i].bFree;
	}

	m_uiSize = source.m_uiSize;
	m_bGrow = source.m_bGrow;
	m_bFull = source.m_bFull;
	m_uiFirstFreeIndex = source.m_uiFirstFreeIndex;

	source.Unlock();
	Unlock();
}






template<class T>
FObjectPool<T>::~FObjectPool()
{
	Lock(); 


	for (uint i = 0; i < m_uiSize; ++i)
	{
		if (m_pStorage[i].pObject)
			delete m_pStorage[i].pObject;
	}


	SAFE_DELETE_ARRAY(m_pStorage);
	Unlock();
}








template<class T> 
T* FObjectPool<T>::GetFreeObject( )
{
	Lock();

	T* pReturnObj = NULL;


	if (m_bFull)
	{

		if (m_bGrow)
		{


			FPoolNode *pOldStorage = m_pStorage;
			m_pStorage = FNEW( FPoolNode[ m_uiSize * 2 ], c_eMPoolTypeFireWorks, 0);
			memmove( m_pStorage, pOldStorage, sizeof( FPoolNode ) * m_uiSize );
			SAFE_DELETE_ARRAY( pOldStorage );


			#ifndef		_NDS
			if (!m_pStorage)
				throw(-1);
			#endif


			for (uint i = m_uiSize; i < (2*m_uiSize); ++i)
			{
				m_pStorage[i].pObject = FNEW( T, c_eMPoolTypeContainer, 0 );
				m_pStorage[i].bFree = true;
			}

			m_uiFirstFreeIndex = m_uiSize;
			m_uiSize *= 2;
			m_bFull = false;
		}
	}


	if (!m_bFull)
	{

		pReturnObj = m_pStorage[m_uiFirstFreeIndex].pObject;
		m_pStorage[m_uiFirstFreeIndex].bFree = false;

		m_uiFirstFreeIndex = GetNextFreeIndex();
	}

	Unlock();

	return pReturnObj;
}








template<class T> 
void FObjectPool<T>::Release( T* pObject )
{

	Lock();


	for (uint i = 0; i < m_uiSize; ++i)
	{
		if (m_pStorage[i].pObject == pObject)
		{

			m_pStorage[i].bFree = true;
			m_bFull = false;


			if (i < m_uiFirstFreeIndex)
				m_uiFirstFreeIndex = i;

			break;
		}
	}

	Unlock();
}








template<class T>
uint FObjectPool<T>::GetNextFreeIndex( )
{

	m_bFull = true;
	for (; m_uiFirstFreeIndex < m_uiSize; ++m_uiFirstFreeIndex)
	{
		if (m_pStorage[m_uiFirstFreeIndex].bFree)
		{
			m_bFull = false;
			break;
		}
	}

	return m_uiFirstFreeIndex;
}

#endif
