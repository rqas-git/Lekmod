







#pragma once

#include "FAssert.h"
#include "FArray.h"



struct AllocData
{
	void* pOut;
	unsigned int nX, nY;
};





template< unsigned int, unsigned int AllocPool, unsigned int nSubID = 0 >
struct FAllocBase
{
	FAllocBase()
		: m_pData(NULL)
	{}
	~FAllocBase()
	{
		FAssert( !m_pData );
	}

#ifdef AUI_WARNING_FIXES
	void* Alloc(unsigned int nBytes, AllocData*)
#else
	void* Alloc( unsigned int nBytes, AllocData* pData )
#endif
	{
		m_pData = FNEW( unsigned char[nBytes], AllocPool, nSubID );
		return static_cast< void* >( m_pData + nBytes );
	}

	void Free()
	{
		delete[] m_pData;
		m_pData = NULL;
	}

	unsigned char * m_pData;
};






template< class TYPE, class BASE >
struct FAllocArrayType : protected BASE
{
	void* Alloc( AllocData* pData )
	{
		return Alloc( 0, pData );
	}
	void* Alloc( unsigned int nBytes, AllocData* pData )
	{
		unsigned int nLocalBytes = sizeof(TYPE) * (pData->nX + 1);
		void* pRet = BASE::Alloc( nBytes + nLocalBytes, pData+1 );
		unsigned char* pLocMemStart = static_cast< unsigned char* > ( pRet ) - nLocalBytes;

		unsigned int iAlignmentFix = sizeof(TYPE) - reinterpret_cast< unsigned int >(pLocMemStart) % sizeof(TYPE);
		(*static_cast< TYPE** >(pData->pOut)) = (TYPE*)( pLocMemStart + iAlignmentFix );

		return static_cast< void* >( pLocMemStart );
	}

	void Free()
	{
		BASE::Free();
	}
};




template< class TYPE, class BASE >
struct FAllocArray2DType : protected BASE
{
	void* Alloc( AllocData* pData )
	{
		return Alloc( 0, pData );
	}
	void* Alloc( unsigned int nBytes, AllocData* pData )
	{
		unsigned int nArrayBytes = sizeof(TYPE*) * (pData->nX + 1);
		unsigned int nDataBytes = sizeof(TYPE) * pData->nX * ( pData->nY + 1 );
		void* pRet = BASE::Alloc( nBytes + nDataBytes + nArrayBytes, pData+1 );
		unsigned char* pLocMemStart = static_cast< unsigned char* > ( pRet );

		unsigned int iDataAlignmentFix = sizeof(TYPE) - reinterpret_cast< unsigned int >(pLocMemStart -= nDataBytes) % sizeof(TYPE);
		TYPE* pFirstElement = (TYPE*)( pLocMemStart + iDataAlignmentFix );

		unsigned int iArrayAlignmentFix = sizeof(TYPE*) - reinterpret_cast< unsigned int >(pLocMemStart -= nArrayBytes) % sizeof(TYPE*);
		TYPE** ppArray = (*static_cast< TYPE*** >(pData->pOut)) = (TYPE**)( pLocMemStart + iArrayAlignmentFix );

		for( unsigned int i = 0; i < pData->nX; ++i )
		{
			*ppArray = pFirstElement + i * pData->nY;
			++ppArray;
		}

		return static_cast< void* >( pLocMemStart );
	}
};












inline void TestBatchAlloc()
{
	struct TestStruct
	{
		int a,b,c;
		bool dumbo;
	};

	int* pA;
	bool* pB;
	int* pC;
	double** ppD;


	typedef 
		FAllocArrayType< int,
		FAllocArrayType< bool,
		FAllocArrayType< int,
		FAllocArray2DType< double,

		FAllocBase< 0, 0 > > > > >  TestBatchAlloc;

	AllocData aData[] = 
	{
		{ &pA, 20, 0 },
		{ &pB, 50, 0 },
		{ &pC, 30, 0 },
		{ &ppD, 30, 50 },

	};

	TestBatchAlloc kCompositeElement;
	kCompositeElement.Alloc( aData );


	for( unsigned int i = 0; i < 20; ++i )
	{
		pA[i] = 0;
	}
	for( unsigned int i = 0; i < 50; ++i )
	{
		pB[i] = true;
	}
	for( unsigned int i = 0; i < 30; ++i )
	{
		pC[i] = i;
	}

	for( unsigned int x = 0; x < 30; ++x )
	for( unsigned int y = 0; y < 50; ++y )
	{
		ppD[x][y] = double(x);
	}

	kCompositeElement.Free();

};
