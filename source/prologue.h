#ifndef __PROLOGUE_H__
#define __PROLOGUE_H__

#ifdef _WIN32
#   define DL_PLATFORM_WINDOWS
#elif defined __linux__
#   define DL_PLATFORM_LINUX
#elif defined __APPLE__ // could also use TargetConditionals.h ?
#   define DL_PLATFORM_MACOS
#endif

#ifdef DL_PLATFORM_WINDOWS
#   ifdef OPENDL_EXPORTS
#       define OPENDL_API __declspec(dllexport)
#   else
#       define OPENDL_API __declspec(dllimport)
#   endif
#   ifdef _WIN64
typedef __int64 ssize_t;
#   else
typedef __int32 ssize_t;
#   endif
#   include <Windows.h> // for CDECL
//#   define CDECL __cdecl
#elif defined DL_PLATFORM_LINUX
#   define OPENDL_API __attribute__((visibility("default")))
#   define CDECL
#   include <sys/types.h> // for ssize_t
#elif defined DL_PLATFORM_MACOS
#   define OPENDL_API __attribute__((visibility("default")))
#   define CDECL
#endif

#include <stddef.h>
#include <stdint.h>

#define DLHANDLE(ns,x) struct __dl_##ns##x; typedef struct __dl_##ns##x* dl_##ns##x##Ref
#define DLHANDLE_WITH_MUTABLE(ns,x) struct __dl_##ns##x; typedef const struct __dl_##ns##x* dl_##ns##x##Ref; typedef struct __dl_##ns##x* dl_##ns##Mutable##x##Ref

#ifndef __cplusplus
#include <stdbool.h>
#endif

#endif // __PROLOGUE_H__
