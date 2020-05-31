from regression_base import RegressionBase

from XPLMGraphics import XPLMLocalToWorld

from XPLMCamera import XPLMControlCamera, XPLMDontControlCamera, \
    XPLMIsCameraBeingControlled, XPLMReadCameraPosition, \
    xplm_ControlCameraUntilViewChanges, xplm_ControlCameraForever

"""
On success, camera will rotate, once, 360 degrees.
XPPythonLog will return current camera position similar to:
[xppython3.camera] Current camera position: (39.895, -104.696) at 1657.8 meters
"""


class PythonInterface(RegressionBase):
    def __init__(self):
        self.Sig = "xppython3.camera"
        self.Name = "Regression Test {}".format(self.Sig)
        self.Desc = "{} test".format(self.Sig)

        super(PythonInterface, self).__init__(self.Sig)

    def XPluginStart(self):
        TEST_TO_RUN = 0
        whichFlightLoop = TEST_TO_RUN
        self.startFlightLoop(whichFlightLoop)
        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        super(PythonInterface, self).stop()
        self.check()

    def XPluginEnable(self):
        super(PythonInterface, self).enable()
        return 1

    def XPluginDisable(self):
        super(PythonInterface, self).disable()

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        super(PythonInterface, self).receive(inFromWho, inMessage, inParam)

    def cameraControl(self, outCameraPosition, inIsLosingControl, inRefcon):
        self.checkVal('camera refcon', inRefcon, ['camera', ])
        if inIsLosingControl:
            self.log("Camera losing control")
            return 0
        outCameraPosition[4] += 1  # change heading by one degree each call.
        if outCameraPosition[4] >= 360:
            XPLMDontControlCamera()
        return 1

    def flightLoopCallback(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
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
        self.log("Flight Loop #{} unknown".format(whichFlightLoop))
        return 0

    def mainFlightLoop(self, elapsedSinceLastCall, elapsedTimeSinceLastFlightLoop, counter, inRefcon):
        whichFlightLoop = 0
        self.testSteps[whichFlightLoop] += 1
        self.log('[{}] flight loop called {:.2f}, {:.2f}, {}'.format(self.testSteps[whichFlightLoop],
                                                                     elapsedSinceLastCall,
                                                                     elapsedTimeSinceLastFlightLoop, counter))
        codeStep = 0
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Step {}".format(codeStep))
            self.checkVal('Camera being controlled', XPLMIsCameraBeingControlled()[0], 0)

        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            (x, y, z, pitch, heading, roll, zoom) = XPLMReadCameraPosition()
            lat, lng, alt = XPLMLocalToWorld(x, y, z)
            self.log("Current camera position: ({}, {}) at {} meters".format(lat, lng, alt))
            self.log("Step {}".format(codeStep))
            self.cameraControlFun = self.cameraControl
            XPLMControlCamera(xplm_ControlCameraUntilViewChanges, self.cameraControlFun, ['camera', ])
        codeStep += 1
        if codeStep == self.testSteps[whichFlightLoop]:
            self.log("Test Complete")
            return 0

        return 1.0

    def drawCallback(self, inPhase, inIsBefore, inRefcon):
        self.drawCallbackCalled = 1
        self.checkVal('drawCallback: Unexpected inPhase', inPhase, self.drawPhase)
        self.checkVal('drawCallback: Unexpected inIsBefore', inIsBefore, self.drawBefore)
        self.checkVal('drawCallback: Unexpected inRefcon', inRefcon, self.drawRefcon)
        return 1
