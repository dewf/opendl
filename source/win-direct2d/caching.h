#pragma once

#include "../opendl.h"

#include <map>

struct ColorKey {
	dl_CGFloat red, green, blue, alpha;
};

// less-than operator defined to use ColorKey as a std::map key
bool operator <(const ColorKey &left, const ColorKey &right);

struct DeviceDependentResources {
	std::map<ColorKey, ID2D1SolidColorBrush *> solidColorBrushMap;
	~DeviceDependentResources();
};

void createCacheForTarget(ID2D1RenderTarget *newTarget, ID2D1RenderTarget *oldTarget);
void deleteCacheForTarget(ID2D1RenderTarget *target);
ID2D1SolidColorBrush *getCachedColorBrush(ID2D1RenderTarget *target, dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha);
