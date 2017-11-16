#include <iostream>
#include <map>
#include <vector>
#include <dlfcn.h>
#include <string.h>
#define XPLM200
#define XPLM210
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>
#include <XPLM/XPLMGraphics.h>

#include "chkGraphics.h"

#include "chk_helper.h"


static int enableFog;
static int numTexUnits;
static int enableLighting;
static int enableAlphaTest;
static int enableAlphaBlend;
static int enableDepthTest;
static int enableDepthWrite;
static int texNum;
static int texUnitNum;

static int int0;
static int int1;
static int int2;
static int int3;
static int int4;
static int int5;

static float float0;
static float float1;
static float float2;

static double double0;

static std::string str0;

void initGraphicsModule(void)
{
  registerROAccessor("enableFog", enableFog);
  registerROAccessor("numTexUnits", numTexUnits);
  registerROAccessor("enableLighting", enableLighting);
  registerROAccessor("enableAlphaTest", enableAlphaTest);
  registerROAccessor("enableAlphaBlend", enableAlphaBlend);
  registerROAccessor("enableDepthTest", enableDepthTest);
  registerROAccessor("enableDepthWrite", enableDepthWrite);
  registerROAccessor("texNum", texNum);
  registerROAccessor("texUnitNum", texUnitNum);
  registerROAccessor("int0", int0);
  registerROAccessor("int1", int1);
  registerROAccessor("int2", int2);
  registerROAccessor("int3", int3);
  registerROAccessor("int4", int4);
  registerROAccessor("int5", int5);
  registerROAccessor("float0", float0);
  registerROAccessor("float1", float1);
  registerROAccessor("float2", float2);
  registerROAccessor("str0", str0);
  registerROAccessor("double0", double0);

}

void XPLMSetGraphicsState(int inEnableFog, int inNumberTexUnits, int inEnableLighting,
                       int inEnableAlphaTesting, int inEnableAlphaBlending,
                       int inEnableDepthTesting, int inEnableDepthWriting)
{
  enableFog = inEnableFog;
  numTexUnits = inNumberTexUnits;
  enableLighting = inEnableLighting;
  enableAlphaTest = inEnableAlphaTesting;
  enableAlphaBlend = inEnableAlphaBlending;
  enableDepthTest = inEnableDepthTesting;
  enableDepthWrite = inEnableDepthWriting;
}

void XPLMBindTexture2d(int inTextureNum, int inTextureUnit)
{
  //std::cout << inTextureNum << ", " << inTextureUnit << std::endl;
  texNum = inTextureNum;
  texUnitNum = inTextureUnit;
}

void XPLMGenerateTextureNumbers(int *outTextureIDs, int inCount)
{
  for(int i = 42; i < 42 + inCount; ++i){
    outTextureIDs[i - 42] = i;
  }
}

int XPLMGetTexture(XPLMTextureID inTexture)
{
  return inTexture + 43;
}

void XPLMWorldToLocal(double inLatitude, double inLongitude, double inAltitude,
                      double *outX, double *outY, double *outZ)
{
  *outX = inLatitude + 1.23;
  *outY = inLongitude + 2.34;
  *outZ = inAltitude + 3.45;
}

void XPLMLocalToWorld(double inX, double inY, double inZ,
                      double *outLatitude, double *outLongitude, double *outAltitude)
{
  *outLatitude = inX + 4.56;
  *outLongitude = inY + 5.67;
  *outAltitude = inZ + 6.78;
}

void XPLMDrawTranslucentDarkBox(int inLeft, int inTop, int inRight, int inBottom)
{
  int0 = inLeft;
  int1 = inTop;
  int2 = inRight;
  int3 = inBottom;
}

void XPLMDrawString(float *inColorRGB, int inXOffset, int inYOffset, char *inChar,
                    int *inWordWrapWidth, XPLMFontID inFontID)
{
  float0 = inColorRGB[0];
  float1 = inColorRGB[1];
  float2 = inColorRGB[2];
  int0 = inXOffset;
  int1 = inYOffset;
  str0 = inChar;
  if(inWordWrapWidth != NULL){
    int2 = *inWordWrapWidth;
  }else{
    int2 = -1;
  }
  int3 = inFontID;
}

void XPLMDrawNumber(float *inColorRGB, int inXOffset, int inYOffset, double inValue, int inDigits,
                    int inDecimals, int inShowSign, XPLMFontID inFontID)
{
  float0 = inColorRGB[0];
  float1 = inColorRGB[1];
  float2 = inColorRGB[2];
  int0 = inXOffset;
  int1 = inYOffset;
  double0 = inValue;
  int2 = inDigits;
  int3 = inDecimals;
  int4 = inShowSign;
  int5 = inFontID;
}

void XPLMGetFontDimensions(XPLMFontID inFontID, int *outCharWidth, int *outCharHeight, int *outDigitsOnly)
{
  *outCharWidth = inFontID + 333;
  *outCharHeight = inFontID + 444;
  *outDigitsOnly = inFontID + 555;
}

float XPLMMeasureString(XPLMFontID inFontID, const char *inChar, int inNumChars)
{
  str0 = inChar;
  int0 = inNumChars;
  return inFontID + 3.14;
}

