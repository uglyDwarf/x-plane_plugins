#!/usr/bin/env python3
#
from XPLMCHKHelper import *

from XPLMDefs import *
from XPLMDataAccess import *

class PythonInterface:
   def __init__(self):
      self.errors = 0
   
   def check(self):
      if self.errors == 0:
         print('DataAccess module check OK.')
      else:
         print('DataAccess module check: {0} errors found.'.format(self.errors))

   def checkVal(self, prompt, got, expected):
      if got != None:
         if isinstance(expected, float):
            if abs(got - expected) > 2e-6:
               print(' ** ERROR ** {0}: got {1}, expected {2}'.format(prompt, got, expected))
               self.errors += 1
         elif isinstance(expected, list):
            if len(got) != len(expected):
               print(' ** ERROR ** {0}: got {1}, expected {2}(bad length)'.format(prompt, got, expected))
               self.errors += 1
               return
            for v1, v2 in zip(got, expected):
               if abs(v1 - v2) > 2e-6:
                  print(' ** ERROR ** {0}: got {1}, expected {2} (|{3} - {4}| = {5})'.format(prompt, got, expected,
                        v1, v2, abs(v1-v2)))
                  self.errors += 1
                  return
                  
         else:
            if got != expected:
               print(' ** ERROR ** {0}: got {1}, expected {2}'.format(prompt, got, expected))
               self.errors += 1
         return
      valID = prompt      
      if isinstance(expected, int):
         if not XPLMCHKHelperCheckInt(valID, expected):
            print(' ** ERROR ** {0} != {1}'.format(valID, expected))
            self.errors += 1
      elif isinstance(expected, float):
         if not XPLMCHKHelperCheckDouble(valID, expected):
            print(' ** ERROR ** {0} != {1}'.format(valID, expected))
            self.errors += 1
      elif isinstance(expected, str):
         if not XPLMCHKHelperCheckStr(valID, expected):
            print(' ** ERROR ** {0} != {1}'.format(valID, expected))
            self.errors += 1
      else:
         print(' ** ERROR ** Unsupported type passed to checkVal')
         self.errors += 1

   def XPluginStart(self):
      self.Name = ""
      self.Sig = ""
      self.Desc = ""

      self.getInt = self.getDatai; self.getIntRefcon = 44444444; self.intVal = 234;
      self.setInt = self.setDatai; self.setIntRefcon = 55555555;
      self.getFloat = self.getDataf; self.getFloatRefcon = 66666666; self.floatVal = 4.567;
      self.setFloat = self.setDataf; self.setFloatRefcon = 77777777;
      self.getDouble = self.getDatad; self.getDoubleRefcon = 88888888; self.doubleVal = 7.890;
      self.setDouble = self.setDatad; self.setDoubleRefcon = 99999999;
      self.getIntVec = self.getDatavi; self.getIntVecRefcon = 10101010; self.intVecVal = [1, 2, 3];
      self.setIntVec = self.setDatavi; self.setIntVecRefcon = 20202020;
      self.getFloatVec = self.getDatavf;self.getFloatVecRefcon = 30303030; self.floatVecVal = [1.1, 2.2, 3.3, 4.4];
      self.setFloatVec = self.setDatavf;self.setFloatVecRefcon = 40404040;
      self.getByteVec = self.getDatab;self.getByteVecRefcon = 50505050; self.byteVecVal = [65, 66, 67, 68, 69];
      self.setByteVec = self.setDatab;self.setByteVecRefcon = 60606060;


      self.intDataref = XPLMRegisterDataAccessor(self, 'test/int', xplmType_Int, True,
                                                 self.getInt, self.setInt,
                                                 None, None, #float
                                                 None, None, #double
                                                 None, None, #int vec
                                                 None, None, #float vec
                                                 None, None, #byte vec
                                                 self.getIntRefcon, self.setIntRefcon
                                                )

      self.floatDataref = XPLMRegisterDataAccessor(self, 'test/float', xplmType_Float, True,
                                                 None, None, #int
                                                 self.getFloat, self.setFloat, #float
                                                 None, None, #double
                                                 None, None, #int vec
                                                 None, None, #float vec
                                                 None, None, #byte vec
                                                 self.getFloatRefcon, self.setFloatRefcon
                                                )
      self.doubleDataref = XPLMRegisterDataAccessor(self, 'test/double', xplmType_Double, True,
                                                 None, None, #int
                                                 None, None, #float
                                                 self.getDouble, self.setDouble, #double
                                                 None, None, #int vec
                                                 None, None, #float vec
                                                 None, None, #byte vec
                                                 self.getDoubleRefcon, self.setDoubleRefcon
                                                )
      self.intVecDataref = XPLMRegisterDataAccessor(self, 'test/intVec', xplmType_IntArray, True,
                                                 None, None, #int
                                                 None, None, #float
                                                 None, None, #double
                                                 self.getIntVec, self.setIntVec, #int vec
                                                 None, None, #float vec
                                                 None, None, #byte vec
                                                 self.getIntVecRefcon, self.setIntVecRefcon
                                                )
      self.floatVecDataref = XPLMRegisterDataAccessor(self, 'test/floatVec', xplmType_FloatArray, True,
                                                 None, None, #int
                                                 None, None, #float
                                                 None, None, #double
                                                 None, None, #int vec
                                                 self.getFloatVec, self.setFloatVec, #float vec
                                                 None, None, #byte vec
                                                 self.getFloatVecRefcon, self.setFloatVecRefcon
                                                )
      self.byteVecDataref = XPLMRegisterDataAccessor(self, 'test/byteVec', xplmType_Data, True,
                                                 None, None, #int
                                                 None, None, #float
                                                 None, None, #double
                                                 None, None, #int vec
                                                 None, None, #float vec
                                                 self.getByteVec, self.setByteVec, #byte vec
                                                 self.getByteVecRefcon, self.setByteVecRefcon
                                                )
      self.shdiName = 'test/intShd'
      self.shdiType = xplmType_Int
      self.shdCbk = self.sharedDataCallback
      self.shdiCbkCalled = [0]
      self.shdiRefcon = self.shdiCbkCalled
      shdRes = XPLMShareData(self, self.shdiName, self.shdiType, self.shdCbk, self.shdiRefcon)
      self.checkVal('XPLMShareData failed', shdRes, 1)
      
      self.shdfName = 'test/floatShd'
      self.shdfType = xplmType_Float
      self.shdfCbkCalled = [0]
      self.shdfRefcon = self.shdfCbkCalled
      shdRes = XPLMShareData(self, self.shdfName, self.shdfType, self.shdCbk, self.shdfRefcon)
      self.checkVal('XPLMShareData failed', shdRes, 1)

      return self.Name, self.Sig, self.Desc
   
   def XPluginStop(self):
      XPLMUnregisterDataAccessor(self, self.intDataref)
      XPLMUnregisterDataAccessor(self, self.floatDataref)
      XPLMUnregisterDataAccessor(self, self.doubleDataref)
      XPLMUnregisterDataAccessor(self, self.intVecDataref)
      XPLMUnregisterDataAccessor(self, self.floatVecDataref)
      XPLMUnregisterDataAccessor(self, self.byteVecDataref)
      XPLMUnshareData(self, self.shdiName, self.shdiType, self.shdCbk, self.shdiRefcon)
      XPLMUnshareData(self, self.shdfName, self.shdfType, self.shdCbk, self.shdfRefcon)

      self.check()
   
   def XPluginEnable(self):
      return 1
   
   def XPluginDisable(self):
      return

   def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
      self.checkVal('XPluginReceiveMessage: Unexpected inFromWho', inFromWho, 5)
      self.checkVal('XPluginReceiveMessage: Unexpected inMessage', inMessage, 103)
      self.checkVal('XPluginReceiveMessage: Unexpected inParam', inParam, 333)

      #Test shared data
      shdi = XPLMFindDataRef(self.shdiName)
      self.checkVal('Int shared data callback called prematurely.', self.shdiCbkCalled, [0])
      shdf = XPLMFindDataRef(self.shdfName)
      self.checkVal('Float shared data callback called prematurely.', self.shdfCbkCalled, [0])
      XPLMSetDatai(shdi, 16)
      self.checkVal('Int shared data callback call number wrong.', self.shdiCbkCalled, [1])
      self.checkVal('Int shared data value doesn\'t match.', XPLMGetDatai(shdi), 16)
      XPLMSetDataf(shdf, 444.444)
      self.checkVal('Float shared data callback call number wrong.', self.shdfCbkCalled, [1])
      self.checkVal('Float shared data value doesn\'t match.', XPLMGetDataf(shdf), 444.444)

      i = XPLMFindDataRef('test/int')
      f = XPLMFindDataRef('test/float')
      d = XPLMFindDataRef('test/double')
      iv = XPLMFindDataRef('test/intVec')
      fv = XPLMFindDataRef('test/floatVec')
      bv = XPLMFindDataRef('test/byteVec')

      self.checkVal('Int dataref has should be writable.', XPLMCanWriteDataRef(i), 1)
      self.checkVal('Int dataref has unexpected type.', XPLMGetDataRefTypes(i), xplmType_Int)
      newVal = XPLMGetDatai(i)
      self.checkVal('Int dataref value doesn\'t match.', newVal, self.intVal)
      newVal += 1
      XPLMSetDatai(i, newVal)
      self.checkVal('Int dataref new value doesn\'t match.', newVal, self.intVal)

      self.checkVal('Float dataref has should be writable.', XPLMCanWriteDataRef(f), 1)
      self.checkVal('Float dataref has unexpected type.', XPLMGetDataRefTypes(f), xplmType_Float)
      newVal = XPLMGetDataf(f)
      self.checkVal('Float dataref value doesn\'t match.', newVal, self.floatVal)
      newVal += .1
      XPLMSetDataf(f, newVal)
      self.checkVal('Float dataref new value doesn\'t match.', newVal, self.floatVal)

      self.checkVal('Double dataref has should be writable.', XPLMCanWriteDataRef(d), 1)
      self.checkVal('Double dataref has unexpected type.', XPLMGetDataRefTypes(d), xplmType_Double)
      newVal = XPLMGetDatad(d)
      self.checkVal('Double dataref value doesn\'t match.', newVal, self.doubleVal)
      newVal += 0.2
      XPLMSetDatad(d, newVal)
      self.checkVal('Double dataref new value doesn\'t match.', newVal, self.doubleVal)

      #Int vectors
      self.checkVal('Int array dataref has should be writable.', XPLMCanWriteDataRef(iv), 1)
      self.checkVal('Int array dataref has unexpected type.', XPLMGetDataRefTypes(iv), xplmType_IntArray)
      #Check array length retrieval
      newVal = XPLMGetDatavi(iv, None, 0, 0)
      self.checkVal('Int array dataref length doesn\'t match', newVal, len(self.intVecVal))
      #Get all data
      newVec = []
      newVal = XPLMGetDatavi(iv, newVec, 0, newVal)
      self.checkVal('Int array dataref get data result length doesn\'t match', newVal, len(self.intVecVal))
      self.checkVal('Int array dataref get data result doesn\'t match', newVec, self.intVecVal)
      #Try getting too much data
      newVal = XPLMGetDatavi(iv, newVec, 0, newVal+1)
      self.checkVal('Int array dataref length doesn\'t match (inMax too big)', newVal, len(self.intVecVal))
      self.checkVal('Int array dataref data doesn\'t match (inMax too big)', newVec, self.intVecVal)
      #Try getting data beyond the array
      newVec = [0]
      newVal = XPLMGetDatavi(iv, newVec, len(self.intVecVal), 1)
      self.checkVal('Int array dataref length doesn\'t match (inOffset too big)', newVal, 0)
      self.checkVal('Int array dataref data doesn\'t match (inOffset too big)', newVec, [])
      #Set array subset
      newVec = [15]
      XPLMSetDatavi(iv, newVec, 2, len(newVec))
      self.checkVal('Int array dataref elem doesn\'t match', newVec, self.intVecVal[2:3])
      #Set full array
      newVec = [33, 34, 35]
      XPLMSetDatavi(iv, newVec, 0, len(newVec))
      self.checkVal('Int array dataref data doesn\'t match', newVec, self.intVecVal)
      #Try setting too much data
      newVec = [44, 45, 46, 47]
      XPLMSetDatavi(iv, newVec, 0, len(newVec))
      self.checkVal('Int array dataref data doesn\'t match (too much data)', newVec[:3], self.intVecVal)
      #Try setting beyond the border with offset 
      newVec = [55, 56, 57]
      XPLMSetDatavi(iv, newVec, 1, len(newVec))
      self.checkVal('Int array dataref data doesn\'t match (too much data + offset)', newVec[:2], self.intVecVal[1:])


      #Float vectors
      self.checkVal('Float array dataref has should be writable.', XPLMCanWriteDataRef(fv), 1)
      self.checkVal('Float array dataref has unexpected type.', XPLMGetDataRefTypes(fv), xplmType_FloatArray)
      #Check array length retrieval
      newVal = XPLMGetDatavf(fv, None, 0, 0)
      self.checkVal('Float array dataref length doesn\'t match', newVal, len(self.floatVecVal))
      #Get all data
      newVec = []
      newVal = XPLMGetDatavf(fv, newVec, 0, newVal)
      self.checkVal('Float array dataref get data result length doesn\'t match', newVal, len(self.floatVecVal))
      self.checkVal('Float array dataref get data result doesn\'t match', newVec, self.floatVecVal)
      #Try getting too much data
      newVal = XPLMGetDatavf(fv, newVec, 0, newVal+1)
      self.checkVal('Float array dataref length doesn\'t match (inMax too big)', newVal, len(self.floatVecVal))
      self.checkVal('Float array dataref data doesn\'t match (inMax too big)', newVec, self.floatVecVal)
      #Try getting data beyond the array
      newVec = [0]
      newVal = XPLMGetDatavf(fv, newVec, len(self.floatVecVal), 1)
      self.checkVal('Float array dataref length doesn\'t match (inOffset too big)', newVal, 0)
      self.checkVal('Float array dataref data doesn\'t match (inOffset too big)', newVec, [])
      #Set array subset
      newVec = [5.8]
      pos = len(self.floatVecVal) - 2
      XPLMSetDatavf(fv, newVec, pos, len(newVec))
      self.checkVal('Float array dataref elem doesn\'t match', newVec, self.floatVecVal[pos:pos+1])
      #Set full array
      newVec = [11.2, 12.3, 13.4, 14.5]
      XPLMSetDatavf(fv, newVec, 0, len(newVec))
      self.checkVal('Float array dataref data doesn\'t match', newVec, self.floatVecVal)
      #Try setting too much data
      newVec = [21.3, 22.4, 23.5, 24.6]
      XPLMSetDatavf(fv, newVec, 0, len(newVec))
      self.checkVal('Float array dataref data doesn\'t match (too much data)', newVec[:4], self.floatVecVal)
      #Try setting beyond the border with offset 
      newVec = [31.4, 32.5, 33.6, 34.7]
      XPLMSetDatavf(fv, newVec, 1, len(newVec))
      self.checkVal('Float array dataref data doesn\'t match (too much data + offset)', newVec[:3], self.floatVecVal[1:])


      #Byte vectors
      self.checkVal('Byte array dataref has should be writable.', XPLMCanWriteDataRef(bv), 1)
      self.checkVal('Byte array dataref has unexpected type.', XPLMGetDataRefTypes(bv), xplmType_Data)
      #Check array length retrieval
      newVal = XPLMGetDatab(bv, None, 0, 0)
      self.checkVal('Byte array dataref length doesn\'t match', newVal, len(self.byteVecVal))
      #Get all data
      newVec = []
      newVal = XPLMGetDatab(bv, newVec, 0, newVal)
      self.checkVal('Byte array dataref get data result length doesn\'t match', newVal, len(self.byteVecVal))
      self.checkVal('Byte array dataref get data result doesn\'t match', newVec, self.byteVecVal)
      #Try getting too much data
      newVal = XPLMGetDatab(bv, newVec, 0, newVal+1)
      self.checkVal('Byte array dataref length doesn\'t match (inMax too big)', newVal, len(self.byteVecVal))
      self.checkVal('Byte array dataref data doesn\'t match (inMax too big)', newVec, self.byteVecVal)
      #Try getting data beyond the array
      newVec = [0]
      newVal = XPLMGetDatab(bv, newVec, len(self.byteVecVal), 1)
      self.checkVal('Byte array dataref length doesn\'t match (inOffset too big)', newVal, 0)
      self.checkVal('Byte array dataref data doesn\'t match (inOffset too big)', newVec, [])
      #Set array subset
      newVec = [78]
      pos = len(self.byteVecVal) - 2
      XPLMSetDatab(bv, newVec, pos, len(newVec))
      self.checkVal('Byte array dataref elem doesn\'t match', newVec, self.byteVecVal[pos:pos+1])
      #Set full array
      newVec = [88, 89, 90, 91, 92]
      XPLMSetDatab(bv, newVec, 0, len(newVec))
      self.checkVal('Byte array dataref data doesn\'t match', newVec, self.byteVecVal)
      #Try setting too much data
      newVec = [93, 94, 95, 96, 97, 98]
      XPLMSetDatab(bv, newVec, 0, len(newVec))
      self.checkVal('Byte array dataref data doesn\'t match (too much data)', newVec[:5], self.byteVecVal)
      #Try setting beyond the border with offset 
      newVec = [99, 100, 101, 102, 103]
      XPLMSetDatab(bv, newVec, 1, len(newVec))
      self.checkVal('Byte array dataref data doesn\'t match (too much data + offset)', newVec[:4], self.byteVecVal[1:])

   def sharedDataCallback(self, inRefcon):
      inRefcon[0] += 1

   def getDatai(self, inRefcon):
      self.checkVal('getDatai refcon doesn\'t match.', inRefcon, self.getIntRefcon)
      return self.intVal

   def setDatai(self, inRefcon, inValue):
      self.checkVal('setDatai refcon doesn\'t match.', inRefcon, self.setIntRefcon)
      self.intVal = inValue
      return

   def getDataf(self, inRefcon):
      self.checkVal('getDataf refcon doesn\'t match.', inRefcon, self.getFloatRefcon)
      return self.floatVal

   def setDataf(self, inRefcon, inValue):
      self.checkVal('setDataf refcon doesn\'t match.', inRefcon, self.setFloatRefcon)
      self.floatVal = inValue
      return

   def getDatad(self, inRefcon):
      self.checkVal('getDatad refcon doesn\'t match.', inRefcon, self.getDoubleRefcon)
      return self.doubleVal

   def setDatad(self, inRefcon, inValue):
      self.checkVal('setDatad refcon doesn\'t match.', inRefcon, self.setDoubleRefcon)
      self.doubleVal = inValue
      return

   def getDatavi(self, inRefcon, outValues, inOffset, inMax):
      if outValues is None:
         return len(self.intVecVal)
      self.checkVal('getDatavi refcon doesn\'t match.', inRefcon, self.getIntVecRefcon)
      return self.getArrayData(self.intVecVal, outValues, inOffset, inMax)

   def setDatavi(self, inRefcon, inValues, inOffset, inCount):
      self.checkVal('setDatavi refcon doesn\'t match.', inRefcon, self.setIntVecRefcon)
      self.setArrayData(self.intVecVal, inValues, inOffset, inCount)

   def getDatavf(self, inRefcon, outValues, inOffset, inMax):
      if outValues is None:
         return len(self.floatVecVal)
      self.checkVal('getDatavf refcon doesn\'t match.', inRefcon, self.getFloatVecRefcon)
      return self.getArrayData(self.floatVecVal, outValues, inOffset, inMax)

   def setDatavf(self, inRefcon, inValues, inOffset, inCount):
      self.checkVal('setDatavf refcon doesn\'t match.', inRefcon, self.setFloatVecRefcon)
      self.setArrayData(self.floatVecVal, inValues, inOffset, inCount)

   def getDatab(self, inRefcon, outValue, inOffset, inMax):
      if outValue is None:
         return len(self.byteVecVal)
      self.checkVal('getDatab refcon doesn\'t match.', inRefcon, self.getByteVecRefcon)
      return self.getArrayData(self.byteVecVal, outValue, inOffset, inMax)

   def setDatab(self, inRefcon, inValue, inOffset, inCount):
      self.checkVal('setDatab refcon doesn\'t match.', inRefcon, self.setByteVecRefcon)
      self.setArrayData(self.byteVecVal, inValue, inOffset, inCount)

   def getArrayData(self, values, outValues, inOffset, inMax):
      l = len(values)
      if inOffset >= l:
         return 0
      if l >= inOffset + inMax:
         outValues[:] = values[inOffset:inOffset + inMax + 1]
         return inMax
      else:
         outValues[:] = values[inOffset:]
         return l - inOffset

   def setArrayData(self, values, inValues, inOffset, inCount):
      l = len(values)
      if inOffset >= l:
         return
      
      if l >= inOffset + inCount:
         values[inOffset: inOffset + inCount] = inValues
      else:
         values[inOffset:] = inValues[:l - inOffset]

#
