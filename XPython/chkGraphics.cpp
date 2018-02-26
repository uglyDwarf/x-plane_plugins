#include <iostream>
#include <map>
#include <list>
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
static std::list<XPLMDataRef> d;

void initGraphicsModule(void)
{
  d.push_back(registerROAccessor("enableFog", enableFog));
  d.push_back(registerROAccessor("numTexUnits", numTexUnits));
  d.push_back(registerROAccessor("enableLighting", enableLighting));
  d.push_back(registerROAccessor("enableAlphaTest", enableAlphaTest));
  d.push_back(registerROAccessor("enableAlphaBlend", enableAlphaBlend));
  d.push_back(registerROAccessor("enableDepthTest", enableDepthTest));
  d.push_back(registerROAccessor("enableDepthWrite", enableDepthWrite));
  d.push_back(registerROAccessor("texNum", texNum));
  d.push_back(registerROAccessor("texUnitNum", texUnitNum));
  d.push_back(registerROAccessor("int0", int0));
  d.push_back(registerROAccessor("int1", int1));
  d.push_back(registerROAccessor("int2", int2));
  d.push_back(registerROAccessor("int3", int3));
  d.push_back(registerROAccessor("int4", int4));
  d.push_back(registerROAccessor("int5", int5));
  d.push_back(registerROAccessor("float0", float0));
  d.push_back(registerROAccessor("float1", float1));
  d.push_back(registerROAccessor("float2", float2));
  d.push_back(registerROAccessor("str0", str0));
  d.push_back(registerROAccessor("double0", double0));

}

void cleanupGraphicsModule(void)
{
  for(std::list<XPLMDataRef>::iterator i = d.begin(); i != d.end(); ++i){
    XPLMUnregisterDataAccessor(*i);
  }
  d.empty();
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

