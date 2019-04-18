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

wl_WindowRef mainWin;
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
wl_TimerRef animTimer;

class MyWindowFuncs : public WindowFuncs
{
public:
	void invalidate(int x, int y, int width, int height) override {
		if (width > 0 && height > 0) {
			wl_WindowInvalidate(mainWin, x, y, width, height);
		}
		else {
			wl_WindowInvalidate(mainWin, 0, 0, 0, 0);
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
    wl_ResizeEvent sizeEvent;
    sizeEvent.newWidth = width;
    sizeEvent.newHeight = height;
    sizeEvent.oldWidth = sizeEvent.oldHeight = -1;
    currentPage->onSizeEvent(sizeEvent);
    
	wl_WindowInvalidate(mainWin, 0, 0, 0, 0);
}

dl_CGContextRef createContext(void *voidPlatformContext) {
#ifdef WL_PLATFORM_WINDOWS
    auto platformContext = (wl_PlatformContextD2D *)voidPlatformContext;
    auto context = dl_CGContextCreateD2D(platformContext->target /*, platformContext->writeFactory */);
#elif defined(WL_PLATFORM_APPLE)
    auto context = dl_CGContextCreateQuartz((CGContextRef)voidPlatformContext, height);
#elif defined(WL_PLATFORM_LINUX)
    auto context = dl_CGContextCreateCairo((cairo_t *)voidPlatformContext, width, height);
#endif
    return context;
}

int CDECL wlCallback(wl_WindowRef window, struct wl_Event *event, void *userData)
{
    event->handled = true;
    switch(event->eventType) {
        case wl_kEventTypeWindowDestroyed:
            wl_ExitRunloop();
            break;
        case wl_kEventTypeAction:
            if (event->actionEvent.id == QUIT_ACTION) {
                wl_WindowDestroy(mainWin);
            }
            break;
        case wl_kEventTypeWindowResized:
            width = event->resizeEvent.newWidth;
            height = event->resizeEvent.newHeight;
            if (mainWin) { // have to check, on win32 we get size events before wlCreateWindow has returned
				currentPage->onSizeEvent(event->resizeEvent);
                if (!currentPage->isAnimating()) {
					// don't bother if it's going to get refreshed anyway ...
                    wl_WindowInvalidate(mainWin, 0, 0, 0, 0);
                }
            }
            break;
        case wl_kEventTypeWindowRepaint:
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
        case wl_kEventTypeMouse:
			currentPage->onMouseEvent(event->mouseEvent);
            break;
        case wl_kEventTypeKey:
			switch (event->keyEvent.key) {
			case wl_kKey1:
			case wl_kKey2:
			case wl_kKey3:
			case wl_kKey4:
			case wl_kKey5:
			case wl_kKey6:
			case wl_kKey7:
			case wl_kKey8:
				if (event->keyEvent.eventType == wl_kKeyEventTypeDown) {
					setPageIndex(event->keyEvent.key - wl_kKey1);
				}
				break;
			case wl_kKeyTab:
				if (event->keyEvent.eventType == wl_kKeyEventTypeDown) {
					if (!renderSecondary) {
						renderSecondary = true;
						if (!currentPage->isAnimating()) {
							wl_WindowInvalidate(mainWin, 0, 0, 0, 0);
						}
					}
				}
				else if (event->keyEvent.eventType == wl_kKeyEventTypeUp) {
					if (renderSecondary) {
						renderSecondary = false;
						if (!currentPage->isAnimating()) {
							wl_WindowInvalidate(mainWin, 0, 0, 0, 0);
						}
					}
				}
				break;
			default:
				currentPage->onKeyEvent(event->keyEvent);
				break;
			}
            break;
        case wl_kEventTypeTimer:
        {
			currentPage->onTimerEvent(event->timerEvent.secondsSinceLast);
            break;
        }
#ifdef WL_PLATFORM_WINDOWS
		case wl_kEventTypeD2DTargetRecreated:
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
	wl_PlatformOptions wlOptions = {};
#ifdef WL_PLATFORM_WINDOWS
	wlOptions.useDirect2D = true;
#endif
    wl_Init(&wlCallback, &wlOptions);

	DLPlatformOptions dlOptions = {};
#ifdef WL_PLATFORM_WINDOWS
	dlOptions.factory = wlOptions.outParams.factory;
#endif
	dl_Init(&dlOptions);

	// init pages now that WL/DL are ready
	for (int i = 0; i < NUMPAGES; i++) {
		pages[i]->init();
	}

    wl_WindowProperties props;
    props.usedFields = wl_kWindowPropMinWidth | wl_kWindowPropMinHeight | wl_kWindowPropMaxWidth | wl_kWindowPropMaxHeight;
    props.minWidth = minWidth;
    props.minHeight = minHeight;
    props.maxWidth = maxWidth;
    props.maxHeight = maxHeight;

    mainWin = wl_WindowCreate(initWidth, initHeight, "woot opendl", nullptr, &props);

    auto quitAccel = wl_AccelCreate(wl_kKeyQ, wl_kModifierControl);
    auto quitAction = wl_ActionCreate(QUIT_ACTION, "&Quit", nullptr, quitAccel);
#if defined(WL_PLATFORM_WINDOWS) || defined(WL_PLATFORM_LINUX)
    auto fileMenu = wl_MenuCreate();
    wl_MenuAddAction(fileMenu, quitAction);
    auto menuBar = wl_MenuBarCreate();
    wl_MenuBarAddMenu(menuBar, "File", fileMenu);
    wl_WindowSetMenuBar(mainWin, menuBar);
#elif defined(WL_PLATFORM_APPLE)
    auto appMenu = wl_GetApplicationMenu();
    wl_MenuAddAction(appMenu, quitAction);
#endif

    animTimer = wl_TimerCreate(mainWin, ANIM_TIMER, 1000 / 60);
    
    wl_WindowShow(mainWin);
    
    wl_Runloop();

	dl_Shutdown();
    wl_Shutdown();
    return 0;
}
