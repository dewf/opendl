#include "MyTextAnalyzer.h"
#include "../private_defs.h"

#include <assert.h>
#include <stdio.h>

static const WCHAR *defaultLocaleName = L"en-us";

MyTextAnalyzer::MyTextAnalyzer(const wchar_t *text)
	: MyUnknown(__uuidof(IDWriteTextAnalysisSource), __uuidof(IDWriteTextAnalysisSink))
{
	this->text = text;
	HR(writeFactory->CreateNumberSubstitution(
		DWRITE_NUMBER_SUBSTITUTION_METHOD_NONE,
		defaultLocaleName,
		FALSE,
		&numberSubst));
}

MyTextAnalyzer::~MyTextAnalyzer()
{
	SafeRelease(&numberSubst);
}

DWRITE_READING_DIRECTION MyTextAnalyzer::getBaseDirection()
{
	if (segments[0].resolvedLevel == 1) {
		return DWRITE_READING_DIRECTION_RIGHT_TO_LEFT;
	}
	else {
		return DWRITE_READING_DIRECTION_LEFT_TO_RIGHT;
	}
}

IFACEMETHODIMP MyTextAnalyzer::GetTextAtPosition(
	UINT32 textPosition,
	_Outptr_result_buffer_(*textLength) WCHAR const** textString,
	_Out_ UINT32* textLength
) {
	auto len = text.length() - textPosition;
	if (len > 0) {
		*textString = &text.c_str()[textPosition];
		*textLength = (UINT32) len;
	}
	else {
		*textString = NULL;
		*textLength = 0;
	}
	return S_OK;
}

IFACEMETHODIMP MyTextAnalyzer::GetTextBeforePosition(
	UINT32 textPosition,
	_Outptr_result_buffer_(*textLength) WCHAR const** textString,
	_Out_ UINT32* textLength
) {
	if (textPosition > 0 && textPosition <= text.length()) {
		*textLength = textPosition;
		*textString = text.c_str();
	}
	else {
		*textLength = 0;
		*textString = NULL;
	}
	return S_OK;
}

IFACEMETHODIMP_(DWRITE_READING_DIRECTION) MyTextAnalyzer::GetParagraphReadingDirection() {
	return DWRITE_READING_DIRECTION_LEFT_TO_RIGHT;
}

IFACEMETHODIMP MyTextAnalyzer::GetLocaleName(
	UINT32 textPosition,
	_Out_ UINT32* textLength,
	_Outptr_result_z_ WCHAR const** localeName
) {
	assert(textPosition < text.length());
	*localeName = defaultLocaleName;
	*textLength = (UINT32) (text.length() - textPosition);
	return S_OK;
}

IFACEMETHODIMP MyTextAnalyzer::GetNumberSubstitution(
	UINT32 textPosition,
	_Out_ UINT32* textLength,
	_COM_Outptr_ IDWriteNumberSubstitution** numberSubstitution
) {
	assert(textPosition < text.length());
	*textLength = (UINT32) (text.length() - textPosition);

	this->numberSubst->AddRef();
	*numberSubstitution = this->numberSubst;

	return S_OK;
}


IFACEMETHODIMP MyTextAnalyzer::SetScriptAnalysis(
	UINT32 textPosition,
	UINT32 textLength,
	_In_ DWRITE_SCRIPT_ANALYSIS const* scriptAnalysis
) {
	// sink method, don't care 
	return S_OK;
}

IFACEMETHODIMP MyTextAnalyzer::SetLineBreakpoints(
	UINT32 textPosition,
	UINT32 textLength,
	_In_reads_(textLength) DWRITE_LINE_BREAKPOINT const* lineBreakpoints
) {
	// sink method, don't care 
	return S_OK;
}

IFACEMETHODIMP MyTextAnalyzer::SetBidiLevel(
	UINT32 textPosition,
	UINT32 textLength,
	UINT8 explicitLevel,
	UINT8 resolvedLevel
) {
	segments.push_back({ textPosition, textLength, explicitLevel, resolvedLevel });
	return S_OK;
}

IFACEMETHODIMP MyTextAnalyzer::SetNumberSubstitution(
	UINT32 textPosition,
	UINT32 textLength,
	_In_ IDWriteNumberSubstitution* numberSubstitution
) {
	// sink method, don't care 
	return S_OK;
}

// IUnknown methods ======================================


IFACEMETHODIMP MyTextAnalyzer::QueryInterface(IID const& riid, void** ppvObject)
{
	return MyUnknown::QueryInterface(riid, ppvObject);
}

IFACEMETHODIMP_(unsigned long) MyTextAnalyzer::AddRef()
{
	return MyUnknown::AddRef();
}

IFACEMETHODIMP_(unsigned long) MyTextAnalyzer::Release()
{
	return MyUnknown::Release();
}
