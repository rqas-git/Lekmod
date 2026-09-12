







#ifndef __FIRAXIS_MEM_HOOKS_H__
#define __FIRAXIS_MEM_HOOKS_H__




typedef enum eMPoolType
{
	c_eMPoolTypeContainer = 0,
	c_eMPoolTypeSBA,
	c_eMPoolTypeFireWorks,
	c_eMPoolTypeFString,
    c_eMPoolTypeStringPalette,
    c_eMPoolTypeStringHashPalette,
	c_eMPoolTypeTinyXML,
	c_eMPoolTypeFXml,
	c_eMPoolTypeFFileIO,
	c_eMPoolTypeFireGrafix,
	c_eMPoolTypeTextures,
	c_eMPoolTypeGranny,
	c_eMPoolTypeFireSound,
	c_eMPoolTypeSamples,
	c_eMPoolTypeFireTune,
	c_eMPoolTypeFireWire,
	c_eMPoolTypeFireEngine,
	c_eMPoolTypeJobManager,
	c_eMPoolTypeGFC,
	c_eMPoolTypeUI,
	c_eMPoolTypeLua,
	c_eMPoolTypeGame,
	c_eMPoolTypeGamebryo,
	c_eMPoolTypePhysics,
	c_eMPoolTypeForkParticles,
	c_eMPoolTypeSTL,
	c_eMPoolTypeTool,
    c_eMPoolTypeGrannyAsset,
    c_eMPoolTypeGrannyAssetInstance,
    c_eMPoolTypeFGXModelTemplate,
    c_eMPoolTypeFGXModelInstance,
    c_eMPoolTypeEventSystem,
    c_eMPoolTypeInstructionCache,
    c_eMPoolTypeResourceManager,
    c_eMPoolTypeTriggerSystem,
    c_eMPoolTypeFileManager,
    c_eMPoolTypeStateMachine,
    c_eMPoolTypeFloatVarList,
	c_eMPoolTypeDebugMenu,
	c_eMPoolTypeCurve,
	c_eDebugSafetyNet,
	c_eMPoolTypeUserStart
#ifdef AUI_VC120_FORMALITIES
} _eMPoolType;
#else
};
#endif

enum eMPoolTag
{
	c_eMPoolTagDefault,


	c_eMPoolTagFireWorksDefaultNew = 1
};

enum eMHeaps
{
	c_eMHeapMain,
	c_eMHeapSBA,
	c_eMHeapGPU,
};




	#include "FUnDefNew.h"








#if defined (FXS_MEMORY_TRACKER) || defined (FXS_MEMORY_MANAGER)
	void * FireMalloc(size_t nSize, const char * szFile, int nLine, int nPoolType, int nPoolTag );
	void * FireMallocAligned(size_t nSize, size_t nAlignment, const char * szFile, int nLine, int nPoolType, int nPoolTag );
	void * FireCalloc(size_t nItems, size_t nSize, const char * szFile, int nLine, int nPoolType, int nPoolTag );
	void * FireRealloc(void * pBlock, size_t nSize, const char * szFile, int nLine, int nPoolType, int nPoolTag );
	void * FireReallocAligned(void * pBlock, size_t nSize, size_t nAlignment, const char * szFile, int nLine, int nPoolType, int nPoolTag );

	void FireFree(void * pBlock);
	void FireFreeAligned(void * pBlock);

    void * FireGuardBuild( void *pBlock, unsigned int size );
    void * FireGuardCheck( void *pBlock );

	size_t FireMemSize(void * pBlock);
	bool Validate();
#if !defined (FINAL_RELEASE)
#define VALIDATE_MEMORY
#else
#define VALIDATE_MEMORY
#endif
#endif

#ifndef VALIDATE_MEMORY
#define VALIDATE_MEMORY
#endif








#if defined (FXS_MEMORY_TRACKER) || defined (FXS_MEMORY_MANAGER)

	void * operator new  (size_t nSize);
	void * operator new[](size_t nSize);


	void * operator new  (size_t nSize, int nBlockType, const char * szFile, int nLine, int nPool, int nTag);
	void * operator new[](size_t nSize, int nBlockType, const char * szFile, int nLine, int nPool, int nTag);


	void operator delete  (void * pBlock);
	void operator delete[](void * pBlock);	
	void operator delete  (void * pBlock, int nBlockType, const char * szFile, int nLine, int nPool, int nTag );
	void operator delete[](void * pBlock, int nBlockType, const char * szFile, int nLine, int nPool, int nTag );
#else

	void * FireMallocAlignedNoTracking(size_t nSize, size_t nAlignment, const char * szFile, int nLine);
	void FireFreeAlignedNoTracking(void * pBlock);
#endif















namespace FMemHooks
{
	void EnableDefaultNewTraps();
	unsigned int DisableDefaultNewTraps();
	unsigned int SetDefaultNewTrapsState(unsigned int uiState);
	unsigned int AreDefaultNewTrapsEnabled();
	void EnableSBATracking( bool bEnable );
	bool IsSBATrackingEnabled();
}






	#include "FDefNew.h"




#pragma warning( disable : 4291 )


#define FMEMORY_VALIDATE_DEFAULT			0
#define FMEMORY_VALIDATE_CONTENTS			0x00000001
#define FMEMORY_VALIDATE_ON_ALLOCATION		0x00000002
#define FMEMORY_VALIDATE_ON_DEALLOCATION	0x00000004

bool FMemoryManagerValidate(uint uiOptions = 0);
uint FGetMemoryManagerValidateOptions();
uint FSetMemoryManagerValidateOptions(uint uiOptions);
uint FSetMemoryManagerValidateFrequency(uint uiValidateFrequency);


extern bool g_bDisableSmallObjectAllocator; 

#ifdef _WINPC
 #ifdef FXS_IS_DLL
  #define FSBAExport 
 #else
  #define FSBAExport __declspec(dllexport)
 #endif
#else
  #define FSBAExport 
#endif

class FSmallObjectAllocator;
class FSmallObjectAllocatorInitParams;

extern "C" 
{
	FSBAExport FSmallObjectAllocator * GetSmallObjectAllocator(bool bCreate = true);
	FSBAExport FSmallObjectAllocatorInitParams * GetSmallObjectAllocatorInitParams();

	FSBAExport void* FSBAAllocate(size_t nSize);
	FSBAExport bool FSBADeallocate(void* pBlock);
	FSBAExport void FSBADisable(bool bState);
	FSBAExport bool FSBAIsDisabled();
	FSBAExport bool FSBAIsSmallBlock(void* pBlock, size_t& stSize);
	FSBAExport bool FSBAHasAllocatorForSize(size_t tSize);
	FSBAExport size_t FSBAGetMaxAllocatorSize();
	FSBAExport bool FSBAPreventCreation(bool bState);
}


#endif
