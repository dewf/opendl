# OpenDL

This library provides a Quartz2D/CoreText-compatible API on multiple platforms:
- Windows, using Direct2D and DirectWrite
- Mac, using Quartz2D and CoreText (but with Y-flipped drawing [see note below])
- Linux, using Cairo and Pango

At this time the only major API difference (other than the function names, prefixed with `dl_`) is that both graphic and text rendering is top-down on all platforms, which is not usually the case with macOS. (Specifically, the origin 0,0 is at the top-left corner on all platforms).

A minimal Core Foundation implementation is baked-in for Windows and Linux, since these are required for many of the Quartz/CoreText calls.

Instructions for building the demo will be coming soon, but in the meantime, check out the `examples/apidemo/build` subdirs for your platform. Regardless of your platform / build location, the `_democontent` subdir must be present relative to your current-working-directory when running the demo. And don't forget to `git submodule init` / `git submodule update` !

Once running the demo, the number keys 1-8 switch pages.

Individual demo page notes:

- Page 1: mouse click toggles animation, [c] toggles masking
- Page 8: pressing [space] toggles the right-to-left text demo

This project, along with its sister project [OpenWL](https://github.com/dewf/openwl) is intended to facilitate the development of new, language-native GUIs in non-C++ languages.

![screenshot](https://user-images.githubusercontent.com/1072976/56012648-aab9a680-5caa-11e9-8511-527b323284ba.png)
