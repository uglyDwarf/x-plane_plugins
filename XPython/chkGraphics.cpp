#include <iostream>
#include <map>
#include <vector>
#include <dlfcn.h>
#include <string.h>
#define XPLM200
#define XPLM210
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMGraphics.h>

#include "chkGraphics.h"
#include "chk_helper.h"


void XPLMSetGraphicsState(int inEnableFog, int inNumberTexUnits, int inEnableLighting,
                       int inEnableAlphaTesting, int inEnableAlphaBlending,
                       int inEnableDepthTesting, int inEnableDepthWriting)
{
  /*std::cout << inEnableFog << ", " << inNumberTexUnits << ", " << inEnableLighting <<
            ", " << inEnableAlphaTesting << ", " << inEnableAlphaBlending << ", " <<
            inEnableDepthTesting << ", " << inEnableDepthWriting << std::endl;
  */
  add_int_value(std::string("XPLMSetGraphicsState:inEnableFog"), inEnableFog);
  add_int_value(std::string("XPLMSetGraphicsState:inNumberTexUnits"), inNumberTexUnits);
  add_int_value(std::string("XPLMSetGraphicsState:inEnableLighting"), inEnableLighting);
  add_int_value(std::string("XPLMSetGraphicsState:inEnableAlphaTesting"), inEnableAlphaTesting);
  add_int_value(std::string("XPLMSetGraphicsState:inEnableAlphaBlending"), inEnableAlphaBlending);
  add_int_value(std::string("XPLMSetGraphicsState:inEnableDepthTesting"), inEnableDepthTesting);
  add_int_value(std::string("XPLMSetGraphicsState:inEnableDepthWriting"), inEnableDepthWriting);
}

void XPLMBindTexture2d(int inTextureNum, int inTextureUnit)
{
  //std::cout << inTextureNum << ", " << inTextureUnit << std::endl;
  add_int_value(std::string("XPLMBindTexture2d:inTextureNum"), inTextureNum);
  add_int_value(std::string("XPLMBindTexture2d:inTextureUnit"), inTextureUnit);
}

void XPLMGenerateTextureNumbers(int *outTextureIDs, int inCount)
{
  for(int i = 42; i < 42 + inCount; ++i){
    outTextureIDs[i - 42] = i;
  }
}

int XPLMGetTexture(XPLMTextureID inTexture)
{
  return inTexture + 42;
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
  //std::cout << inLeft << ", " << inTop << ", " << inRight << ", " << inBottom << std::endl;
  add_int_value(std::string("XPLMDrawTranslucentDarkBox:inLeft"), inLeft);
  add_int_value(std::string("XPLMDrawTranslucentDarkBox:inTop"), inTop);
  add_int_value(std::string("XPLMDrawTranslucentDarkBox:inRight"), inRight);
  add_int_value(std::string("XPLMDrawTranslucentDarkBox:inBottom"), inBottom);
}

void XPLMDrawString(float *inColorRGB, int inXOffset, int inYOffset, char *inChar,
                    int *inWordWrapWidth, XPLMFontID inFontID)
{
  /*std::cout << "Draw '" << inChar << "' at (" << inXOffset << ", " << inYOffset << ")" << std::endl;
  std::cout << "  FontID " << inFontID << " Color (" << inColorRGB[0] << ", " << inColorRGB[1] << ", " <<
               inColorRGB[2] << ") WordWrapWidth " << *inWordWrapWidth << std::endl;
  */
  add_double_value(std::string("XPLMDrawString:inColorRGB[0]"), inColorRGB[0]);
  add_double_value(std::string("XPLMDrawString:inColorRGB[1]"), inColorRGB[1]);
  add_double_value(std::string("XPLMDrawString:inColorRGB[2]"), inColorRGB[2]);
  add_int_value(std::string("XPLMDrawString:inXOffset"), inXOffset);
  add_int_value(std::string("XPLMDrawString:inYOffset"), inYOffset);
  add_str_value(std::string("XPLMDrawString:inChar"), inChar);
  add_int_value(std::string("XPLMDrawString:inWordWrapWidth"), *inWordWrapWidth);
  add_int_value(std::string("XPLMDrawString:inFontID"), inFontID);
}

void XPLMDrawNumber(float *inColorRGB, int inXOffset, int inYOffset, double inValue, int inDigits,
                    int inDecimals, int inShowSign, XPLMFontID inFontID)
{
  /*std::cout << "Draw '" << inValue << "' at (" << inXOffset << ", " << inYOffset << ")" << std::endl;
  std::cout << "  FontID " << inFontID << " Color (" << inColorRGB[0] << ", " << inColorRGB[1] << ", " <<
               inColorRGB[2] << ") Decimals" << inDecimals << " Digits " << inDigits << 
               " ShowSign " << inShowSign << std::endl;
  */
  add_double_value(std::string("XPLMDrawNumber:inColorRGB[0]"), inColorRGB[0]);
  add_double_value(std::string("XPLMDrawNumber:inColorRGB[1]"), inColorRGB[1]);
  add_double_value(std::string("XPLMDrawNumber:inColorRGB[2]"), inColorRGB[2]);
  add_int_value(std::string("XPLMDrawNumber:inXOffset"), inXOffset);
  add_int_value(std::string("XPLMDrawNumber:inYOffset"), inYOffset);
  add_double_value(std::string("XPLMDrawNumber:inValue"), inValue);
  add_int_value(std::string("XPLMDrawNumber:inDigits"), inDigits);
  add_int_value(std::string("XPLMDrawNumber:inDecimals"), inDecimals);
  add_int_value(std::string("XPLMDrawNumber:inShowSign"), inShowSign);
  add_int_value(std::string("XPLMDrawNumber:inFontID"), inFontID);
 }

void XPLMGetFontDimensions(XPLMFontID inFontID, int *outCharWidth, int *outCharHeight, int *outDigitsOnly)
{
  *outCharWidth = inFontID + 333;
  *outCharHeight = inFontID + 444;
  *outDigitsOnly = inFontID + 555;
}

float XPLMMeasureString(XPLMFontID inFontID, const char *inChar, int inNumChars)
{
  return inFontID + strlen(inChar) + inNumChars + 3.14;
}

void checkGraphicsModule()
{
}
