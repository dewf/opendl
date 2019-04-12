#include "CFMisc.h"

#include <map>
#include <algorithm>
#include <string>
#include <cctype>
#include <exception>

using namespace cf;

static bool monitorAllocations = false;
static std::map<void *, RefCountedRef> allocatedObjects;

RefCounted::RefCounted()
{
	if (monitorAllocations) {
		allocatedObjects[this] = this;
	}
}

RefCounted::~RefCounted()
{
	if (monitorAllocations) {
		allocatedObjects.erase(this);
	}
}

void RefCounted::startLeakMonitoring()
{
	monitorAllocations = true;
}

void RefCounted::endLeakMonitoring()
{
	if (monitorAllocations) {
		printf("=== BEGIN LEAK REPORT ===\n");
		for (auto i = allocatedObjects.begin(); i != allocatedObjects.end(); i++) {
			RefCountedRef ref = i->second;
			if (!ref->isEternal) {
				printf("leaked: %s\n", ref->toString().c_str());
			}
		}
		printf("=== END LEAK REPORT =====\n");

		allocatedObjects.clear();
		monitorAllocations = false;
	}
}

void RefCounted::release() {
	assert(this != nullptr);
	if (refCount > 0) {
		if (!isEternal) {
			refCount--;
#ifdef _DEBUG
			if (monitorAllocations) {
				//printf("refCount of [[%s]] now %d\n", toString().c_str(), refCount);
			}
#endif // DEBUG
			if (refCount == 0) {
				//printf("deleting [%s]\n", toString().c_str());
				delete this;
			}
		}
		else {
			//printf("attempted to release eternal object [%s]\n", toString().c_str()); // did some testing, using a std::string like this seems safe
		}
	}
	else {
		printf("attempted to released object with refcount <= 0 (actual: %d - probably corrupted memory)\n", refCount);
	}
}

