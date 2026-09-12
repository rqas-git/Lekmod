

























#ifndef __FIRAXIS_STL_BLOCK_ALLOCATOR_MT_H__
#define __FIRAXIS_STL_BLOCK_ALLOCATOR_MT_H__





	#include "FMemHooks.h"
	#include "FCriticalSection.h"








FCriticalSection & FSTLBlockAllocatorMTGetCriticalSection();





template <typename T, unsigned int uBlockCountT, unsigned int pool_type, unsigned int uAlignment = __alignof(T)>
class FSTLBlockAllocatorMT
{



	public:


		typedef T value_type;


		typedef       T * pointer;
		typedef const T * const_pointer;


		typedef       T & reference;
		typedef const T & const_reference;


		typedef size_t    size_type;
		typedef ptrdiff_t difference_type;




	public:

		template <typename U> struct rebind
		{
			typedef FSTLBlockAllocatorMT<U, uBlockCountT, pool_type, uAlignment> other;
		};




	public:

		FSTLBlockAllocatorMT() : m_pFreeStack(NULL)
		{
		}

		FSTLBlockAllocatorMT(const FSTLBlockAllocatorMT<T, uBlockCountT, pool_type, uAlignment> & rhs) : m_pFreeStack(NULL)
		{
		}

		template <typename U, unsigned int uBlockCountU, unsigned int pool_typeU, unsigned int uAlignmentU >
		FSTLBlockAllocatorMT(const FSTLBlockAllocatorMT<U, uBlockCountU, pool_typeU, uAlignmentU> & rhs) : m_pFreeStack(NULL)
		{
		}




	public:

		~FSTLBlockAllocatorMT()
		{
		}




	public:

		template <typename U, unsigned int uBlockCountU, unsigned int pool_typeU, unsigned int uAlignmentU>
		FSTLBlockAllocatorMT& operator=(const FSTLBlockAllocatorMT<U, uBlockCountU, pool_typeU, uAlignmentU> & rhs)
		{
			return * this;
		}




	public:




		pointer allocate(size_type nCount, const void * pHint)
		{
			FScopedCriticalSection Crit(FSTLBlockAllocatorMTGetCriticalSection());

			if (nCount == 1)
			{
				return static_cast<pointer>(AllocateFromPool());
			}
			else
			{

				return static_cast<pointer>( FMALLOCALIGNED(nCount*sizeof(T), (uAlignment == 0) ? __alignof(T) : uAlignment, pool_type, 0) );

			}
		}




		pointer allocate(size_type nCount)
		{
			FScopedCriticalSection Crit(FSTLBlockAllocatorMTGetCriticalSection());
			return allocate(nCount, NULL);
		}




		void deallocate(pointer pMemory, size_type nCount)
		{
			FSTLBlockAllocatorMTGetCriticalSection().Enter();

			if (nCount == 1)
			{
				FreeFromPool(pMemory);
			}
			else
			{

				FFREEALIGNED(pMemory);
			}

			FSTLBlockAllocatorMTGetCriticalSection().Leave();
		}




		void construct(pointer pMemory, const T & rValue)
		{
			::new (pMemory) T(rValue);
		}




		void destroy(pointer pObject)
		{
			pObject->~T();
		}




		pointer address(reference rObject) const
		{
			return & rObject;
		}




		const_pointer address(const_reference rObject) const
		{
			return & rObject;
		}




		size_type max_size() const
		{
			size_type nCount = static_cast<size_type>(-1) / sizeof (T); 

			if (nCount)
			{
				return nCount;
			}
			else
			{
				return 1;
			}
		}




	private:




		void * AllocateFromPool()
		{
			if (m_pFreeStack == NULL)
			{
				CreatePool();
			}

			return Pop();
		}




		void FreeFromPool(void * pBlock)
		{
			Push(pBlock);
		}




		size_t GetBlockSize() const
		{
			return MAX(sizeof(T), sizeof(void *));
		}




		void CreatePool()
		{

			char * pMemoryPool = reinterpret_cast<char *>(FMALLOCALIGNED(GetBlockSize() * uBlockCountT, (uAlignment == 0) ? __alignof( T ) : uAlignment, pool_type, 0));

			if (pMemoryPool)
			{   
				for (unsigned int i = 0; i < uBlockCountT; i++)
				{
					Push(pMemoryPool);
					pMemoryPool += GetBlockSize();
				}
			}
		}




		void Push(void * pBlock)
		{
			*(reinterpret_cast<void **>(pBlock)) = m_pFreeStack;
			m_pFreeStack = pBlock;
		}




		void * Pop()
		{
			void * pBlock = m_pFreeStack;

			if (pBlock)
			{
				m_pFreeStack = *(reinterpret_cast<void **>(pBlock));
			}

			return pBlock;
		}

		void * m_pFreeStack;
};





template <typename T, unsigned int uBlockCountT, unsigned int pool_typeT, unsigned int uAlignmentT, typename U, unsigned int uBlockCountU, unsigned int pool_typeU, unsigned int uAlignmentU>
inline bool operator==(const FSTLBlockAllocatorMT<T, uBlockCountT, pool_typeT, uAlignmentT> & lhs, const FSTLBlockAllocatorMT<U, uBlockCountU, pool_typeU, uAlignmentU> & rhs)
{
	return true;
}

template <typename T, unsigned int uBlockCountT, unsigned int pool_typeT, unsigned int uAlignmentT, typename U, unsigned int uBlockCountU, unsigned int pool_typeU, unsigned int uAlignmentU>
inline bool operator!=(const FSTLBlockAllocatorMT<T, uBlockCountT, pool_typeT, uAlignmentT> & lhs, const FSTLBlockAllocatorMT<U, uBlockCountU, pool_typeU, uAlignmentU> & rhs)
{
	return false;
}





template <typename T, unsigned int uBlockCountT, unsigned int pool_type, unsigned int uAlignment = __alignof(T)>
class FSTL_Tagged_Allocator
{



	public:


		typedef T value_type;


		typedef       T * pointer;
		typedef const T * const_pointer;


		typedef       T & reference;
		typedef const T & const_reference;


		typedef size_t    size_type;
		typedef ptrdiff_t difference_type;




	public:

		template <typename U> struct rebind
		{
			typedef FSTL_Tagged_Allocator<U, uBlockCountT, pool_type, uAlignment> other;
		};




	public:

		FSTL_Tagged_Allocator()
		{
		}

		FSTL_Tagged_Allocator(const FSTL_Tagged_Allocator<T, uBlockCountT, pool_type, uAlignment> & rhs)
		{
		}

		template <typename U, unsigned int uBlockCountU, unsigned int pool_typeU, unsigned int uAlignmentU >
		FSTL_Tagged_Allocator(const FSTL_Tagged_Allocator<U, uBlockCountU, pool_typeU, uAlignmentU> & rhs)
		{
		}




	public:

		~FSTL_Tagged_Allocator()
		{
		}




	public:

		template <typename U, unsigned int uBlockCountU, unsigned int pool_typeU, unsigned int uAlignmentU>
		FSTL_Tagged_Allocator& operator=(const FSTL_Tagged_Allocator<U, uBlockCountU, pool_typeU, uAlignmentU> & rhs)
		{
			return * this;
		}




	public:




		pointer allocate(size_type nCount, const void * pHint)
		{
			return static_cast<pointer>( FMALLOCALIGNED(nCount*sizeof(T), (uAlignment == 0) ? __alignof(T) : uAlignment, pool_type, 0) );
		}




		pointer allocate(size_type nCount)
		{
			return static_cast<pointer>( FMALLOCALIGNED(nCount*sizeof(T), (uAlignment == 0) ? __alignof(T) : uAlignment, pool_type, 0) );
		}




		void deallocate(pointer pMemory, size_type nCount)
		{
			FFREEALIGNED(pMemory);
		}




		void construct(pointer pMemory, const T & rValue)
		{
			::new (pMemory) T(rValue);
		}




		void destroy(pointer pObject)
		{
			pObject->~T();
		}




		pointer address(reference rObject) const
		{
			return & rObject;
		}




		const_pointer address(const_reference rObject) const
		{
			return & rObject;
		}




		size_type max_size() const
		{
			size_type nCount = static_cast<size_type>(-1) / sizeof (T); 

			if (nCount)
			{
				return nCount;
			}
			else
			{
				return 1;
			}
		}

};





template <typename T, unsigned int uBlockCountT, unsigned int pool_typeT, unsigned int uAlignmentT, typename U, unsigned int uBlockCountU, unsigned int pool_typeU, unsigned int uAlignmentU>
inline bool operator==(const FSTL_Tagged_Allocator<T, uBlockCountT, pool_typeT, uAlignmentT> & lhs, const FSTL_Tagged_Allocator<U, uBlockCountU, pool_typeU, uAlignmentU> & rhs)
{
	return true;
}

template <typename T, unsigned int uBlockCountT, unsigned int pool_typeT, unsigned int uAlignmentT, typename U, unsigned int uBlockCountU, unsigned int pool_typeU, unsigned int uAlignmentU>
inline bool operator!=(const FSTL_Tagged_Allocator<T, uBlockCountT, pool_typeT, uAlignmentT> & lhs, const FSTL_Tagged_Allocator<U, uBlockCountU, pool_typeU, uAlignmentU> & rhs)
{
	return false;
}




#endif
