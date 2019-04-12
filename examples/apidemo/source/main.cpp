#include <stdio.h>

#include "../deps/openwl/source/openwl.h"
#include "../../../source/opendl.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "common.h"

#include "page01.hpp"
#include "page02.hpp"
#include "page03.hpp"
#include "page04.hpp"
#include "page05.hpp"
#include "page06.hpp"
#include "page07.hpp"
#include "page08.hpp"

enum actions {
    QUIT_ACTION
};

wlWindow mainWin;
int minWidth = 320;
int minHeight = 240;
int maxWidth = 1600;
int maxHeight = 900;
int initWidth = 1024;
int initHeight = 768;

int width = initWidth;
int height = initHeight;

enum timers {
    ANIM_TIMER
};
wlTimer animTimer;

class MyWindowFuncs : public WindowFuncs
{
public:
	void invalidate(int x, int y, int width, int height) override {
		if (width > 0 && height > 0) {
			wlWindowInvalidate(mainWin, x, y, width, height);
		}
		else {
			wlWindowInvalidate(mainWin, 0, 0, 0, 0);
		}
	}
};
MyWindowFuncs woot;

PageCommon *pages[] = {
	new CPage01(&woot, minWidth, minHeight, maxWidth, maxHeight, initWidth, initHeight),
	new CPage02(&woot),
	new CPage03(&woot),
	new CPage04(&woot),
	new CPage05(&woot),
	new CPage06(&woot),
	new CPage07(&woot),
	new CPage08(&woot)
};
const int NUMPAGES = sizeof(pages) / sizeof(PageCommon*);
PageCommon *currentPage = pages[0];
bool renderSecondary = false; // A/B comparison, each page can optionally implement render2(), to be shown when Tab is down

void setPageIndex(int index) {
	currentPage = pages[index];
    
    // make sure and send a size event first, useful for laying out text or whatever
    WLResizeEvent sizeEvent;
    sizeEvent.newWidth = width;
    sizeEvent.newHeight = height;
    sizeEvent.oldWidth = sizeEvent.oldHeight = -1;
    currentPage->onSizeEvent(sizeEvent);
    
	wlWindowInvalidate(mainWin, 0, 0, 0, 0);
}

dl_CGContextRef createContext(void *voidPlatformContext) {
#ifdef WL_PLATFORM_WINDOWS
    auto platformContext = (WLPlatformContextD2D *)voidPlatformContext;
    auto context = dl_CGContextCreateD2D(platformContext->target /*, platformContext->writeFactory */);
#elif defined(WL_PLATFORM_APPLE)
    auto context = dl_CGContextCreateQuartz((CGContextRef)voidPlatformContext, height);
#elif defined(WL_PLATFORM_LINUX)
    auto context = dl_CGContextCreateCairo((cairo_t *)voidPlatformContext, width, height);
#endif
    return context;
}

int CDECL wlCallback(wlWindow window, struct WLEvent *event, void *userData)
{
    event->handled = true;
    switch(event->eventType) {
        case WLEventType_WindowDestroyed:
            wlExitRunloop();
            break;
        case WLEventType_Action:
            if (event->actionEvent.id == QUIT_ACTION) {
                wlWindowDestroy(mainWin);
            }
            break;
        case WLEventType_WindowResized:
            width = event->resizeEvent.newWidth;
            height = event->resizeEvent.newHeight;
            if (mainWin) { // have to check, on win32 we get size events before wlCreateWindow has returned
				currentPage->onSizeEvent(event->resizeEvent);
                if (!currentPage->isAnimating()) {
					// don't bother if it's going to get refreshed anyway ...
                    wlWindowInvalidate(mainWin, 0, 0, 0, 0);
                }
            }
            break;
        case WLEventType_WindowRepaint:
            {
                auto context = createContext(event->repaintEvent.platformContext);
				dl_CGContextSaveGState(context);

				if (renderSecondary) {
					currentPage->render2(context, width, height);
				}
				else {
					currentPage->render(context, width, height);
				}

				dl_CGContextRestoreGState(context);
                dl_CGContextRelease(context);
            }
            break;
        case WLEventType_Mouse:
			currentPage->onMouseEvent(event->mouseEvent);
            break;
        case WLEventType_Key:
			switch (event->keyEvent.key) {
			case WLKey_1:
			case WLKey_2:
			case WLKey_3:
			case WLKey_4:
			case WLKey_5:
			case WLKey_6:
			case WLKey_7:
			case WLKey_8:
				if (event->keyEvent.eventType == WLKeyEventType_Down) {
					setPageIndex(event->keyEvent.key - WLKey_1);
				}
				break;
			case WLKey_Tab:
				if (event->keyEvent.eventType == WLKeyEventType_Down) {
					if (!renderSecondary) {
						renderSecondary = true;
						if (!currentPage->isAnimating()) {
							wlWindowInvalidate(mainWin, 0, 0, 0, 0);
						}
					}
				}
				else if (event->keyEvent.eventType == WLKeyEventType_Up) {
					if (renderSecondary) {
						renderSecondary = false;
						if (!currentPage->isAnimating()) {
							wlWindowInvalidate(mainWin, 0, 0, 0, 0);
						}
					}
				}
				break;
			default:
				currentPage->onKeyEvent(event->keyEvent);
				break;
			}
            break;
        case WLEventType_Timer:
        {
			currentPage->onTimerEvent(event->timerEvent.secondsSinceLast);
            break;
        }
#ifdef WL_PLATFORM_WINDOWS
		case WLEventType_D2DTargetRecreated:
			// this just lets us know to flush any cached stuff we have for a target / init a new cache
			dl_D2DTargetRecreated(event->d2dTargetRecreatedEvent.newTarget, event->d2dTargetRecreatedEvent.oldTarget);
			break;
#endif
        default:
            event->handled = false;
    }
    return 0;
}

int main()
{
	WLPlatformOptions wlOptions = {};
#ifdef WL_PLATFORM_WINDOWS
	wlOptions.useDirect2D = true;
#endif
    wlInit(&wlCallback, &wlOptions);

	DLPlatformOptions dlOptions = {};
#ifdef WL_PLATFORM_WINDOWS
	dlOptions.factory = wlOptions.outParams.factory;
#endif
	dl_Init(&dlOptions);

	// init pages now that WL/DL are ready
	for (int i = 0; i < NUMPAGES; i++) {
		pages[i]->init();
	}

    WLWindowProperties props;
    props.usedFields = WLWindowProp_MinWidth | WLWindowProp_MinHeight | WLWindowProp_MaxWidth | WLWindowProp_MaxHeight;
    props.minWidth = minWidth;
    props.minHeight = minHeight;
    props.maxWidth = maxWidth;
    props.maxHeight = maxHeight;

    mainWin = wlWindowCreate(initWidth, initHeight, "woot opendl", nullptr, &props);

    auto quitAccel = wlAccelCreate(WLKey_Q, WLModifier_Control);
    auto quitAction = wlActionCreate(QUIT_ACTION, "&Quit", nullptr, quitAccel);
#if defined(WL_PLATFORM_WINDOWS) || defined(WL_PLATFORM_LINUX)
    auto fileMenu = wlMenuCreate();
    wlMenuAddAction(fileMenu, quitAction);
    auto menuBar = wlMenuBarCreate();
    wlMenuBarAddMenu(menuBar, "File", fileMenu);
    wlWindowSetMenuBar(mainWin, menuBar);
#elif defined(WL_PLATFORM_APPLE)
    auto appMenu = wlGetApplicationMenu();
    wlMenuAddAction(appMenu, quitAction);
#endif

    animTimer = wlTimerCreate(mainWin, ANIM_TIMER, 1000 / 60);
    
    wlWindowShow(mainWin);
    
    wlRunloop();

	dl_Shutdown();
    wlShutdown();
    return 0;
}
