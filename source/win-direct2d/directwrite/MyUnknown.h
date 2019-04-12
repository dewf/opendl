#pragma once

#include <objbase.h>
#include <vector>

class MyUnknown : public IUnknown {
protected:
	unsigned long cRefCount_ = 1; // odd that this worked as 0 for so long ...
	std::vector<IID> interfaceIIDs;
public:
	IFACEMETHOD_(unsigned long, AddRef) ();
	IFACEMETHOD_(unsigned long, Release) ();
	IFACEMETHOD(QueryInterface) (
		IID const& riid,
		void** ppvObject
		);

	virtual ~MyUnknown() {}

	MyUnknown() {
		interfaceIIDs.push_back(__uuidof(IUnknown));
	}
	MyUnknown(const IID a) : MyUnknown() {
		interfaceIIDs.push_back(a);
	}
	MyUnknown(const IID a, const IID b) : MyUnknown(a) {
		interfaceIIDs.push_back(b);
	}
	MyUnknown(const IID a, const IID b, const IID c) : MyUnknown(a, b) {
		interfaceIIDs.push_back(c);
	}
	MyUnknown(const IID a, const IID b, const IID c, const IID d) : MyUnknown(a, b, c) {
		interfaceIIDs.push_back(d);
	}
	MyUnknown(const IID a, const IID b, const IID c, const IID d, const IID e) : MyUnknown(a, b, c, d) {
		interfaceIIDs.push_back(e);
	}
	MyUnknown(const IID a, const IID b, const IID c, const IID d, const IID e, const IID f) : MyUnknown(a, b, c, d, e) {
		interfaceIIDs.push_back(f);
	}
	MyUnknown(const IID a, const IID b, const IID c, const IID d, const IID e, const IID f, const IID g) : MyUnknown(a, b, c, d, e, f) {
		interfaceIIDs.push_back(g);
	}
	MyUnknown(const IID a, const IID b, const IID c, const IID d, const IID e, const IID f, const IID g, const IID h) : MyUnknown(a, b, c, d, e, f, g) {
		interfaceIIDs.push_back(h);
	}
};
