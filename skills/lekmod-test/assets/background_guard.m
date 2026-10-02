#import <AppKit/AppKit.h>
#import <Carbon/Carbon.h>
#import <objc/runtime.h>
#include <stdlib.h>
#include <string.h>

/* This library only changes the disposable validation process. It sends no
   desktop input. The game retains its renderer and Lua automation loop, while
   attempts to show windows, activate, grab input, or alter displays are ignored. */
static unsigned long blocked;
static void ignoreObject(id obj, SEL sel, id arg) { ++blocked; }
static void ignoreBool(id obj, SEL sel, BOOL arg) { ++blocked; }
static void ignoreNone(id obj, SEL sel) { ++blocked; }
static void ignoreOrder(id obj, SEL sel, NSWindowOrderingMode mode, NSInteger other) { ++blocked; }
static BOOL noKey(id obj, SEL sel) { return NO; }
static BOOL noActivate(id obj, SEL sel, NSApplicationActivationOptions options) { ++blocked; return NO; }
static BOOL forceBackground(id obj, SEL sel, NSApplicationActivationPolicy policy) {
    ++blocked;
    return YES; /* The plist starts the process as prohibited; never promote it. */
}
static void replace(Class base, SEL sel, IMP imp) {
    int count = objc_getClassList(NULL, 0);
    Class *classes = calloc(count, sizeof(Class));
    count = objc_getClassList(classes, count);
    for (int i = 0; i < count; ++i) {
        Class ancestor = classes[i];
        while (ancestor && ancestor != base) ancestor = class_getSuperclass(ancestor);
        if (!ancestor) continue;
        Method method = class_getInstanceMethod(classes[i], sel);
        if (method) class_replaceMethod(classes[i], sel, imp, method_getTypeEncoding(method));
    }
    free(classes);
}
static CGError noPoint(CGPoint p) { ++blocked; return kCGErrorSuccess; }
static CGError noDisplayPoint(CGDirectDisplayID d, CGPoint p) { ++blocked; return kCGErrorSuccess; }
static CGError noDisplay(CGDirectDisplayID d) { ++blocked; return kCGErrorSuccess; }
static CGError noAssociate(boolean_t b) { ++blocked; return kCGErrorSuccess; }
static CGError noGamma(CGDirectDisplayID d, uint32_t n, const CGGammaValue *r,
                      const CGGammaValue *g, const CGGammaValue *b) { ++blocked; return kCGErrorSuccess; }
static CGError noDisplayMode(CGDirectDisplayID d, CGDisplayModeRef mode, CFDictionaryRef options) {
    ++blocked; return kCGErrorFailure;
}
static OSStatus noFront(const ProcessSerialNumber *p) { ++blocked; return noErr; }
static OSStatus noFrontOptions(const ProcessSerialNumber *p, OptionBits options) { ++blocked; return noErr; }
static OSStatus noTransform(const ProcessSerialNumber *p, ProcessApplicationTransformState state) {
    ++blocked; return noErr;
}
#define INTERPOSE(replacement, original) \
    __attribute__((used)) static struct { const void *newFunction; const void *oldFunction; } \
    interpose_##original __attribute__((section("__DATA,__interpose"))) = \
    { (const void *)(replacement), (const void *)(original) }
INTERPOSE(noPoint, CGWarpMouseCursorPosition);
INTERPOSE(noDisplayPoint, CGDisplayMoveCursorToPoint);
INTERPOSE(noDisplay, CGDisplayHideCursor);
INTERPOSE(noDisplay, CGDisplayShowCursor);
INTERPOSE(noAssociate, CGAssociateMouseAndMouseCursorPosition);
INTERPOSE(noGamma, CGSetDisplayTransferByTable);
INTERPOSE(noDisplayMode, CGDisplaySetDisplayMode);
INTERPOSE(noFront, SetFrontProcess);
INTERPOSE(noFrontOptions, SetFrontProcessWithOptions);
INTERPOSE(noTransform, TransformProcessType);

__attribute__((constructor)) static void startGuard(void) {
    @autoreleasepool {
        const char *profile = getenv("CFFIXED_USER_HOME");
        if (!profile || strcmp(NSHomeDirectory().fileSystemRepresentation, profile)) {
            fprintf(stderr, "BACKGROUND_GUARD refused: private profile was not selected\n");
            _Exit(78);
        }
        if (![[NSBundle mainBundle] objectForInfoDictionaryKey:@"LSBackgroundOnly"] ||
            ![[[NSBundle mainBundle] objectForInfoDictionaryKey:@"LSBackgroundOnly"] boolValue]) {
            fprintf(stderr, "BACKGROUND_GUARD refused: app is not background-only\n");
            _Exit(78);
        }
        replace([NSApplication class], @selector(activateIgnoringOtherApps:), (IMP)ignoreBool);
        replace([NSApplication class], @selector(setActivationPolicy:), (IMP)forceBackground);
        replace([NSRunningApplication class], @selector(activateWithOptions:), (IMP)noActivate);
        const SEL oneArg[] = { @selector(makeKeyAndOrderFront:), @selector(orderFront:),
            @selector(orderFrontRegardless), @selector(toggleFullScreen:) };
        for (int i = 0; i < 4; ++i) replace([NSWindow class], oneArg[i],
            i == 2 ? (IMP)ignoreNone : (IMP)ignoreObject);
        replace([NSWindow class], @selector(orderWindow:relativeTo:), (IMP)ignoreOrder);
        replace([NSWindow class], @selector(makeKeyWindow), (IMP)ignoreNone);
        replace([NSWindow class], @selector(makeMainWindow), (IMP)ignoreNone);
        replace([NSWindow class], @selector(canBecomeKeyWindow), (IMP)noKey);
        replace([NSWindow class], @selector(canBecomeMainWindow), (IMP)noKey);
        replace([NSCursor class], @selector(set), (IMP)ignoreNone);
        Class cursorMeta = object_getClass([NSCursor class]);
        Method hide = class_getClassMethod([NSCursor class], @selector(hide));
        Method unhide = class_getClassMethod([NSCursor class], @selector(unhide));
        class_replaceMethod(cursorMeta, @selector(hide), (IMP)ignoreNone, method_getTypeEncoding(hide));
        class_replaceMethod(cursorMeta, @selector(unhide), (IMP)ignoreNone, method_getTypeEncoding(unhide));
        fprintf(stderr, "BACKGROUND_GUARD ready; private profile=%s; windows/input/display changes suppressed\n", profile);
        fflush(stderr);
    }
}
__attribute__((destructor)) static void endGuard(void) {
    fprintf(stderr, "BACKGROUND_GUARD blocked %lu presentation/input calls\n", blocked);
}
