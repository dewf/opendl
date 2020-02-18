#pragma once

#include "common.h"

class CPage09 : public PageCommon
{
private:
    dl_CGGradientRef grad, grad2;

    void drawPoint(dl_CGContextRef context, dl_CGPoint &start, dl_CGFloat radius);
    void transformTest(dl_CGContextRef context);
    void mutablePathTest(dl_CGContextRef context);
public:
	CPage09(WindowFuncs *parent);
	~CPage09();

	void init() override;
	void render(dl_CGContextRef context, int width, int height) override;
};
