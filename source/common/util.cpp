#include "util.h"

#include <cstdarg>

//16843009 (UINT_MAX / 255)
// * 0.2126 =  3580823.7134 (r coeff)
// * 0.7152 = 12046120.0368 (g coeff)
// * 0.0722 =  1216065.2498 (b coeff)
auto const r_coeff = (uint32_t)3580823 + 1; // the +1 is to account for the roundoff error (lost fractional parts)
auto const g_coeff = (uint32_t)12046120;
auto const b_coeff = (uint32_t)1216065;

// this doesn't currently account for gamma which obviously complicates things a bit ...

inline void luminanceToAlpha(uint32_t &pixel) {
	uint8_t r = pixel & 0xFF;
	uint8_t g = (pixel & 0xFF00) >> 8;
	uint8_t b = (pixel & 0xFF0000) >> 16;

	// essentially a fixed point multiplication where we only care about the top 8 bits of the result
	uint32_t alpha = (r * r_coeff) + (g * g_coeff) + (b * b_coeff);

	pixel &= 0x00FFFFFF; // clear any existing alpha on the target pixel
	pixel |= alpha & 0xFF000000; // set pixel alpha to top 8 bits of calculated luminance
}

// old version========
//BYTE r = (*p32 & 0xFF);
//BYTE g = ((*p32 >> 8) & 0xFF);
//BYTE b = ((*p32 >> 16) & 0xFF);
//BYTE lum = (BYTE)((0.2126 * r) + (0.7152 * g) + (0.0722 * b));
//*p32 &= 0x00FFFFFF;
//*p32 |= (lum << 24);

#define SPRINTF_BUFFER_SIZE 4096
std::string sprintfToStdString(const char *fmt, ...)
{
	char bigBuffer[SPRINTF_BUFFER_SIZE]; // let's keep it reentrant

	va_list args;
	va_start(args, fmt);
	vsnprintf(bigBuffer, SPRINTF_BUFFER_SIZE, fmt, args);
	va_end(args);

	return std::string(bigBuffer);
}