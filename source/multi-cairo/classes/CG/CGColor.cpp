#include "CGColor.h"

using namespace cf;

CGColorRef CGColor::White = makeEternal(new CGColor(1, 1, 1));
CGColorRef CGColor::Black = makeEternal(new CGColor(0, 0, 0));
CGColorRef CGColor::Clear = makeEternal(new CGColor(0, 0, 0, 0));
