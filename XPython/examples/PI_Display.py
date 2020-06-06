from check_helper import checkBase
from XPLMProcessing import XPLMRegisterFlightLoopCallback, XPLMUnregisterFlightLoopCallback

from XPLMDefs import xplm_DownFlag, xplm_ShiftFlag, XPLM_VK_Y, XPLM_VK_Z, xplm_UpFlag, xplm_OptionAltFlag
from XPLMDefs import xplm_ControlFlag
from XPLMDataAccess import XPLMFindDataRef, XPLMGetDatai
from XPLMUtilities import XPLMGetVersions, XPPythonGetDicts

# definitions
from XPLMDisplay import xplm_Phase_FirstCockpit, xplm_WindowDecorationRoundRectangle, xplm_WindowLayerFloatingWindows
from XPLMDisplay import xplm_WindowPopOut, xplm_WindowCenterOnMonitor, xplm_WindowFullScreenOnMonitor
from XPLMDisplay import xplm_MouseDrag, xplm_MouseUp, xplm_MouseDown
from XPLMDisplay import xplm_CursorDefault, xplm_CursorHidden, xplm_CursorArrow, xplm_CursorCustom

# functions, tested
from XPLMDisplay import XPLMGetAllMonitorBoundsGlobal, XPLMGetAllMonitorBoundsOS, XPLMGetScreenSize, XPLMGetScreenBoundsGlobal
from XPLMDisplay import XPLMCreateWindowEx, XPLMDestroyWindow
from XPLMDisplay import XPLMGetWindowIsVisible, XPLMSetWindowIsVisible
from XPLMDisplay import XPLMWindowIsPoppedOut, XPLMWindowIsInVR
from XPLMDisplay import XPLMSetWindowTitle, XPLMGetWindowRefCon, XPLMSetWindowRefCon
from XPLMDisplay import XPLMIsWindowInFront, XPLMBringWindowToFront
from XPLMDisplay import XPLMTakeKeyboardFocus, XPLMHasKeyboardFocus
from XPLMDisplay import XPLMSetWindowGravity, XPLMSetWindowPositioningMode
from XPLMDisplay import XPLMSetWindowResizingLimits
from XPLMDisplay import XPLMGetMouseLocationGlobal
from XPLMDisplay import XPLMGetWindowGeometry, XPLMSetWindowGeometry, XPLMGetWindowGeometryOS, XPLMSetWindowGeometryOS
from XPLMDisplay import XPLMRegisterDrawCallback, XPLMUnregisterDrawCallback
from XPLMDisplay import XPLMRegisterKeySniffer, XPLMUnregisterKeySniffer
from XPLMDisplay import XPLMRegisterHotKey, XPLMCountHotKeys, XPLMGetNthHotKey, XPLMGetHotKeyInfo, XPLMSetHotKeyCombination
from XPLMDisplay import XPLMUnregisterHotKey

# functions not tested
from XPLMDisplay import XPLMGetWindowGeometryVR, XPLMSetWindowGeometryVR


class PythonInterface(checkBase):
    def __init__(self):
        checkBase.__init__(self, 'Display')
        self.xpluginStopCalled = 0
        self.xpluginEnableCalled = 0
        self.xpluginDisableCalled = 0
        self.xpluginMessageReceived = 0
        self.flightLoopReferenceConstants = {}
        self.flightLoopCalled = 0
        self.drawCallbackCalled = 0
        self.keySnifferBeforeWin = 1
        self.keySnifferRefcon = ['booya']
        self.keySnifferRegistered = 0
        self.testSteps = {}
        self.winRefcon = {}  # indexed by winID
        self.winRefConByWhich = {}  # indexed by "which"
        self.winID = {}  # indexed by "which"
        self.drawBefore = 0
        self.drawPhase = xplm_Phase_FirstCockpit
        self.drawRefcon = ['foobar', ]
        self.isModern = False
        self.hotKeyCallbackFun = self.hotKeyCallback
        self.hotKeyID = None
        self.hotKeyRefcon = ['hotkey ref con', ]
        checkBase.addRef()

    def check(self):
        if not self.xpluginEnableCalled:
            self.error('pluginEnable was not called!')
        if not self.xpluginDisableCalled:
            self.error('pluginDisable was not called!')
        if not self.xpluginMessageReceived:
            self.error('xpluginReceiveMessage was not called!')
        checkBase.check(self)
        checkBase.remRef()

    def XPluginStart(self):
        self.Sig = "XPython.Display"
        self.Name = "{} regression test".format(self.Sig)
        self.Desc = "Regression test of the {}".format(self.Sig)
        self.versions = XPLMGetVersions()

        self.is_modern = XPLMGetDatai(XPLMFindDataRef('sim/graphics/view/using_modern_driver'))
        self.log("Running {}".format('Vulkan/Metal' if self.is_modern else 'OpenGL'))

        # Because some of these test require a particular monitor setup,
        # or require interaction by the user, I've separated them into
        # different 'test sets'. Select a set (0-5) and re-run.
        # TEST_TO_RUN one of [0, 1, 2, 3, 4, 5]
        TEST_TO_RUN = 5
        try:
            whichFlightLoop = TEST_TO_RUN
            if TEST_TO_RUN == 0:
                self.flightLoopReferenceConstants[whichFlightLoop] = 'main'
            elif TEST_TO_RUN == 1:
                self.flightLoopReferenceConstants[whichFlightLoop] = 'gravity'
            elif TEST_TO_RUN == 2:
                self.flightLoopReferenceConstants[whichFlightLoop] = 'positioning'
            elif TEST_TO_RUN == 3:
                self.flightLoopReferenceConstants[whichFlightLoop] = 'mouse and key tests'
            elif TEST_TO_RUN == 4:
                self.flightLoopReferenceConstants[whichFlightLoop] = 'key sniffer'
            elif TEST_TO_RUN == 5:
                self.flightLoopReferenceConstants[whichFlightLoop] = 'hot keys'
            else:
                raise ValueError
        except ValueError:
            self.error("Need to specify which test to run")
        else:
            XPLMRegisterFlightLoopCallback(self.flightLoopCallback, -1.0,
                                           self.flightLoopReferenceConstants[whichFlightLoop])
            self.testSteps[whichFlightLoop] = 0

        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        if self.hotKeyID:
            XPLMUnregisterHotKey(self.hotKeyID)
        if self.keySnifferRegistered:
            self.checkVal("KeySniffer Registered", self.keySnifferRegistered, 1)
            self.log("sniffer is registered, looking to unregister")
            self.log("Sniffer dict is: {}".format(XPPythonGetDicts()['keySniffCallback']))
            self.checkVal('Unregister key sniffer',
                          XPLMUnregisterKeySniffer(self.keySnifferFun,
                                                   self.keySnifferBeforeWin, self.keySnifferRefcon),
                          1)
            self.log("sniffer unregistered")

        if not self.is_modern:
            self.checkVal("Draw callback called (will fail if running Vulkan/Metal)", self.drawCallbackCalled, 1)
            if self.drawCallbackCalled:
                self.checkVal('UnregisterDrawCallback',
                              XPLMUnregisterDrawCallback(self.drawCallbackFun, self.drawPhase,
                                                         self.drawBefore, self.drawRefcon),
                              1)
        for which in self.winID:
            self.log("Looking to destroy window [{}] with id: {}".format(which, self.winID[which]))
            XPLMDestroyWindow(self.winID[which])

        for flightLoop in self.flightLoopReferenceConstants:
            self.log("Looking to unregister flight loop [{}] {}".format(flightLoop,
                                                                        self.flightLoopReferenceConstants[flightLoop]))
            XPLMUnregisterFlightLoopCallback(self.flightLoopCallback,
                                             self.flightLoopReferenceConstants[flightLoop])
        self.check()

    def XPluginEnable(self):
        self.xpluginEnableCalled = 1

        self.drawCallbackFun = self.drawCallback
        self.checkVal("Draw callback registration",
                      XPLMRegisterDrawCallback(self.drawCallbackFun, self.drawPhase,
                                               self.drawBefore, self.drawRefcon),
                      1)
        return 1

    def XPluginDisable(self):
        self.xpluginDisableCalled = 1

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        self.xpluginMessageReceived = 1

    def flightLoopCallback(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):

        # self.drawBefore = 1
        # self.drawRefcon = []
        # i = XPLMRegisterDrawCallback(self.drawCallbackFun, self.drawPhase, self.drawBefore, self.drawRefcon)
        # self.checkVal('Draw callback was not called', self.drawRefcon, [49152])
        # self.checkVal('Draw callback return value incorrect', i, self.drawRefcon[0])

        try:
            whichFlightLoop = [which for which in self.flightLoopReferenceConstants
                               if self.flightLoopReferenceConstants[which] == inRefcon][0]
        except IndexError:
            self.log("Cannot determine which flight loop for refcon: {}. Exiting flight loop".format(inRefcon))
            return 0

        if not self.flightLoopCalled:
            self.flightLoopCalled = elapsedTimeSinceLastFlightLoop
        elif elapsedTimeSinceLastFlightLoop - self.flightLoopCalled < (5 * self.testSteps[whichFlightLoop]):
            return 1.0

        if whichFlightLoop == 0:
            return self.mainFlightLoop(elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon)
        if whichFlightLoop == 1:
            return self.gravityFlightLoop(elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon)
        if whichFlightLoop == 2:
            return self.positioningFlightLoop(elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon)
        if whichFlightLoop == 3:
            return self.keyFlightLoop(elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon)
        if whichFlightLoop == 4:
            return self.keySnifferFlightLoop(elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon)
        if whichFlightLoop == 5:
            return self.hotKeysFlightLoop(elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon)
        self.log("Flight Loop #{} unknown".format(whichFlightLoop))
        return 0

    def hotKeyCallback(self, refCon):
        self.log("Hot key called!, now Press Shift-Y")
        self.checkVal('HotKeyFunction', self.hotKeyRefcon, refCon)
        XPLMSetHotKeyCombination(self.hotKeyID, XPLM_VK_Y, xplm_ShiftFlag)

    def hotKeysFlightLoop(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
        # NOTE: for this test, a simple quick key down / key up, doesn't seem to trigger the hot key
        # _hold_ the key down for a second or so, and hot key will fire.
        whichFlightLoop = 5
        self.log('[{}] flight loop called {:.2f}, {:.2f}, {}'.format(
            self.testSteps[whichFlightLoop],
            elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter))
        codeStep = 0

        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Starting #{} Hotkeys".format(XPLMCountHotKeys()))
            self.hotKeyID = XPLMRegisterHotKey(XPLM_VK_Z, xplm_ShiftFlag, "Z Hot Key!!!!",
                                               self.hotKeyCallbackFun, self.hotKeyRefcon)
            for i in range(XPLMCountHotKeys()):
                outKey = []
                outFlags = []
                outDescription = []
                outPlugin = []
                XPLMGetHotKeyInfo(XPLMGetNthHotKey(i), outKey, outFlags, outDescription, outPlugin)
                self.log("Key #{} on {} with flags: {}. '{}' for plugin #{}".format(
                    i, outKey[0], outFlags[0], outDescription[0], outPlugin[0]))
            self.log("Test set '{}' complete".format(self.flightLoopReferenceConstants[whichFlightLoop]))
            self.log("Now #{} Hotkeys. Press Shift-Z to activate.".format(XPLMCountHotKeys()))
            self.log("Hot key dicts: {}\n{}".format(XPPythonGetDicts()['hotkey'], XPPythonGetDicts()['hotkeyID']))
            return 0

        self.testSteps[whichFlightLoop] += 1

        return 1.0

    def keySnifferFlightLoop(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
        whichFlightLoop = 4
        self.log('[{}] flight loop called {:.2f}, {:.2f}, {}'.format(
            self.testSteps[whichFlightLoop],
            elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter))
        codeStep = 0

        which = 0
        if codeStep == self.testSteps[whichFlightLoop]:
            self.createWindow(which)
            XPLMSetWindowTitle(self.winID[which], 'z to Toggle')
            self.keySnifferFun = self.keySniffer
            self.checkVal('RegisterKeySniffer',
                          XPLMRegisterKeySniffer(self.keySnifferFun,
                                                 self.keySnifferBeforeWin,
                                                 self.keySnifferRefcon),
                          1)
            self.keySnifferRegistered = 1
            self.log("Test set '{}' complete".format(self.flightLoopReferenceConstants[whichFlightLoop]))

        self.testSteps[whichFlightLoop] += 1
        return 1.0

    def keySniffer(self, inChar, inFlags, inVirtualKey, inRefcon):
        modifiers = []
        if inFlags & xplm_ShiftFlag:
            modifiers.append('Shift')
        if inFlags & xplm_OptionAltFlag:
            modifiers.append('Alt')
        if inFlags & xplm_ControlFlag:
            modifiers.append('Ctl')
        if inFlags & xplm_DownFlag:
            modifiers.append('Key Down')
        if inFlags & xplm_UpFlag:
            modifiers.append('Key Up')

        modifiers = '-'.join(modifiers)
        self.log("Key {} {}. Virtual: {}".format(inChar,
                                                 modifiers,
                                                 inVirtualKey))
        if inVirtualKey == XPLM_VK_Z and 'Down' in modifiers:
            XPLMSetWindowIsVisible(self.winID[0], 0 if XPLMGetWindowIsVisible(self.winID[0]) else 1)
            return 0
        return 1

    def keyFlightLoop(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
        whichFlightLoop = 3
        self.log('[{}] flight loop called {:.2f}, {:.2f}, {}'.format(
            self.testSteps[whichFlightLoop],
            elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter))
        codeStep = 0

        which = 0
        if codeStep == self.testSteps[whichFlightLoop]:
            self.createWindow(which)
            XPLMSetWindowTitle(self.winID[which], "Keyboard")
            XPLMSetWindowPositioningMode(self.winID[which], xplm_WindowCenterOnMonitor, -1)
            XPLMTakeKeyboardFocus(self.winID[which])

            XPLMSetWindowResizingLimits(self.winID[which], 20, 10, 300, 600)

        l = []
        t = []
        r = []
        b = []
        XPLMGetWindowGeometry(self.winID[which], l, t, r, b)
        (l, t, r, b) = [x[0] for x in (l, t, r, b)]
        self.log("width: {}, height: {}".format(r - l, t - b))

        x = []
        y = []
        XPLMGetMouseLocationGlobal(x, y)
        self.log("Mouse: {}, {}".format(x[0], y[0]))

        if self.testSteps[whichFlightLoop] == 10:
            if XPLMWindowIsPoppedOut(self.winID[which]):
                XPLMSetWindowTitle(self.winID[which], "Popped out Reset position and size")
                XPLMSetWindowGeometryOS(self.winID[which], 10, 400, 210, 200)
            else:
                XPLMSetWindowTitle(self.winID[which], "Reset position and size")
                XPLMSetWindowGeometry(self.winID[which], 10, 400, 210, 200)

        if self.testSteps[whichFlightLoop] == 15:
            if XPLMWindowIsPoppedOut(self.winID[which]):
                XPLMSetWindowTitle(self.winID[which], "Popped out")
                XPLMSetWindowGeometryOS(self.winID[which], 600, 400, 810, 200)
            else:
                XPLMSetWindowGeometry(self.winID[which], 600, 400, 810, 200)
            self.log("Test set '{}' complete".format(self.flightLoopReferenceConstants[whichFlightLoop]))
            return 0

        self.testSteps[whichFlightLoop] += 1
        return 1.0

    def positioningFlightLoop(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
        whichFlightLoop = 2
        self.log('[{}] flight loop called {:.2f}, {:.2f}, {}'.format(
            self.testSteps[whichFlightLoop],
            elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter))
        codeStep = 0

        which = 0
        if codeStep == self.testSteps[whichFlightLoop]:
            self.createWindow(which)
            XPLMSetWindowTitle(self.winID[which], "Center")
            XPLMSetWindowPositioningMode(self.winID[which], xplm_WindowCenterOnMonitor, -1)

        # codeStep += 2
        # if codeStep == self.testSteps[whichFlightLoop]:
        #     XPLMSetWindowTitle(self.winID[which], "Full")
        #     XPLMSetWindowPositioningMode(self.winID[which], xplm_WindowFullScreenOnMonitor, -1)

        codeStep += 2
        if codeStep == self.testSteps[whichFlightLoop]:
            XPLMSetWindowTitle(self.winID[which], "Popout")
            XPLMSetWindowPositioningMode(self.winID[which], xplm_WindowPopOut, -1)
            self.checkVal('PoppedOut', XPLMWindowIsPoppedOut(self.winID[which]), 1)

        codeStep += 2
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Positioning flight loop called")
            self.log("Test set '{}' complete".format(self.flightLoopReferenceConstants[whichFlightLoop]))
            return 0

        self.testSteps[whichFlightLoop] += 1
        return 1.0

    def gravityFlightLoop(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
        """
        this test possible _only_ if running on main monitor and that monitor is set to Windowed Mode
        Window is displayed, and then tester must manually change the XP window side to observe
        how the position of the popup window changes.
        """
        whichFlightLoop = 1
        self.log('[{}] flight loop called {:.2f}, {:.2f}, {}'.format(
            self.testSteps[whichFlightLoop],
            elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter))
        codeStep = 0

        which = 0
        if codeStep == self.testSteps[whichFlightLoop]:
            self.createWindow(which)
            XPLMSetWindowTitle(self.winID[which], "UpperLeft")
            # [left, top, right, bottom]
            # 1 == top/right
            # 0 == bottom/left
            # 0, 1, 0, 1 == left window relative left edge,
            #               top window relative top,
            #               right window relative left edge,
            #               bottom window relative top
            XPLMSetWindowGravity(self.winID[which], 0, 1, 0, 1)

        codeStep += 2
        if codeStep == self.testSteps[whichFlightLoop]:
            XPLMSetWindowTitle(self.winID[which], "Bottom Right")
            XPLMSetWindowGravity(self.winID[which], 1, 0, 1, 0)

        codeStep += 2
        if codeStep == self.testSteps[whichFlightLoop]:
            XPLMSetWindowTitle(self.winID[which], "Expando")
            XPLMSetWindowGravity(self.winID[which], 1, 1, 1, 1)

        codeStep += 2
        if codeStep == self.testSteps[whichFlightLoop]:
            XPLMSetWindowTitle(self.winID[which], "Center")
            XPLMSetWindowGravity(self.winID[which], .5, .5, .5, .5)

        codeStep += 2
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Gravity flight loop called")
            self.log("Test set '{}' complete".format(self.flightLoopReferenceConstants[whichFlightLoop]))
            return 0

        self.testSteps[whichFlightLoop] += 1
        return 1.0

    def mainFlightLoop(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
        whichFlightLoop = 0
        self.log('[{}] flight loop called {:.2f}, {:.2f}, {}'.format(
            self.testSteps[whichFlightLoop],
            elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter))
        codeStep = 0

        if codeStep == self.testSteps[whichFlightLoop]:
            # try monitor
            self.monitorBounds()
            which = 0
            self.createWindow(which)
            self.winRefcon[self.winID[which]] = [0, 4]
            XPLMSetWindowRefCon(self.winID[which], self.winRefcon[self.winID[which]])

        codeStep += 1
        if self.testSteps[whichFlightLoop] == codeStep:
            which = 0
            self.checkVal('XPLMGetWindowIsVisible(winID)', XPLMGetWindowIsVisible(self.winID[which]), 1)
            self.checkVal('XPLMXWindowIsPoppedOut', XPLMWindowIsPoppedOut(self.winID[which]), 0)
            self.checkVal('XPLMXWindowIsInVR', XPLMWindowIsInVR(self.winID[which]), 0)
            XPLMSetWindowIsVisible(self.winID[which], 0)
            self.log("Is visible, setting to not visible")

        codeStep += 1
        if self.testSteps[whichFlightLoop] == codeStep:
            which = 0
            self.checkVal('XPLMGetWindowIsVisible(winID)', XPLMGetWindowIsVisible(self.winID[which]), 0)
            XPLMSetWindowIsVisible(self.winID[which], 1)
            self.log("Not visible, setting to visible")
            XPLMSetWindowTitle(self.winID[which], "WindowTitle")
            self.checkVal('XPLMGetWindowRefCon', XPLMGetWindowRefCon(self.winID[which]), self.winRefcon[self.winID[which]])

        codeStep += 1
        if self.testSteps[whichFlightLoop] == codeStep:
            which = 0
            self.log("Looking to destroy window [{}] with id: {}".format(which, self.winID[which]))
            XPLMDestroyWindow(self.winID[which])
            del self.winID[which]

        codeStep += 1
        if self.testSteps[whichFlightLoop] == codeStep:
            # self.log("Creating window 0")
            self.createWindow(0)
            # self.log("Creating window 1")
            self.createWindow(1)
            # self.log(" checking ordering")

            self.checkVal('XPLMIsWindowInFront[0]', XPLMIsWindowInFront(self.winID[0]), 0)
            self.checkVal('XPLMIsWindowInFront[1]', XPLMIsWindowInFront(self.winID[1]), 1)
            # self.log("Bringing 0 to front")
            XPLMBringWindowToFront(self.winID[0])
            # self.log('brought 0 to front')
            self.checkVal('XPLMIsWindowInFront[0]', XPLMIsWindowInFront(self.winID[0]), 1)
            self.checkVal('XPLMIsWindowInFront[1]', XPLMIsWindowInFront(self.winID[1]), 0)

            # self.log("XP has keyboard focus? {}".format(XPLMHasKeyboardFocus(0)))

            self.checkVal('XP does not have keyboard focus', XPLMHasKeyboardFocus(0), 1)
            # self.log("taking focus 0")
            XPLMTakeKeyboardFocus(self.winID[0])
            self.checkVal('XPLMHasKeyboardFocus[x-plane]', XPLMHasKeyboardFocus(0), 0)
            self.checkVal('XPLMHasKeyboardFocus[0]', XPLMHasKeyboardFocus(self.winID[0]), 1)
            self.checkVal('XPLMHasKeyboardFocus[1]', XPLMHasKeyboardFocus(self.winID[1]), 0)
            # self.log("taking focus 1")
            XPLMTakeKeyboardFocus(self.winID[1])

            self.checkVal('XPLMHasKeyboardFocus[x-plane]', XPLMHasKeyboardFocus(0), 0)
            self.checkVal('XPLMHasKeyboardFocus[0]', XPLMHasKeyboardFocus(self.winID[0]), 0)
            self.checkVal('XPLMHasKeyboardFocus[1]', XPLMHasKeyboardFocus(self.winID[1]), 1)
            # self.log("giving focus to XP")
            XPLMTakeKeyboardFocus(0)

            self.log("Test set '{}' complete".format(self.flightLoopReferenceConstants[whichFlightLoop]))
            return 0

        self.testSteps[whichFlightLoop] += 1
        return 1.0

    def monitorBounds(self):
        bounds = []
        XPLMGetAllMonitorBoundsOS(self.monitorCallback, bounds)
        # Example: (0, 0, 1440, 2560, 0), (1, 2560, 1840, 3760, -80), (2, -1280, 1368, 0, 344)
        # Note, this includes _all_ monitors, including those not is used by XP
        if not bounds or len(bounds) > 4:
            # I'm testing on 1 to 4 monitors, so make sure we have neither more nor fewer
            self.error('XPLMGetAllMonitorBoundsOS Wrong number of monitors: {}'.format(len(bounds)))
        for bound in bounds:
            left, top, right, bottom = bound[1:]
            if left > right or bottom > top or right - left < 500 or top - bottom < 500:
                self.error("XPLMGetAllMonitorBoundsOS bounds seem wrong for monitor #{}: {}".format(bound[0], bound[1:]))
        self.checkVal('XPLMGetAllMonitorBoundsOS', bounds,
                      ((0, 0, 1440, 2560, 0), (1, 2560, 1840, 3760, -80), (2, -1280, 1368, 0, 344)))

        bounds = []
        XPLMGetAllMonitorBoundsGlobal(self.monitorCallback, bounds)  # bounds = [(monitorIndex, left, top, right, bottom), ]
        # Example: [(2, -1280, 1368, 0, 344)]
        # Note, this includes ONLY those monitors used by XP
        if len(bounds) > 4:
            # I'm testing on 1 to 4 monitors, BUT, this will not return any monitor _not_ running full screen:
            # so if I'm running a single, windowed view of XP, I'll not get any bounds.
            self.error('XPLMGetAllMonitorBoundsGlobal Wrong number of monitors: {}'.format(len(bounds)))
        for bound in bounds:
            left, top, right, bottom = bound[1:]
            if left > right or bottom > top or right - left < 500 or top - bottom < 500:
                self.error("XPLMGetAllMonitorBoundsGlobal bounds seem wrong for monitor #{}: {}".format(bound[0], bound[1:]))

    def screenSize(self):
        # check screen size
        wl = []
        wh = []
        XPLMGetScreenSize(wl, wh)
        if (wl[0] < 1280 or wl[0] > 10000 or wh[0] < 720 or wh[0] > 10000):
            self.error("XPLMGetScreenSize seems wrong: {}".format((wl[0], wh[0])))

        l = []
        t = []
        r = []
        b = []
        XPLMGetScreenBoundsGlobal(l, t, r, b)
        if l[0] >= r[0] or b[0] > t[0] or r[0] - l[0] < 500 or t[0] - b[0] < 500:
            self.error('XPLMGetScreenBoundsGlobal seems wrong: {}'.format((l[0], t[0], r[0], b[0])))
        return [x[0] for x in (l, t, r, b)]

    def createWindow(self, which=0):
        # window position is relative global screen & (0,0) could be anywhere.
        # So get screenSize, and position window relative to that.
        # numbers increase to the right --> and as you go up ^
        (l, t, r, b) = self.screenSize()
        width = 200
        height = 100
        left_offset = 110
        top_offset = 110
        (self.winL, self.winT, self.winR, self.winB) = (l + left_offset + (20 * which),
                                                        t - (top_offset + (20 * which)),
                                                        l + left_offset + width + (20 * which),
                                                        t - (top_offset + height + (20 * which)))

        self.drawWindowFunc = self.drawWindow
        self.handleMouseClickFunc = self.handleMouseClick
        self.handleKeyFunc = self.handleKey
        self.handleCursorFunc = self.handleCursor
        self.handleMouseWheelFunc = self.handleMouseWheel
        self.winRefConByWhich[which] = [which]
        self.log("Setting winrefcon for {} to {}".format(which, self.winRefConByWhich[which]))
        self.winDecorate = xplm_WindowDecorationRoundRectangle
        self.winLayer = xplm_WindowLayerFloatingWindows
        self.handleRightClickFunc = self.handleRightClick
        self.winVis = 1
        pok = [self.winL, self.winT, self.winR, self.winB, self.winVis,
               self.drawWindowFunc, self.handleMouseClickFunc, self.handleKeyFunc,
               self.handleCursorFunc, self.handleMouseWheelFunc, self.winRefConByWhich[which],
               self.winDecorate, self.winLayer, self.handleRightClickFunc]

        self.winID[which] = XPLMCreateWindowEx(pok)
        self.winRefcon[self.winID[which]] = self.winRefConByWhich[which]
        self.log("windows[{}] ID: {} for screensize: {}".format(which, self.winID[which], (l, t, r, b)))
        return

    def drawWindow(self, inWindowID, inRefcon):
        try:
            which = [which for which in self.winID if self.winID[which] == inWindowID][0]
        except IndexError:
            self.error("In drawWindow, but could not determine which for {}, {}".format(inWindowID, self.winID))
        else:
            self.checkRightWindow('drawWindow', which, inWindowID, inRefcon)
        return

    def checkRightWindow(self, prompt, which, inWindowID, inRefcon):
        if (inWindowID == self.winID[which]) and (inRefcon == self.winRefcon[self.winID[which]]):
            return True
        self.error((' ** Error ** {0} unexpected windowID and refcon combination: ' +
                    'got ({1}, {2}), expected ({3}, {4})').format(
                        prompt, inWindowID, inRefcon, self.winID[which], self.winRefcon[self.winID[which]]))
        return

    def drawCallback(self, inPhase, inIsBefore, inRefcon):
        self.drawCallbackCalled = 1
        self.checkVal('drawCallback: Unexpected inPhase', inPhase, self.drawPhase)
        self.checkVal('drawCallback: Unexpected inIsBefore', inIsBefore, self.drawBefore)
        self.checkVal('drawCallback: Unexpected inRefcon', inRefcon, self.drawRefcon)
        return 1

    def handleKey(self, inWindowID, inKey, inFlags, inVirtualKey, inRefcon, losingFocus):
        self.log("handleKey for InWindowsID is {}".format(inWindowID))
        # figure out which of 'our' windows is the one provided in this callback.
        which = [which for which in self.winID if self.winID[which] == inWindowID][0]
        if losingFocus:
            # XPD-10834 on losing focus, windowID is incorrect (appears to send correct refCon, but WindowID 0x0)
            self.log("losing focus, windID:{}, refcon:{}".format(inWindowID, inRefcon))
        else:
            self.checkRightWindow('handleKey', which, inWindowID, inRefcon)
        modifiers = []
        if inFlags & xplm_ShiftFlag:
            modifiers.append('Shift')
        if inFlags & xplm_OptionAltFlag:
            modifiers.append('Alt')
        if inFlags & xplm_ControlFlag:
            modifiers.append('Ctl')
        if inFlags & xplm_DownFlag:
            modifiers.append('Key Down')
        if inFlags & xplm_UpFlag:
            modifiers.append('Key Up')

        modifiers = '-'.join(modifiers)
        self.log("Key {} {}. Virtual: {}, {}".format(inKey,
                                                     modifiers,
                                                     inVirtualKey,
                                                     'losing focus' if losingFocus else ''))
        return

    def handleMouseClick(self, inWindowID, x, y, inMouse, inRefcon):
        which = [which for which in self.winID if self.winID[which] == inWindowID][0]
        self.checkRightWindow('handleMouseClick', which, inWindowID, inRefcon)
        status = {xplm_MouseDown: 'Down',
                  xplm_MouseUp: 'Up',
                  xplm_MouseDrag: 'Drag'}
        self.log("Mouse click ({}, {}), status: {}".format(x, y, status.get(inMouse, 'Unknown')))
        return 1

    def handleRightClick(self, inWindowID, x, y, inMouse, inRefcon):
        which = [which for which in self.winID if self.winID[which] == inWindowID][0]
        self.checkRightWindow('handleMouseClick', which, inWindowID, inRefcon)
        status = {xplm_MouseDown: 'Down',
                  xplm_MouseUp: 'Up',
                  xplm_MouseDrag: 'Drag'}
        self.log("Mouse RIGHT click ({}, {}), status: {}".format(x, y, status.get(inMouse, 'Unknown')))
        return 1

    def handleCursor(self, inWindowID, x, y, inRefcon):
        which = [which for which in self.winID if self.winID[which] == inWindowID][0]
        self.checkRightWindow('handleCursor', which, inWindowID, inRefcon)
        return xplm_CursorDefault

    def handleMouseWheel(self, inWindowID, x, y, wheel, clicks, inRefcon):
        which = [which for which in self.winID if self.winID[which] == inWindowID][0]
        self.checkRightWindow('handleMouseWheel', which, inWindowID, inRefcon)
        self.log("Mouse wheel ({}, {}), {} #{} clicks".format(
            x, y, 'vertical' if wheel == 0 else 'horizontal', clicks))
        return 1

    def monitorCallback(self, inMonitorIndex, inLeftBx, inTopBx, inRightBx, inBottomBx, refcon):
        refcon.append((inMonitorIndex, inLeftBx, inTopBx, inRightBx, inBottomBx))
