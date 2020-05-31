#define _GNU_SOURCE 1
#include <Python.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>

#include <XPLM/XPLMDefs.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPStandardWidgets.h>

static PyObject *cleanup(PyObject *self, PyObject *args)
{
  (void) self;
  (void) args;
  Py_RETURN_NONE;
}


static PyMethodDef XPStandardWidgetsMethods[] = {
  {"cleanup", cleanup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};


static struct PyModuleDef XPStandardWidgetsModule = {
  PyModuleDef_HEAD_INIT,
  "XPStandardWidgets",
  NULL,
  -1,
  XPStandardWidgetsMethods,
  NULL,
  NULL,
  NULL,
  NULL
};

PyMODINIT_FUNC
PyInit_XPStandardWidgets(void)
{
  PyObject *mod = PyModule_Create(&XPStandardWidgetsModule);
  if(mod){
    PyModule_AddIntConstant(mod, "xpWidgetClass_MainWindow", xpWidgetClass_MainWindow);
    PyModule_AddIntConstant(mod, "xpMainWindowStyle_MainWindow", xpMainWindowStyle_MainWindow);
    PyModule_AddIntConstant(mod, "xpMainWindowStyle_Translucent", xpMainWindowStyle_Translucent);
    PyModule_AddIntConstant(mod, "xpProperty_MainWindowType", xpProperty_MainWindowType);
    PyModule_AddIntConstant(mod, "xpProperty_MainWindowHasCloseBoxes", xpProperty_MainWindowHasCloseBoxes);
    PyModule_AddIntConstant(mod, "xpMessage_CloseButtonPushed", xpMessage_CloseButtonPushed);
    PyModule_AddIntConstant(mod, "xpWidgetClass_SubWindow", xpWidgetClass_SubWindow);
    PyModule_AddIntConstant(mod, "xpSubWindowStyle_SubWindow", xpSubWindowStyle_SubWindow);
    PyModule_AddIntConstant(mod, "xpSubWindowStyle_Screen", xpSubWindowStyle_Screen);
    PyModule_AddIntConstant(mod, "xpSubWindowStyle_ListView", xpSubWindowStyle_ListView);
    PyModule_AddIntConstant(mod, "xpProperty_SubWindowType", xpProperty_SubWindowType);
    PyModule_AddIntConstant(mod, "xpWidgetClass_Button", xpWidgetClass_Button);
    PyModule_AddIntConstant(mod, "xpPushButton", xpPushButton);
    PyModule_AddIntConstant(mod, "xpRadioButton", xpRadioButton);
    PyModule_AddIntConstant(mod, "xpWindowCloseBox", xpWindowCloseBox);
    PyModule_AddIntConstant(mod, "xpLittleDownArrow", xpLittleDownArrow);
    PyModule_AddIntConstant(mod, "xpLittleUpArrow", xpLittleUpArrow);
    PyModule_AddIntConstant(mod, "xpButtonBehaviorPushButton", xpButtonBehaviorPushButton);
    PyModule_AddIntConstant(mod, "xpButtonBehaviorCheckBox", xpButtonBehaviorCheckBox);
    PyModule_AddIntConstant(mod, "xpButtonBehaviorRadioButton", xpButtonBehaviorRadioButton);
    PyModule_AddIntConstant(mod, "xpProperty_ButtonType", xpProperty_ButtonType);
    PyModule_AddIntConstant(mod, "xpProperty_ButtonBehavior", xpProperty_ButtonBehavior);
    PyModule_AddIntConstant(mod, "xpProperty_ButtonState", xpProperty_ButtonState);
    PyModule_AddIntConstant(mod, "xpMsg_PushButtonPressed", xpMsg_PushButtonPressed);
    PyModule_AddIntConstant(mod, "xpMsg_ButtonStateChanged", xpMsg_ButtonStateChanged);
    PyModule_AddIntConstant(mod, "xpWidgetClass_TextField", xpWidgetClass_TextField);
    PyModule_AddIntConstant(mod, "xpTextEntryField", xpTextEntryField);
    PyModule_AddIntConstant(mod, "xpTextTransparent", xpTextTransparent);
    PyModule_AddIntConstant(mod, "xpTextTranslucent", xpTextTranslucent);
    PyModule_AddIntConstant(mod, "xpProperty_EditFieldSelStart", xpProperty_EditFieldSelStart);
    PyModule_AddIntConstant(mod, "xpProperty_EditFieldSelEnd", xpProperty_EditFieldSelEnd);
    PyModule_AddIntConstant(mod, "xpProperty_EditFieldSelDragStart", xpProperty_EditFieldSelDragStart);
    PyModule_AddIntConstant(mod, "xpProperty_TextFieldType", xpProperty_TextFieldType);
    PyModule_AddIntConstant(mod, "xpProperty_PasswordMode", xpProperty_PasswordMode);
    PyModule_AddIntConstant(mod, "xpProperty_MaxCharacters", xpProperty_MaxCharacters);
    PyModule_AddIntConstant(mod, "xpProperty_ScrollPosition", xpProperty_ScrollPosition);
    PyModule_AddIntConstant(mod, "xpProperty_Font", xpProperty_Font);
    PyModule_AddIntConstant(mod, "xpProperty_ActiveEditSide", xpProperty_ActiveEditSide);
    PyModule_AddIntConstant(mod, "xpMsg_TextFieldChanged", xpMsg_TextFieldChanged);
    PyModule_AddIntConstant(mod, "xpWidgetClass_ScrollBar", xpWidgetClass_ScrollBar);
    PyModule_AddIntConstant(mod, "xpScrollBarTypeScrollBar", xpScrollBarTypeScrollBar);
    PyModule_AddIntConstant(mod, "xpScrollBarTypeSlider", xpScrollBarTypeSlider);
    PyModule_AddIntConstant(mod, "xpProperty_ScrollBarSliderPosition", xpProperty_ScrollBarSliderPosition);
    PyModule_AddIntConstant(mod, "xpProperty_ScrollBarMin", xpProperty_ScrollBarMin);
    PyModule_AddIntConstant(mod, "xpProperty_ScrollBarMax", xpProperty_ScrollBarMax);
    PyModule_AddIntConstant(mod, "xpProperty_ScrollBarPageAmount", xpProperty_ScrollBarPageAmount);
    PyModule_AddIntConstant(mod, "xpProperty_ScrollBarType", xpProperty_ScrollBarType);
    PyModule_AddIntConstant(mod, "xpProperty_ScrollBarSlop", xpProperty_ScrollBarSlop);
    PyModule_AddIntConstant(mod, "xpMsg_ScrollBarSliderPositionChanged", xpMsg_ScrollBarSliderPositionChanged);
    PyModule_AddIntConstant(mod, "xpWidgetClass_Caption", xpWidgetClass_Caption);
    PyModule_AddIntConstant(mod, "xpProperty_CaptionLit", xpProperty_CaptionLit);
    PyModule_AddIntConstant(mod, "xpWidgetClass_GeneralGraphics", xpWidgetClass_GeneralGraphics);
    PyModule_AddIntConstant(mod, "xpShip", xpShip);
    PyModule_AddIntConstant(mod, "xpILSGlideScope", xpILSGlideScope);
    PyModule_AddIntConstant(mod, "xpMarkerLeft", xpMarkerLeft);
    PyModule_AddIntConstant(mod, "xp_Airport", xp_Airport);
    PyModule_AddIntConstant(mod, "xpNDB", xpNDB);
    PyModule_AddIntConstant(mod, "xpVOR", xpVOR);
    PyModule_AddIntConstant(mod, "xpRadioTower", xpRadioTower);
    PyModule_AddIntConstant(mod, "xpAircraftCarrier", xpAircraftCarrier);
    PyModule_AddIntConstant(mod, "xpFire", xpFire);
    PyModule_AddIntConstant(mod, "xpMarkerRight", xpMarkerRight);
    PyModule_AddIntConstant(mod, "xpCustomObject", xpCustomObject);
    PyModule_AddIntConstant(mod, "xpCoolingTower", xpCoolingTower);
    PyModule_AddIntConstant(mod, "xpSmokeStack", xpSmokeStack);
    PyModule_AddIntConstant(mod, "xpBuilding", xpBuilding);
    PyModule_AddIntConstant(mod, "xpPowerLine", xpPowerLine);
    PyModule_AddIntConstant(mod, "xpVORWithCompassRose", xpVORWithCompassRose);
    PyModule_AddIntConstant(mod, "xpOilPlatform", xpOilPlatform);
    PyModule_AddIntConstant(mod, "xpOilPlatformSmall", xpOilPlatformSmall);
    PyModule_AddIntConstant(mod, "xpWayPoint", xpWayPoint);
    PyModule_AddIntConstant(mod, "xpProperty_GeneralGraphicsType", xpProperty_GeneralGraphicsType);
    PyModule_AddIntConstant(mod, "xpWidgetClass_Progress", xpWidgetClass_Progress);
    PyModule_AddIntConstant(mod, "xpProperty_ProgressPosition", xpProperty_ProgressPosition);
    PyModule_AddIntConstant(mod, "xpProperty_ProgressMin", xpProperty_ProgressMin);
    PyModule_AddIntConstant(mod, "xpProperty_ProgressMax", xpProperty_ProgressMax);
  }

  return mod;
}



