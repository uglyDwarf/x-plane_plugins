#!/usr/bin/env python3
#
from check_helper import *

from XPLMDefs import *
from XPLMDataAccess import *
from XPLMMap import *
from XPLMUtilities import *

class PythonInterface(checkBase):
   def __init__(self):
      checkBase.__init__(self, 'Map');
      checkBase.addRef()
   
   def XPluginStart(self):
      self.Name = "Map regression test"
      self.Sig = "MapRT"
      self.Desc = "Regression test for XPLMMap module"
      self.versions = XPLMGetVersions()

      return self.Name, self.Sig, self.Desc
   
   def XPluginStop(self):
      self.check()
      checkBase.remRef()
   
   def XPluginEnable(self):
      return 1
   
   def XPluginDisable(self):
      return

   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      if self.versions[1] < 300:
         return 1.0

      self.str0Dref = XPLMFindDataRef('map.str0')
      self.int0Dref = XPLMFindDataRef('map.int0')
      self.int1Dref = XPLMFindDataRef('map.int1')
      self.int2Dref = XPLMFindDataRef('map.int2')
      self.int3Dref = XPLMFindDataRef('map.int3')
      self.int4Dref = XPLMFindDataRef('map.int4')
      self.float0Dref = XPLMFindDataRef('map.float0')
      self.float1Dref = XPLMFindDataRef('map.float1')
      self.float2Dref = XPLMFindDataRef('map.float2')
      self.float3Dref = XPLMFindDataRef('map.float3')

      self.mapCreationList = []
      XPLMRegisterMapCreationHook(self.mapCreationCallback, self.mapCreationList)

      self.layerList = []
      self.params = ("En-route map", 5544, self.willBeDeletedCallback, 
                     self.prepCacheCallback, self.drawCallback, self.iconCallback,
                     self.labelCallback, 5545, "Superlayer", self.layerList)
      self.layer = XPLMCreateMapLayer(self, self.params)
      self.checkVal('XPLMCreateMapLayer:layerType', XPLMGetDatai(self.int1Dref), self.params[1])
      self.checkVal('XPLMCreateMapLayer:layerName', self.getString(self.str0Dref), self.params[8])

      mapId = 'Hi-Alt Map'
      res = XPLMMapExists(mapId)
      self.checkVal('XPLMMapExists:mapIdentifier', self.getString(self.str0Dref), mapId)
      self.checkVal('XPLMMapExists:res', res, 32769)

      res = XPLMDestroyMapLayer(self, self.layer)
      self.checkVal('XPLMDestroyMapLayer:res', res, 65537)
      self.checkVal('Callbacks', self.layerList, ['Preparing cache', 'Drawing', 'Icon', 'Label', 'Layer will be deleted'])
      self.checkVal('Callbacks', self.mapCreationList, ["Map En-route map created"])

      return 1.0

   def mapCreationCallback(self, mapIdentifier, inRefcon):
      inRefcon.append("Map {} created".format(mapIdentifier))
      return

   def willBeDeletedCallback(self, inLayer, inRefcon):
      self.checkVal('willBeDeletedCallback:inLayer', inLayer, self.layer)
      inRefcon.append("Layer will be deleted")
      return

   def prepCacheCallback(self, inLayer, inTotalMapBoundsLeftTopRightBottom, projection, inRefcon):
      inRefcon.append("Preparing cache")
      b = self.params[7]
      b1 = self.params[1]
      self.checkVal('prepCacheCallback.inLayer', inLayer, self.layer)
      self.checkVal('prepCacheCallback:inTotalMapBoundsLeftTopRightBottom',
                    inTotalMapBoundsLeftTopRightBottom, [b + 1, b + 2, b + 3, b + 4])
      self.checkVal('prepCacheCallback:projection', projection, b1 + 5)

   def drawCallback(self, inLayer, inMapBoundsLeftTopRightBottom, zoomRatio,
                    mapUnitsPerUserInterfaceUnit, mapStyle, projection, inRefcon):
      inRefcon.append("Drawing")
      b = self.params[7] + 11
      b1 = self.params[1] + 11
      self.checkVal('drawCallback.inLayer', inLayer, self.layer)
      self.checkVal('drawCallback:inMapBoundsLeftTopRightBottom',
                    inMapBoundsLeftTopRightBottom, [b + 1, b + 2, b + 3, b + 4])
      self.checkVal('drawCallback:zoomRatio', zoomRatio, b1 + 6)
      self.checkVal('drawCallback:mapUnitsPerUserInterfaceUnit', mapUnitsPerUserInterfaceUnit, b1 + 10)
      self.checkVal('drawCallback:mapStyle', mapStyle, b1 + 13)
      self.checkVal('drawCallback:projection', projection, b1 + 5)

      projection = 76543
      latitude = 33.6
      longitude = 51.3
      (outX, outY) = ([], [])
      XPLMMapProject(projection, latitude, longitude, outX, outY)
      self.checkVal('XPLMMapProject:projection', XPLMGetDatai(self.int0Dref), projection)
      self.checkVal('XPLMMapProject:rest', (outX[0], outY[0]), (latitude * 2.5,  longitude * 3.6))
   
      projection = 76543
      (mapX, mapY) = (54.8, 32.1)
      (outLatitude, outLongitude) = ([], [])
      XPLMMapUnproject(projection, mapX, mapY, outLatitude, outLongitude)
      self.checkVal('XPLMMapUnproject:projection', XPLMGetDatai(self.int0Dref), projection)
      self.checkVal('XPLMMapUnproject:rest', (outLatitude[0], outLongitude[0]), (mapY * 4.7,  mapX * 5.8))

      projection = 6543
      (mapX, mapY) = (4.8, 2.1)
      res = XPLMMapScaleMeter(projection, mapX, mapY)
      self.checkVal('XPLMMapScaleMeter:projection', XPLMGetDatai(self.int0Dref), projection)
      self.checkVal('XPLMMapScaleMeter:rest', res, 2 * mapX - mapY)

      projection = 4321
      (mapX, mapY) = (64.8, 12.1)
      res = XPLMMapGetNorthHeading(projection, mapX, mapY)
      self.checkVal('XPLMMapGetNorthHeading:projection', XPLMGetDatai(self.int0Dref), projection)
      self.checkVal('XPLMMapGetNorthHeading:rest', res, 2 * mapY - mapX)

   def iconCallback(self, inLayer, inMapBoundsLeftTopRightBottom, zoomRatio,
                    mapUnitsPerUserInterfaceUnit, mapStyle, projection, inRefcon):
      inRefcon.append("Icon")
      b = self.params[7] + 22
      b1 = self.params[1] + 22
      self.checkVal('iconCallback.inLayer', inLayer, self.layer)
      self.checkVal('iconCallback:inMapBoundsLeftTopRightBottom',
                    inMapBoundsLeftTopRightBottom, [b + 1, b + 2, b + 3, b + 4])
      self.checkVal('iconCallback:zoomRatio', zoomRatio, b1 + 6)
      self.checkVal('iconCallback:mapUnitsPerUserInterfaceUnit', mapUnitsPerUserInterfaceUnit, b1 + 10)
      self.checkVal('iconCallback:mapStyle', mapStyle, b1 + 13)
      self.checkVal('iconCallback:projection', projection, b1 + 5)

      inPngPath = 'Resources/icons.png'
      (s, t, ds, dt) = (335, 224, 16, 32)
      (mapX, mapY) = (554.3, 665.4)
      orientation = 776
      rotationDegrees = 16.8
      mapWidth = 25643.2
      XPLMDrawMapIconFromSheet(inLayer, inPngPath, s, t, ds, dt, mapX, mapY, orientation,
                               rotationDegrees, mapWidth)
      self.checkVal('XPLMDrawMapIconFromSheet:inPngPath', self.getString(self.str0Dref), inPngPath)
      self.checkVal('XPLMDrawMapIconFromSheet:s,t,ds,dt', 
                    (XPLMGetDatai(self.int0Dref), XPLMGetDatai(self.int1Dref),
                     XPLMGetDatai(self.int2Dref), XPLMGetDatai(self.int3Dref)), (s, t, ds, dt))
      self.checkVal('XPLMDrawMapIconFromSheet:mapX,mapY',
                    (XPLMGetDataf(self.float0Dref), XPLMGetDataf(self.float1Dref)), (mapX, mapY))
      self.checkVal('XPLMDrawMapIconFromSheet:orientation', XPLMGetDatai(self.int4Dref), orientation)
      self.checkVal('XPLMDrawMapIconFromSheet:rotationDegrees', XPLMGetDataf(self.float2Dref), rotationDegrees)
      self.checkVal('XPLMDrawMapIconFromSheet:mapWidth', XPLMGetDataf(self.float3Dref), mapWidth)

   def labelCallback(self, inLayer, inMapBoundsLeftTopRightBottom, zoomRatio,
                    mapUnitsPerUserInterfaceUnit, mapStyle, projection, inRefcon):
      inRefcon.append("Label")
      b = self.params[7] + 33
      b1 = self.params[1] + 33
      self.checkVal('labelCallback.inLayer', inLayer, self.layer)
      self.checkVal('labelCallback:inMapBoundsLeftTopRightBottom',
                    inMapBoundsLeftTopRightBottom, [b + 1, b + 2, b + 3, b + 4])
      self.checkVal('labelCallback:zoomRatio', zoomRatio, b1 + 6)
      self.checkVal('labelCallback:mapUnitsPerUserInterfaceUnit', mapUnitsPerUserInterfaceUnit, b1 + 10)
      self.checkVal('labelCallback:mapStyle', mapStyle, b1 + 13)
      self.checkVal('labelCallback:projection', projection, b1 + 5)

      inText = 'Resources/icons.png'
      (mapX, mapY) = (123.2, 657.7)
      orientation = 345
      rotationDegrees = 24.5
      XPLMDrawMapLabel(inLayer, inText, mapX, mapY, orientation, rotationDegrees)
      self.checkVal('XPLMDrawMapLabel:inText', self.getString(self.str0Dref), inText)
      self.checkVal('XPLMDrawMapLabel:mapX,mapY',
                    (XPLMGetDataf(self.float0Dref), XPLMGetDataf(self.float1Dref)), (mapX, mapY))
      self.checkVal('XPLMDrawMapLabel:orientation', XPLMGetDatai(self.int0Dref), orientation)
      self.checkVal('XPLMDrawMapLabel:rotationDegrees', XPLMGetDataf(self.float2Dref), rotationDegrees)


