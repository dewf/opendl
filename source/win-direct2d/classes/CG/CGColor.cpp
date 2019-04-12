#include "CGColor.h"

CGColorRef CGColor::White = makeEternal(new CGColor(D2D1::ColorF::White));
CGColorRef CGColor::Black = makeEternal(new CGColor(D2D1::ColorF::Black));
CGColorRef CGColor::Clear = makeEternal(new CGColor(0, 0, 0, 0));
