#pragma once

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include "MyUnknown.h"

#include <vector>

#include "../../../deps/CFMinimal/source/CF/CFTypes.h"

struct BidiAnalysisSegment {
	UINT32 textPosition;
	UINT32 textLength;
	UINT8 explicitLevel;
	UINT8 resolvedLevel;
};

class MyTextAnalyzer
	: public MyUnknown,
	  public IDWriteTextAnalysisSource,
	  public IDWriteTextAnalysisSink
{
private:
	IDWriteNumberSubstitution *numberSubst;
	std::wstring text;
	std::vector<BidiAnalysisSegment> segments;

public:
	MyTextAnalyzer(const wchar_t *text);
	~MyTextAnalyzer();

	DWRITE_READING_DIRECTION getBaseDirection();

	// IDWriteTextAnalysisSource ==================================

	IFACEMETHOD(GetTextAtPosition)(
		UINT32 textPosition,
		_Outptr_result_buffer_(*textLength) WCHAR const** textString,
		_Out_ UINT32* textLength
		);

	IFACEMETHOD(GetTextBeforePosition)(
		UINT32 textPosition,
		_Outptr_result_buffer_(*textLength) WCHAR const** textString,
		_Out_ UINT32* textLength
		);

	IFACEMETHOD_(DWRITE_READING_DIRECTION, GetParagraphReadingDirection)();

	IFACEMETHOD(GetLocaleName)(
		UINT32 textPosition,
		_Out_ UINT32* textLength,
		_Outptr_result_z_ WCHAR const** localeName
		);

	IFACEMETHOD(GetNumberSubstitution)(
		UINT32 textPosition,
		_Out_ UINT32* textLength,
		_COM_Outptr_ IDWriteNumberSubstitution** numberSubstitution
		);


	// IDWriteTextAnalysisSink ======================================

	IFACEMETHOD(SetScriptAnalysis)(
		UINT32 textPosition,
		UINT32 textLength,
		_In_ DWRITE_SCRIPT_ANALYSIS const* scriptAnalysis
		);

	IFACEMETHOD(SetLineBreakpoints)(
		UINT32 textPosition,
		UINT32 textLength,
		_In_reads_(textLength) DWRITE_LINE_BREAKPOINT const* lineBreakpoints
		);

	IFACEMETHOD(SetBidiLevel)(
		UINT32 textPosition,
		UINT32 textLength,
		UINT8 explicitLevel,
		UINT8 resolvedLevel
		);

	IFACEMETHOD(SetNumberSubstitution)(
		UINT32 textPosition,
		UINT32 textLength,
		_In_ IDWriteNumberSubstitution* numberSubstitution
		);

	// IUnknown ===============================================

	// apparently the intricacies of COM force us to redeclare/implement these :(
	// but at least we can just directly call the MyUnknown versions and not have to REALLY implement them
	IFACEMETHOD_(unsigned long, AddRef) ();
	IFACEMETHOD_(unsigned long, Release) ();
	IFACEMETHOD(QueryInterface) (
		IID const& riid,
		void** ppvObject
		);
};
