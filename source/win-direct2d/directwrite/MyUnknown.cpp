#include "MyUnknown.h"

IFACEMETHODIMP MyUnknown::QueryInterface(IID const& riid, void** ppvObject)
{
	for (auto i = interfaceIIDs.begin(); i != interfaceIIDs.end(); i++) {
		if (*i == riid) {
			*ppvObject = this;
			this->AddRef();
			return S_OK;
		}
	}
	// else
	*ppvObject = NULL;
	return E_FAIL;
}

IFACEMETHODIMP_(unsigned long) MyUnknown::AddRef()
{
	return InterlockedIncrement(&cRefCount_);
}

IFACEMETHODIMP_(unsigned long) MyUnknown::Release()
{
	unsigned long newCount = InterlockedDecrement(&cRefCount_);
	if (newCount == 0)
	{
		delete this;
		return 0;
	}
	return newCount;
}

