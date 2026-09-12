





























#ifndef __FIRAXIS_STL_BLOCK_ALLOCATOR_H__
#define __FIRAXIS_STL_BLOCK_ALLOCATOR_H__





void FireFreeNoTracking(void * pBlock);
void * FireMallocNoTracking(size_t nSize, const char * szFile, int nLine );
void * FireMalloc(size_t nSize, const char * szFile, int nLine, int nPoolType, int nPoolTag );





template <typename T, unsigned int uBlockCountT, unsigned int pool_type = 0>
class FSTLBlockAllocator
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
			typedef FSTLBlockAllocator<U, uBlockCountT> other;
		};




	public:

		FSTLBlockAllocator()
		{
		}

#ifdef AUI_WARNING_FIXES
		FSTLBlockAllocator(const FSTLBlockAllocator<T, uBlockCountT> &)
#else
		FSTLBlockAllocator(const FSTLBlockAllocator<T, uBlockCountT> & rhs)
#endif
		{
		}

		template <typename U, unsigned int uBlockCountU>
		FSTLBlockAllocator(const FSTLBlockAllocator<U, uBlockCountU> & rhs)
		{
		}




	public:

		~FSTLBlockAllocator()
		{
		}




	public:

		template <typename U, unsigned int uBlockCountU>
		FSTLBlockAllocator<T, uBlockCountT> & operator=(const FSTLBlockAllocator<U, uBlockCountU> & rhs)
		{
			return * this;
		}




	public:




#ifdef AUI_WARNING_FIXES
		pointer allocate(size_type nCount, const void *)
#else
		pointer allocate(size_type nCount, const void * pHint)
#endif
		{
			if (nCount == 1)
			{
				return static_cast<pointer>(AllocateFromPool());
			}
			else
			{
				return static_cast<pointer>(FireMalloc(nCount * sizeof(T), __FILE__, __LINE__, pool_type, 0));
			}
		}




		pointer allocate(size_type nCount)
		{
			return allocate(nCount, NULL);
		}




		void deallocate(pointer pMemory, size_type nCount)
		{
			if (nCount == 1)
			{
				FreeFromPool(pMemory);
			}
			else
			{
				FireFreeNoTracking(pMemory);
			}
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
			if (s_pFreeStack == NULL)
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

			return (sizeof(T) > sizeof(void *) ? sizeof(T) : sizeof(void *));
		}




		void CreatePool()
		{

			char * pMemoryPool = reinterpret_cast<char *>(FireMallocNoTracking(GetBlockSize() * uBlockCountT, __FILE__, __LINE__));

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
			*(reinterpret_cast<void **>(pBlock)) = s_pFreeStack;
			s_pFreeStack = pBlock;
		}




		void * Pop()
		{
			void * pBlock = s_pFreeStack;

			if (pBlock)
			{
				s_pFreeStack = *(reinterpret_cast<void **>(pBlock));
			}

			return pBlock;
		}


		static void * s_pFreeStack;
};





template <typename T, unsigned int uBlockCountT, unsigned int pool_type>
void * FSTLBlockAllocator<T, uBlockCountT, pool_type>::s_pFreeStack = NULL;





template <typename T, unsigned int uBlockCountT, typename U, unsigned int uBlockCountU>
inline bool operator==(const FSTLBlockAllocator<T, uBlockCountT> & lhs, const FSTLBlockAllocator<U, uBlockCountU> & rhs)
{
	return true;
}

template <typename T, unsigned int uBlockCountT, typename U, unsigned int uBlockCountU>
inline bool operator!=(const FSTLBlockAllocator<T, uBlockCountT> & lhs, const FSTLBlockAllocator<U, uBlockCountU> & rhs)
{
	return false;
}





	#include "FDefNew.h"

#endif
