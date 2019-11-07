#ifndef __COMMON_UTIL_H__
#define __COMMON_UTIL_H__

#include <stdint.h>
#include <string>

#define STRUCT_CAST(x, y) (*((y*)&x))

extern inline void luminanceToAlpha(uint32_t &pixel);
//extern std::string sprintfToStdString(const char *fmt, ...);

#endif // __COMMON_UTIL_H__
