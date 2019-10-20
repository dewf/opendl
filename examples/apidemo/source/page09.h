#pragma once

#include "common.h"

class CPage09 : public PageCommon
{
private:
    dl_CGGradientRef grad, grad2;

public:
	CPage09(WindowFuncs *parent);
	~CPage09();

	void init() override;
	void drawPoint(const dl_CGContextRef &context, dl_CGPoint &start, dl_CGFloat radius);
	void render(dl_CGContextRef context, int width, int height) override;
};
