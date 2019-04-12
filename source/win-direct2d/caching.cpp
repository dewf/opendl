#include "caching.h"

// less-than operator defined to use ColorKey as a std::map key
bool operator <(const ColorKey &left, const ColorKey &right) {
    if (left.red == right.red) {
        if (left.green == right.green) {
            if (left.blue == right.blue) {
                return (left.alpha < right.alpha);
            }
            else {
                return (left.blue < right.blue);
            }
        }
        else {
            return (left.green < right.green);
        }
    }
    else {
        return (left.red < right.red);
    }
}

static std::map<ID2D1RenderTarget *, DeviceDependentResources *> targetDDRMap;

DeviceDependentResources::~DeviceDependentResources()
{
	for (auto i = solidColorBrushMap.begin(); i != solidColorBrushMap.end(); i++) {
		auto brush = i->second;
		brush->Release();
	}
	// no need to clear the brush map, it's all going to be freed immediately
}

void createCacheForTarget(ID2D1RenderTarget *newTarget, ID2D1RenderTarget *oldTarget)
{
	if (oldTarget) {
		deleteCacheForTarget(oldTarget);
	}
	auto ddr = new DeviceDependentResources();
	targetDDRMap[newTarget] = ddr;
	//printf("Allocated new device-dependent resources for D2D target %p\n", newTarget);
}

void deleteCacheForTarget(ID2D1RenderTarget *target) {
	auto found = targetDDRMap.find(target);
	if (found != targetDDRMap.end()) {
		auto ddr = found->second;
		//printf("Releasing device dependent resources for old target %p\n", target);
		delete ddr;
		targetDDRMap.erase(found);
	}
}

ID2D1SolidColorBrush *getCachedColorBrush(ID2D1RenderTarget *target, dl_CGFloat red, dl_CGFloat green, dl_CGFloat blue, dl_CGFloat alpha)
{
	auto ddr = targetDDRMap[target];

    ColorKey key = { red, green, blue, alpha };
    auto found = ddr->solidColorBrushMap.find(key);
    if (found != ddr->solidColorBrushMap.end()) {
        return found->second;
    }
    else {
        // create anew
        auto color = D2D1::ColorF((FLOAT)red, (FLOAT)green, (FLOAT)blue, (FLOAT)alpha);
        ID2D1SolidColorBrush *colorBrush;
        target->CreateSolidColorBrush(color, &colorBrush);
        ddr->solidColorBrushMap[key] = colorBrush;
        return colorBrush;
    }
}


