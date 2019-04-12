#include "common.h"

#include <stdio.h>

const char *loremIpsum = 
"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Amet mattis vulputate enim nulla aliquet. Bibendum neque egestas congue quisque egestas diam. Sapien nec sagittis aliquam malesuada. Fermentum odio eu feugiat pretium nibh ipsum consequat nisl vel. Imperdiet sed euismod nisi porta lorem mollis aliquam ut porttitor. Elit sed vulputate mi sit. Consequat id porta nibh venenatis. Adipiscing diam donec adipiscing tristique risus nec feugiat in fermentum. Morbi tristique senectus et netus et malesuada fames ac turpis. Faucibus a pellentesque sit amet. Mattis vulputate enim nulla aliquet porttitor lacus. Vel facilisis volutpat est velit egestas. Enim eu turpis egestas pretium aenean pharetra magna ac. Ultricies leo integer malesuada nunc vel."
"\nTellus at urna condimentum mattis pellentesque id nibh. Amet luctus venenatis lectus magna. Dignissim diam quis enim lobortis scelerisque fermentum dui faucibus. Mollis nunc sed id semper risus in hendrerit. Dui vivamus arcu felis bibendum ut tristique et egestas quis. Tempus iaculis urna id volutpat lacus. Pharetra convallis posuere morbi leo urna molestie at elementum. Et ligula ullamcorper malesuada proin libero. Varius vel pharetra vel turpis nunc. In eu mi bibendum neque egestas congue quisque egestas diam. Diam quis enim lobortis scelerisque fermentum dui faucibus in ornare. Dignissim enim sit amet venenatis urna cursus eget nunc scelerisque. Felis bibendum ut tristique et egestas quis. Sagittis orci a scelerisque purus semper eget duis at tellus. Rhoncus est pellentesque elit ullamcorper dignissim."
"\nSed lectus vestibulum mattis ullamcorper velit sed ullamcorper morbi. Nulla facilisi nullam vehicula ipsum a arcu cursus. Venenatis a condimentum vitae sapien. Ultrices gravida dictum fusce ut placerat orci nulla pellentesque. Aliquam etiam erat velit scelerisque in dictum non. Eget est lorem ipsum dolor sit amet consectetur. Quam pellentesque nec nam aliquam sem et. Amet cursus sit amet dictum sit amet. Vitae tortor condimentum lacinia quis vel eros donec ac. Id diam maecenas ultricies mi eget mauris. Nullam vehicula ipsum a arcu cursus vitae. Donec ac odio tempor orci. Id venenatis a condimentum vitae sapien pellentesque habitant morbi tristique. Pharetra convallis posuere morbi leo urna molestie. Malesuada nunc vel risus commodo viverra maecenas accumsan lacus vel. Dui vivamus arcu felis bibendum ut. Nisi lacus sed viverra tellus in hac habitasse platea."
"\nOrnare arcu odio ut sem nulla pharetra. Ut tristique et egestas quis ipsum. Donec ultrices tincidunt arcu non sodales neque sodales ut etiam. Eu volutpat odio facilisis mauris sit. Adipiscing vitae proin sagittis nisl rhoncus mattis rhoncus urna neque. In fermentum et sollicitudin ac orci phasellus egestas. Vitae auctor eu augue ut lectus arcu bibendum at varius. Sit amet volutpat consequat mauris nunc congue. Aliquet lectus proin nibh nisl condimentum id venenatis a. Lorem ipsum dolor sit amet. Pellentesque eu tincidunt tortor aliquam nulla. Fames ac turpis egestas sed tempus urna. Non tellus orci ac auctor augue. Feugiat nibh sed pulvinar proin. Laoreet id donec ultrices tincidunt arcu non sodales neque sodales."
"\nUrna neque viverra justo nec ultrices dui sapien eget mi. Et malesuada fames ac turpis egestas sed tempus. Etiam tempor orci eu lobortis elementum. Phasellus faucibus scelerisque eleifend donec pretium vulputate sapien. Venenatis a condimentum vitae sapien. Aenean pharetra magna ac placerat vestibulum lectus mauris. Varius sit amet mattis vulputate. Pellentesque adipiscing commodo elit at imperdiet. Dignissim cras tincidunt lobortis feugiat. Lorem dolor sed viverra ipsum nunc aliquet. Ante metus dictum at tempor commodo ullamcorper. Risus quis varius quam quisque id.";

const char *arabicSample = "الديباجة\n"
"لمّا كان الاعتراف بالكرامة المتأصلة في جميع أعضاء الأسرة البشرية وبحقوقهم المتساوية الثابتة هوLOLWUT أساس الحرية والعدل والسلام في العالم.\n"
"ولما كان تناسي حقوق الإنسان وازدراؤها قد أفضيا إلى أعمال همجية آذت الضمير الإنساني. وكان غاية ما يرنو إليه عامة البشر انبثاق عالم يتمتع فيه الفرد بحرية القول والعقيدة ويتحرر من الفزع والفاقة.\n"
"ولما كان من الضروري أن يتولى القانون حماية حقوق الإنسان لكيلا يضطر المرء آخرFOO الأمر إلى التمرد على الاستبداد والظلم.\n"
"ولما كانت شعوب الأمم المتحدة قد أكدت في الميثاق من جديد إيمانها بحقوق الإنسان الأساسية وبكرامة الفرد وقدره وبما للرجال والنساء من حقوق متساوية وحزمت أمرها على أن تدفع بالرقي الاجتماعي قدمًا وأن ترفع مستوى الحياة في جو من الحرية أفسح.\n"
"ولما كانت الدول الأعضاء قد تعهدت بالتعاون مع الأمم المتحدة على ضمان إطراد مراعاة حقوق الإنسان والحريات الأساسية واحترامها.\n"
"ولما كان للإدراك العام لهذه الحقوق والحريات الأهمية الكبرى للوفاء التام بهذا التعهد.\n"
"فإن الجمعية العامة\n"
"تنادي بهذا الإعلان العالمي لحقوق الإنسان\n"
"على أنه المستوى المشترك الذي ينبغي أن تستهدفه كافة الشعوب والأمم حتى يسعى كل فرد وهيئة في المجتمع، واضعين على الدوام هذا الإعلان نصب أعينهم، إلى توطيد احترام هذه الحقوق والحريات عن طريق التعليم والتربية واتخاذ إجراءات مطردة، قومية وعالمية، لضمان الإعتراف بها ومراعاتها بصورة عالمية فعالة بين الدول الأعضاء ذاتها وشعوب البقاع الخاضعة لسلطانها.\n"
"المادة 1\n"
"يولد جميع الناس أحرارًا متساوين في الكرامة والحقوق. وقد وهبوا عقلاً وضميرًا وعليهم أن يعامل بعضهم بعضًا بروح الإخاء.\n"
"المادة 2\n"
"لكل إنسان حق التمتع بكافة الحقوق والحريات الواردة في هذا الإعلان، دون أي تمييز، كالتمييز بسبب العنصر أو اللون أو الجنس أو اللغة أو الدين أو الرأي السياسي أو أي رأي آخر، أو الأصل الوطني أو الإجتماعي أو الثروة أو الميلاد أو أي وضع آخر، دون أية تفرقة بين الرجال والنساء.\n"
"وفضلاً عما تقدم فلن يكون هناك أي تمييز أساسه الوضع السياسي أو القانوني أو الدولي لبلد أو البقعة التي ينتمي إليها الفرد سواء كان هذا البلد أو تلك البقعة مستقلاً أو تحت الوصاية أو غير متمتع بالحكم الذاتي أو كانت سيادته خاضعة لأي قيد من القيود.\n";

static dl_CGFloat dashes[] = { 2, 2 };
void dashedRect(dl_CGContextRef c, dl_CGRect r) {
	dl_CGContextSetRGBStrokeColor(c, 0, 0, 0, 1);
	dl_CGContextSetLineDash(c, 0, dashes, 2);
	dl_CGContextStrokeRect(c, r);
}

dl_CGGradientRef getGradient(dl_CGFloat fromR, dl_CGFloat fromG, dl_CGFloat fromB, dl_CGFloat fromA,
	dl_CGFloat toR, dl_CGFloat toG, dl_CGFloat toB, dl_CGFloat toA) {
	dl_CGFloat locations[2] = { 0, 1 };
	dl_CGFloat components[8] = {
		fromR, fromG, fromB, fromA,
		toR, toG, toB, toA
	};
	auto colorspace = dl_CGColorSpaceCreateWithName(dl_kCGColorSpaceGenericRGB);
	auto grad = dl_CGGradientCreateWithColorComponents(colorspace, components, locations, 2);
	dl_CGColorSpaceRelease(colorspace);
	return grad;
}

void gradientDashedRect(dl_CGContextRef c, dl_CGRect r) {
	static auto grad = getGradient(0.8, 0.8, 0.3, 1, 1, 0.5, 0.4, 1);

	dl_CGContextSaveGState(c);
	dl_CGContextClipToRect(c, r);
	dl_CGPoint start, end;
	start.x = r.origin.x;
	start.y = r.origin.y;
	end.x = r.origin.x + r.size.width;
	end.y = r.origin.y + r.size.height;
	dl_CGContextDrawLinearGradient(c, grad, start, end, 0);
	// gradient is static, don't release

	dl_CGContextRestoreGState(c); // clear clip rect

	dl_CGContextSaveGState(c);
	dl_CGContextSetLineWidth(c, 1.0);
	dashedRect(c, r);
	dl_CGContextRestoreGState(c);
}

void textLine(dl_CGContextRef c, dl_CGFloat x, dl_CGFloat y, dl_CFStringRef text, dl_CFDictionaryRef attrsDict, bool withGradient /* = true */, bool fromBaseline /* = false */) {
	auto labelString = dl_CFAttributedStringCreate(text, attrsDict);
	auto line = dl_CTLineCreateWithAttributedString(labelString);

	dl_CGFloat ascent, descent, leading;
	dl_CTLineGetTypographicBounds(line, &ascent, &descent, &leading);

	auto bounds = dl_CTLineGetBoundsWithOptions(line, 0); //dl_kCTLineBoundsUseGlyphPathBounds
	bounds.origin = dl_CGPointMake(x, y);
	bounds.origin.x -= 0.5;
	bounds.origin.y -= 0.5;
	bounds.size.width += 1.0;
	bounds.size.height += 1.0;
	if (withGradient) {
		gradientDashedRect(c, bounds);
	}

	if (fromBaseline) {
		dl_CGContextSetTextPosition(c, x, y);
	}
	else {
		dl_CGContextSetTextPosition(c, x, y + ascent);
	}

	dl_CTLineDraw(line, c);

	dl_CFRelease(line);
	dl_CFRelease(labelString);
}

void pointAt(dl_CGContextRef c, dl_CGPoint p) {
	dl_CGContextSaveGState(c);
	dl_CGContextSetRGBFillColor(c, 1, 0, 0, 1);
	dl_CGContextBeginPath(c);
	dl_CGContextAddArc(c, p.x, p.y, 2.5, 0, M_PI * 2, 0);
	dl_CGContextDrawPath(c, dl_kCGPathFill);
	dl_CGContextRestoreGState(c);
}

void drawRect(dl_CGContextRef c, dl_CGRect r, dl_CGColorRef color, dl_CGFloat width)
{
	dl_CGContextSetStrokeColorWithColor(c, color);
	dl_CGContextStrokeRectWithWidth(c, r, width);
}

void drawLine(dl_CGContextRef c, dl_CGPoint from, dl_CGPoint to, dl_CGColorRef color)
{
	dl_CGContextBeginPath(c);
	dl_CGContextMoveToPoint(c, from.x, from.y);
	dl_CGContextAddLineToPoint(c, to.x, to.y);
	dl_CGContextSetStrokeColorWithColor(c, color);
	dl_CGContextStrokePath(c);
}

void drawLineAt(dl_CGContextRef c, dl_CTLineRef line, dl_CGFloat x, dl_CGFloat y, dl_CGFloat *y_advance)
{
	dl_CGContextSaveGState(c);

	auto green = dl_CGColorCreateGenericRGB(0, 1, 0, 1);
	auto magenta = dl_CGColorCreateGenericRGB(1, 0, 1, 1);
	auto blue = dl_CGColorCreateGenericRGB(0, 0.3, 1, 1);
	auto yellow = dl_CGColorCreateGenericRGB(1, 1, 0, 1);
	auto orange = dl_CGColorCreateGenericRGB(1, 0.6, 0, 1);

	dl_CGFloat tb_width, ascent, descent, leading;
	tb_width = dl_CTLineGetTypographicBounds(line, &ascent, &descent, &leading);

	pointAt(c, dl_CGPointMake(x, y));

	auto lineBounds = dl_CTLineGetBoundsWithOptions(line, 0); //0 = same as dl_kCTLineBoundsUseOpticalBounds?
	lineBounds.origin.x += x;
	lineBounds.origin.y += y;
	drawRect(c, lineBounds, green, 1);

	*y_advance = lineBounds.size.height;

	auto glyphBounds = dl_CTLineGetBoundsWithOptions(line, dl_kCTLineBoundsUseGlyphPathBounds);
	glyphBounds.origin.x += x;
	glyphBounds.origin.y += y;
	drawRect(c, glyphBounds, magenta, 1);

	// various lines
	dl_CGFloat dashes[] = { 4, 4 };
	dl_CGContextSetLineDash(c, 0.0, dashes, 2);
	dl_CGContextSetLineWidth(c, 2);

	// baseline
	drawLine(c, dl_CGPointMake(x, y), dl_CGPointMake(x + lineBounds.size.width, y), blue);

	// descent
	drawLine(c, dl_CGPointMake(x, y + descent), dl_CGPointMake(x + lineBounds.size.width, y + descent), yellow);

	// leading
	drawLine(c, dl_CGPointMake(x, y + descent + leading), dl_CGPointMake(x + lineBounds.size.width, y + descent + leading), orange);

	// ascent
	drawLine(c, dl_CGPointMake(x, y - ascent), dl_CGPointMake(x + lineBounds.size.width, y - ascent), yellow);

	// text itself
	dl_CGContextSetTextPosition(c, x, y);
	dl_CTLineDraw(line, c);

	// release =======================================
	dl_CFRelease(orange);
	dl_CFRelease(yellow);
	dl_CFRelease(blue);
	dl_CFRelease(magenta);
	dl_CFRelease(green);

	dl_CGContextRestoreGState(c);
}

