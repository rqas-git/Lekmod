













#ifndef _INC_WINAPIFAMILY
#define _INC_WINAPIFAMILY

#if defined(_MSC_VER) && !defined(MOFCOMP_PASS)
#if _MSC_VER >= 1200
#pragma warning(push)
#pragma warning(disable:4001)
#endif
#pragma once
#endif

#include <winpackagefamily.h>









































#define WINAPI_FAMILY_PC_APP               2
#define WINAPI_FAMILY_PHONE_APP            3
#define WINAPI_FAMILY_SYSTEM               4
#define WINAPI_FAMILY_SERVER               5
#define WINAPI_FAMILY_GAMES                6
#define WINAPI_FAMILY_DESKTOP_APP          100








#define WINAPI_FAMILY_APP  WINAPI_FAMILY_PC_APP





#ifndef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#endif

















































#if WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP &&                              \
    WINAPI_FAMILY != WINAPI_FAMILY_PC_APP &&                                   \
    WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP &&                                \
    WINAPI_FAMILY != WINAPI_FAMILY_SYSTEM &&                                   \
    WINAPI_FAMILY != WINAPI_FAMILY_GAMES &&                                    \
    WINAPI_FAMILY != WINAPI_FAMILY_SERVER
#error Unknown WINAPI_FAMILY value. Was it defined in terms of a WINAPI_PARTITION_* value?
#endif

#ifndef WINAPI_PARTITION_DESKTOP
#define WINAPI_PARTITION_DESKTOP (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#endif 

#ifndef WINAPI_PARTITION_APP
#define WINAPI_PARTITION_APP                                                   \
  (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP ||                               \
   WINAPI_FAMILY == WINAPI_FAMILY_PC_APP ||                                    \
   WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
#endif 

#ifndef WINAPI_PARTITION_PC_APP
#define WINAPI_PARTITION_PC_APP                                                \
  (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP ||                               \
   WINAPI_FAMILY == WINAPI_FAMILY_PC_APP)
#endif 

#ifndef WINAPI_PARTITION_PHONE_APP
#define WINAPI_PARTITION_PHONE_APP (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
#endif 

#ifndef WINAPI_PARTITION_GAMES
#define WINAPI_PARTITION_GAMES                                                 \
  (WINAPI_FAMILY == WINAPI_FAMILY_GAMES ||                                     \
   WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#endif







#ifndef WINAPI_PARTITION_SYSTEM
#define WINAPI_PARTITION_SYSTEM                                                \
  (WINAPI_FAMILY == WINAPI_FAMILY_SYSTEM ||                                    \
   WINAPI_FAMILY == WINAPI_FAMILY_SERVER)
#endif 






#define WINAPI_PARTITION_PHONE  WINAPI_PARTITION_PHONE_APP
































#define WINAPI_FAMILY_PARTITION(Partitions)     (Partitions)






#define _WINAPI_DEPRECATED_DECLARATION  __declspec(deprecated("This API cannot be used in the context of the caller's application type."))







#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) && !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#   define APP_DEPRECATED_HRESULT    HRESULT _WINAPI_DEPRECATED_DECLARATION
#endif

#if defined(_MSC_VER) && !defined(MOFCOMP_PASS)
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif
#endif

#endif
