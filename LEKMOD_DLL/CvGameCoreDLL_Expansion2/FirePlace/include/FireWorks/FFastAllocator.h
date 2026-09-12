




#include "FFastVector.h"

#ifndef FFAST_ALLOCATOR_H
#define FFAST_ALLOCATOR_H









template< class T > struct FastAllocatorNodePolicy
{
	FastAllocatorNodePolicy(){};
	FastAllocatorNodePolicy(const T& x):data(x){};

	bool m_bDeleted;
	unsigned int uiNext;


	T data;


	unsigned int ALLOC_GetNext() const{ return uiNext; };
	void ALLOC_SetNext(unsigned int uiNodeIndex){ uiNext = uiNodeIndex; };
	bool ALLOC_GetDeleted() const{ return m_bDeleted; };
	void ALLOC_SetDeleted(bool bDeleted){ m_bDeleted = bDeleted; };
};

struct NullFastAllocatorNodePolicy
{
	bool m_bDeleted;
	unsigned int uiNext;


	unsigned int ALLOC_GetNext() const{ return uiNext; };
	void ALLOC_SetNext(unsigned int uiNodeIndex){ uiNext = uiNodeIndex; };
	bool ALLOC_GetDeleted() const{ return m_bDeleted; };
	void ALLOC_SetDeleted(bool bDeleted){ m_bDeleted = bDeleted; };
};








template< 
	class T, 
	bool bPODType = false, 
	unsigned int AllocPool = c_eMPoolTypeContainer, 
	unsigned int nSubID = 0,
	class BASE_ALLOC = typename BaseVector< T, bPODType >::FDefaultFastVectorAllocator
> class FFastAllocator
{
	const static unsigned int ms_uiAnchorNodeIndex = 0x0fffffff;
protected:
	typedef FFastVector< T, bPODType, AllocPool, nSubID, BASE_ALLOC > VectorType;
public:




	FFastAllocator() 
		: m_uiFirstEmpty(ms_uiAnchorNodeIndex), m_uiSize(0) {};
	FFastAllocator( unsigned int uiReserve ) 
		: m_vec( uiReserve ), m_uiFirstEmpty(ms_uiAnchorNodeIndex), m_uiSize(0) {};
	~FFastAllocator(){ assert( m_uiSize == 0); };




	void Reserve(unsigned int uiResSize)
	{
		m_vec.reserve(uiResSize);
	}




	unsigned int Alloc( const T& x)
	{
		unsigned int uiPos;


		if( m_uiFirstEmpty == ms_uiAnchorNodeIndex )
		{
			uiPos = m_vec.size();
			unsigned int uiIndex = m_vec.push_back( x );
			m_vec[uiIndex].ALLOC_SetDeleted(false);
		}


		else{
			uiPos = m_uiFirstEmpty;
			T* pTemp = &m_vec[uiPos];
			m_uiFirstEmpty = pTemp->ALLOC_GetNext();
			pTemp->~T();
			new( (void*)pTemp )T( x );
			pTemp->ALLOC_SetDeleted(false);
		}
		m_uiSize++;
		return uiPos;
	};





	unsigned int Alloc()
	{
		unsigned int uiPos;


		if( m_uiFirstEmpty == ms_uiAnchorNodeIndex )
		{
			uiPos = m_vec.size();
			unsigned int uiIndex = m_vec.push_back();
			m_vec[uiIndex].ALLOC_SetDeleted(false);
		}


		else{
			uiPos = m_uiFirstEmpty;
			T* pTemp = &m_vec[uiPos];
			m_uiFirstEmpty = pTemp->ALLOC_GetNext();
			pTemp->~T();
			new( (void*)pTemp )T();
			pTemp->ALLOC_SetDeleted(false);
		}
		m_uiSize++;
		return uiPos;
	};





	unsigned int Recycle(const T& x)
	{
		unsigned int uiPos;


		if( m_uiFirstEmpty == ms_uiAnchorNodeIndex )
		{
			uiPos = m_vec.size();
			unsigned int uiIndex = m_vec.push_back( x );
			m_vec[uiIndex].ALLOC_SetDeleted(false);
		}


		else{
			uiPos = m_uiFirstEmpty;
			T* pTemp = &m_vec[uiPos];
			m_uiFirstEmpty = pTemp->ALLOC_GetNext();
			pTemp->ALLOC_SetDeleted(false);
		}
		m_uiSize++;
		return uiPos;
	};




	bool is_element_valid(unsigned int uiIndex) const
	{
		return (uiIndex < m_vec.size()) && !m_vec[uiIndex].ALLOC_GetDeleted();
	};



	unsigned int size() const{
		return m_uiSize;
	};



	unsigned int max_active_index() const{
		return m_vec.size();
	};



	unsigned int capacity() const{
		return m_vec.capacity();
	};




	void clear(){
		m_uiFirstEmpty = ms_uiAnchorNodeIndex;
		m_vec.clear();
		m_uiSize = 0;
	};




	void destroy(){
		clear();
		m_vec.setsize(0);
	};




	void Free( unsigned int uiIndex )
	{
		T& element = m_vec[uiIndex];
		FAssert( !element.ALLOC_GetDeleted() );
		element.ALLOC_SetDeleted(true);

		if( uiIndex < m_uiFirstEmpty){
			element.ALLOC_SetNext(m_uiFirstEmpty);
			m_uiFirstEmpty = uiIndex;
		}else{
			T* pRoot = &m_vec[m_uiFirstEmpty];
			element.ALLOC_SetNext( pRoot->ALLOC_GetNext() );
			pRoot->ALLOC_SetNext(uiIndex);
		}
		m_uiSize--;
	};





	bool FreeIfDeleted( unsigned int uiIndex )
	{
		T& element = m_vec[uiIndex];
		if( !element.ALLOC_GetDeleted() ){
			return false;
		}

		if( uiIndex < m_uiFirstEmpty){
			element.ALLOC_SetNext(m_uiFirstEmpty);

			m_uiFirstEmpty = uiIndex;
		}else{
			T* pRoot = &m_vec[m_uiFirstEmpty];
			element.ALLOC_SetNext( pRoot->ALLOC_GetNext() );
			pRoot->ALLOC_SetNext(uiIndex);
		}
		m_uiSize--;

		return true;
	};




	void operator = (const FFastAllocator& rhs)
	{
		m_uiFirstEmpty = rhs.m_uiFirstEmpty;
		m_uiSize	   = rhs.m_uiSize;
		m_vec          = rhs.m_vec;
	}
	T& operator[] ( unsigned int ui ) 
	{
		return m_vec[ui];
	}

	const T& operator[] ( unsigned int ui ) const
	{
		return m_vec[ui];

	}


	unsigned int CalcIndex(void* pLoc)
	{
		const unsigned int uiTypeSize = sizeof(VectorType::TYPE);
		const byte* pHead = reinterpret_cast<byte*>(&m_vec.front());
		const byte* pLast = pHead+(m_vec.capacity()*uiTypeSize);
		if( pLoc < pHead || pLoc >= pLast ) return 0xFFFFFFFF;

		return (reinterpret_cast<byte*>(pLoc)-pHead)/uiTypeSize;
	}

	const char* GetStoragePtr()
	{
		return  reinterpret_cast<char*>(&m_vec.front());
	}

	const uint GetActiveStorageSize()
	{
		return m_vec.capacity()*sizeof(VectorType::TYPE);
	}

protected:






	unsigned int m_uiFirstEmpty;
	unsigned int m_uiSize;


	VectorType m_vec;

#if defined(LEKMOD_MACOS)
	template< class U, bool POD, unsigned int Pool, unsigned int SubID, class Allocator >
	friend void* operator new( size_t uiSize, FFastAllocator< U, POD, Pool, SubID, Allocator >& kAlloc );
#else
	template< class T, bool bPODType, unsigned int AllocPool, unsigned int nSubID, class BASE_ALLOC > 
	friend void* operator new( size_t uiSize, FFastAllocator< T, bPODType, AllocPool, nSubID, BASE_ALLOC >& kAlloc );
#endif
};


template< class T, bool bPODType, unsigned int AllocPool, unsigned int nSubID, class BASE_ALLOC >
void* operator new(size_t uiSize, FFastAllocator< T, bPODType, AllocPool, nSubID, BASE_ALLOC >& kAlloc )
{
	unsigned int uiPos;
	void* pBuf;


	if( kAlloc.m_uiFirstEmpty == kAlloc.ms_uiAnchorNodeIndex )
	{
		uiPos = kAlloc.m_vec.size();
		pBuf = operator new( uiSize, kAlloc.m_vec );
		static_cast<T*>(pBuf)->ALLOC_SetDeleted(false);
	}


	else{
		uiPos = kAlloc.m_uiFirstEmpty;
		T* pTemp = &kAlloc.m_vec[uiPos];
		pBuf = static_cast<void*>(pTemp);
		kAlloc.m_uiFirstEmpty = pTemp->ALLOC_GetNext();
		pTemp->~T();
		pTemp->ALLOC_SetDeleted(false);
	}
	kAlloc.m_uiSize++;
	return pBuf;
}









template< 
	class T,
	unsigned int AllocPool = c_eMPoolTypeContainer, 
	unsigned int nSubID = 0,
#if defined(LEKMOD_MACOS)
	class BASE_ALLOC = typename BaseVector< T, false >::FDefaultFastVectorAllocator
#else
	class BASE_ALLOC = typename BaseVector< T, bPODType >::FDefaultFastVectorAllocator
#endif
> class FFixedBlockAllocator
{
	const static unsigned int ms_uiAnchorNodeIndex = 0x0fffffff;
public:




	FFixedBlockAllocator()
		: m_uiFirstEmpty(ms_uiAnchorNodeIndex), m_uiSize(0), m_pData(NULL), m_uiCapacity(0), m_uiMaxActiveIndex(0)
	{};

	FFixedBlockAllocator( unsigned int uiCapacity ) 
		: m_uiFirstEmpty(ms_uiAnchorNodeIndex), m_uiSize(0), m_pData(NULL), m_uiCapacity(uiCapacity), m_uiMaxActiveIndex(0)
	{
		m_pData = BASE_ALLOC::AllocAligned( sizeof(T) * m_uiCapacity, __alignof(T), AllocPool, nSubID );
	};
	~FFixedBlockAllocator()
	{
		FAssert( m_uiSize == 0);
		clear();
		BASE_ALLOC::FreeAligned( m_pData );
		m_pData = NULL;
	};

	void Reserve( unsigned int uiCapacity )
	{
		if( m_pData )
		{
			FAssert( m_uiSize == 0);
			clear();
			BASE_ALLOC::FreeAligned( m_pData );
			m_pData = NULL;
		}

		m_uiCapacity = uiCapacity;
		m_pData = BASE_ALLOC::AllocAligned( sizeof(T) * m_uiCapacity, __alignof(T), AllocPool, nSubID );
	}




	unsigned int Alloc( const T& x)
	{
		if( m_uiSize == m_uiCapacity )
		{
			FAssertMsg(false, "Fixed-size allocator is full!" );
			return UINT_MAX;
		}

		unsigned int uiPos;


		if( m_uiFirstEmpty == ms_uiAnchorNodeIndex )
		{
			uiPos = m_uiSize+1;
			new( (void*)&m_pData[uiPos]) T( x );
			m_pData[uiPos].ALLOC_SetDeleted(false);
		}


		else{
			uiPos = m_uiFirstEmpty;
			T* pTemp = &m_pData[uiPos];
			m_uiFirstEmpty = pTemp->ALLOC_GetNext();
			pTemp->~T();
			new( (void*)pTemp )T( x );
			pTemp->ALLOC_SetDeleted(false);
		}
		m_uiMaxActiveIndex++;
		m_uiSize++;
		return uiPos;
	};





	unsigned int Alloc()
	{
		if( m_uiSize == m_uiCapacity )
		{
			FAssertMsg(false, "Fixed-size allocator is full!" );
			return UINT_MAX;
		}

		unsigned int uiPos;


		if( m_uiFirstEmpty == ms_uiAnchorNodeIndex )
		{
			uiPos = m_uiSize+1;
			new( (void*)&m_pData[uiPos]) T();
			m_pData[uiPos].ALLOC_SetDeleted(false);
		}


		else{
			uiPos = m_uiFirstEmpty;
			T* pTemp = &m_pData[uiPos];
			m_uiFirstEmpty = pTemp->ALLOC_GetNext();
			new( (void*)pTemp )T( x );
			pTemp->ALLOC_SetDeleted(false);
		}
		m_uiMaxActiveIndex++;
		m_uiSize++;
		return uiPos;
	};




	bool is_element_valid(unsigned int uiIndex) const
	{
		return (uiIndex < m_uiSize) && !m_pData[uiIndex].ALLOC_GetDeleted();
	};



	unsigned int size() const{
		return m_uiSize;
	};



	unsigned int max_active_index() const{
		return m_uiMaxActiveIndex;
	};



	unsigned int capacity() const{
		return m_uiCapacity;
	};




	void clear(){
		while( m_uiFirstEmpty != ms_uiAnchorNodeIndex )
			Free( m_uiFirstEmpty );
	};




	void Free( unsigned int uiIndex )
	{
		if( uiIndex >= m_uiMaxActiveIndex )
		{
			FAssert(false);
			return;
		}

		T& element = m_pData[uiIndex];
		FAssert( !element.ALLOC_GetDeleted() );
		element.ALLOC_SetDeleted(true);
		element.~T();

		if( uiIndex < m_uiFirstEmpty){
			element.ALLOC_SetNext(m_uiFirstEmpty);
			m_uiFirstEmpty = uiIndex;
		}else{
			T* pRoot = &m_pData[m_uiFirstEmpty];
			element.ALLOC_SetNext( pRoot->ALLOC_GetNext() );
			pRoot->ALLOC_SetNext(uiIndex);
		}
		m_uiSize--;
		if( uiIndex == m_uiMaxActiveIndex-1 ) m_uiMaxActiveIndex--;
	};





	bool FreeIfDeleted( unsigned int uiIndex )
	{
		T& element = m_pData[uiIndex];
		if( !element.ALLOC_GetDeleted() ){
			return false;
		}

		if( uiIndex < m_uiFirstEmpty){
			element.ALLOC_SetNext(m_uiFirstEmpty);

			m_uiFirstEmpty = uiIndex;
		}else{
			T* pRoot = &m_vec[m_uiFirstEmpty];
			element.ALLOC_SetNext( pRoot->ALLOC_GetNext() );
			pRoot->ALLOC_SetNext(uiIndex);
		}
		m_uiSize--;
		if( uiIndex == m_uiMaxActiveIndex-1 ) m_uiMaxActiveIndex--;

		return true;
	};



private:
#if defined(LEKMOD_MACOS)
	void operator = (const FFixedBlockAllocator& rhs){}
#else
	void operator = (const FFastAllocator& rhs){}
#endif


public:
	T& operator[] ( unsigned int ui ) 
	{
		FAssert( ui < m_uiMaxActiveIndex );
		FAssert( !m_pData[ui].ALLOC_GetDeleted() );
		return m_pData[ui];
	}

	const T& operator[] ( unsigned int ui ) const
	{
		FAssert( ui < m_uiMaxActiveIndex );
		FAssert( !m_pData[ui].ALLOC_GetDeleted() );
		return m_pData[ui];
	}












protected:






	unsigned int m_uiSize;
	unsigned int m_uiFirstEmpty;
	unsigned int m_uiCapacity;
	unsigned int m_uiMaxActiveIndex;


	T* m_pData;

#if defined(LEKMOD_MACOS)
	template< class U, unsigned int Pool, unsigned int SubID, class Allocator >
	friend void* operator new( size_t uiSize, FFixedBlockAllocator< U, Pool, SubID, Allocator >& kAlloc );
#else
	template< class T, unsigned int AllocPool, unsigned int nSubID, class BASE_ALLOC >
	friend void* operator new( size_t uiSize, FFixedBlockAllocator< T, AllocPool, nSubID, BASE_ALLOC >& kAlloc );
#endif
};


template< class T, unsigned int AllocPool, unsigned int nSubID, class BASE_ALLOC >
void* operator new(size_t uiSize, FFixedBlockAllocator< T, AllocPool, nSubID, BASE_ALLOC >& kAlloc )
{
	if( kAlloc.m_uiSize == kAlloc.m_uiCapacity )
	{
		FAssertMsg(false, "Fixed-size allocator is full!" );
		return UINT_MAX;
	}

	unsigned int uiPos;


	if( kAlloc.m_uiFirstEmpty == kAlloc.ms_uiAnchorNodeIndex )
	{
		uiPos = kAlloc.m_uiSize+1;
		kAlloc.m_pData[uiPos].ALLOC_SetDeleted(false);
	}


	else{
		uiPos = kAlloc.m_uiFirstEmpty;
		T* pTemp = &kAlloc.m_pData[uiPos];
		kAlloc.m_uiFirstEmpty = pTemp->ALLOC_GetNext();
		pTemp->ALLOC_SetDeleted(false);
	}
	kAlloc.m_uiMaxActiveIndex++;
	kAlloc.m_uiSize++;

	return &kAlloc.m_pData[uiPos];
}

#endif
