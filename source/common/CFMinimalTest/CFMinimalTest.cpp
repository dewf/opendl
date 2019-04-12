// CFMinimalTest.cpp : Defines the entry point for the console application.
//

#include "../CFMinimal.h"

using namespace cf;

int main()
{
	{
		auto wat = String::createWithCString("LOLWUT WUT WUT")->autoRelease();
		//wat->retain();
	}
	printf("out of scope!\n");
	//auto pretend_mutable = new String("C:/hellothere/wat.txt");
	//auto url = URL::createWithFileSystemPath(pretend_mutable, dl_kCFURLPOSIXPathStyle, false);
	//auto out = url->copyFileSystemPath(dl_kCFURLPOSIXPathStyle);
	//out->dump();
	//out->release();
	//url->release();
	//pretend_mutable->release();

	//auto number1 = Number::createWithDouble(4.999);
	//auto number2 = Number::createWithDouble(5.00);
	//printf("less than: %d\n", *number1 < *number2);
	//printf("equals: %d\n", *number1 == *number2);
	//printf("greater than: %d\n", *number1 > *number2);
	//number1->release();
	//number2->release();

	//auto kWhatever01 = String::makeConstantString("kWhatever01");
	//auto kWhatever02 = String::makeConstantString("kWhatever02");
	//auto kWhatever03 = String::makeConstantString("kWhatever03");
	//auto value01 = new String("HEYO-VALUE");
	//auto value02 = new String("SECOND-VALUE");
	//auto value03 = new String("THIRDLY");

	//auto ms = new MutableAttributedString();
	//ms->replaceString(dl_CFRangeMake(0, 0), String::makeConstantString("woot doooot"));
	//ms->setAttribute(dl_CFRangeMake(0, 4), kWhatever01, value01);
	//ms->setAttribute(dl_CFRangeMake(0, 8), kWhatever02, value02);
	//ms->setAttribute(dl_CFRangeMake(2, 6), kWhatever03, value03);
	//ms->dump();

	////auto ms2 = ms->copy();
	//ms->release();
	////ms2->release();

	//value01->release();
	//value02->release();
	//value03->release();

    return 0;
}

