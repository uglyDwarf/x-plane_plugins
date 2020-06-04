from check_helper import checkBase
from XPLMPlugin import XPLM_MSG_WILL_WRITE_PREFS
from XPLMDataAccess import xplmType_Unknown, xplmType_Int, xplmType_Float, xplmType_Double, xplmType_FloatArray, xplmType_IntArray, xplmType_Data

from XPLMDataAccess import XPLMFindDataRef, XPLMCanWriteDataRef, XPLMIsDataRefGood, XPLMGetDataRefTypes
from XPLMDataAccess import XPLMGetDatai, XPLMSetDatai, XPLMGetDataf, XPLMSetDataf, XPLMGetDatad, XPLMSetDatad
from XPLMDataAccess import XPLMGetDatavi, XPLMSetDatavi, XPLMGetDatavf, XPLMSetDatavf, XPLMGetDatab, XPLMSetDatab
from XPLMDataAccess import XPLMRegisterDataAccessor, XPLMUnregisterDataAccessor, XPLMShareData, XPLMUnshareData


class PythonInterface(checkBase):

    def __init__(self):
        checkBase.__init__(self, 'DataAccess')
        checkBase.addRef()
        self.types = [
            {'dataTypeID': xplmType_Int,
             'refCon': 'int type',
             'array_type': False,
             'x_get': XPLMGetDatai,
             'x_set': XPLMSetDatai,
             'value': 1,
             # 'get_f': self.getDatai,
             # 'set_f': self.setDatai,
             'existing_dataref': 'sim/aircraft/overflow/acf_o2_bottle_cap_liters',
            },
            {'dataTypeID': xplmType_Float,
             'refCon': 'float type',
             'array_type': False,
             'x_get': XPLMGetDataf,
             'x_set': XPLMSetDataf,
             'value': 1.0,
             # 'get_f': self.getDataf,
             # 'set_f': self.setDataf,
             'existing_dataref': 'sim/aircraft/view/acf_Vso'

            },
            {'dataTypeID': xplmType_Double,
             'refCon': 'double type',
             'array_type': False,
             'x_get': XPLMGetDatad,
             'x_set': XPLMSetDatad,
             'value': 1.0,
             # 'get_f': self.getDatad,
             # 'set_f': self.setDatad,
             'existing_dataref': 'sim/flightmodel/position/local_x'
            },
            {'dataTypeID': xplmType_IntArray,
             'refCon': 'int array type',
             'array_type': True,
             'x_get': XPLMGetDatavi,
             'x_set': XPLMSetDatavi,
             'value': [1, 2, 3, 4],
             # 'get_f': self.getDatavi,
             # 'set_f': self.setDatavi,
             'existing_dataref': 'sim/cockpit2/electrical/battery_on',  # 1 or 0 [8]
            },
            {'dataTypeID': xplmType_FloatArray,
             'refCon': 'float array type',
             'array_type': True,
             'x_get': XPLMGetDatavf,
             'x_set': XPLMSetDatavf,
             'value': [1.0, 2.0, 3.0, 4.0],
             # 'get_f': self.getDatavf,
             # 'set_f': self.setDatavf,
             'existing_dataref': 'sim/graphics/colors/plane_path1_3d_rgb'  # rgb colors[3]
            },
            {'dataTypeID': xplmType_Data,
             'refCon': 'Data type',
             'array_type': True,
             'x_get': XPLMGetDatab,
             'x_set': XPLMSetDatab,
             'value': [1.0, 2.0, 3.0, 4.0],
             # 'get_f': self.getDatab,
             # 'set_f': self.setDatab,
             'existing_dataref': 'sim/aircraft/view/acf_tailnum'
            },
        ]

    def XPluginStart(self):
        self.Sig = "xppython3.DataAccess"
        self.Name = "{} regression test".format(self.Sig)
        self.Desc = "Regression test for {} module".format(self.Sig)

        self.msgCounter = 0
        return self.Name, self.Sig, self.Desc

    def XPluginStop(self):
        self.check()
        checkBase.remRef()

    def XPluginEnable(self):
        return 1

    def XPluginDisable(self):
        return

    def XPluginReceiveMessage(self, inFromWho, inMessage, inParam):
        # do test on receipt of WILL_WRITE_PREFS -- likely at the end
        if inMessage != XPLM_MSG_WILL_WRITE_PREFS:
            return

        #####
        # Check each type using existing datatypes
        #  a) get and existing dataref of that type,
        #  b) increment it by one
        #  c) set it
        #  d) get it again and verify the update
        for t in self.types:
            # a) Find a data ref of that type,
            self.log("Testing {}".format(t['refCon']))
            t['dataRefName'] = 'test/{}'.format(t['refCon'])
            t['dataRefID'] = XPLMFindDataRef(t['existing_dataref'])
            t['localDataRefID'] = XPLMRegisterDataAccessor(t['dataRefName'],  # needs to be 'global'?
                                                           t['dataTypeID'], True,
                                                           self.getDatai, self.setDatai,
                                                           self.getDataf, self.setDataf,
                                                           self.getDatad, self.setDatad,
                                                           self.getDatavi, self.setDatavi,
                                                           self.getDatavf, self.setDatavf,
                                                           self.getDatab, self.setDatab,
                                                           'get' + t['refCon'], 'set' + t['refCon'])
            for d in (t['dataRefID'], t['localDataRefID']):
                if d == 0:
                    self.error("Failed to get data ref for {}".format(t['existing_dataref']))
                    return
                datarefname = t['existing_dataref'] if d == t['dataRefID'] else t['dataRefName']
                self.checkVal('Data ref is good failed', XPLMIsDataRefGood(d), 1)
                self.checkVal('Data ref is writable failed', XPLMCanWriteDataRef(d), 1)
                self.checkVal('Data ref type mismatch', XPLMGetDataRefTypes(d) & t['dataTypeID'], t['dataTypeID'])
                # b) get value
                if t['array_type']:
                    offset = 0
                    max_values = 5
                    currentValue = []
                    num_read = t['x_get'](d, currentValue, offset, max_values)
                    if num_read <= 0:
                        self.error("Failed to read array {}".format(datarefname))
                        return
                else:
                    currentValue = t['x_get'](d)
                self.checkVal('{} get value'.format(datarefname), 1, 1)
                self.log("{} is currently {}".format(datarefname, currentValue))
                # c) set value
                if t['array_type']:
                    new_value = currentValue[:]
                    new_value[0] = new_value[0] + 1
                    t['x_set'](d, new_value, 0, 1)
                    updated_values = []
                    # d) get value again, to make sure it works
                    t['x_get'](d, updated_values, offset, max_values)
                    self.checkVal('{} set value'.format(datarefname), updated_values, new_value)
                    self.log("{} updated to   {}".format(datarefname, updated_values))
                else:
                    new_value = currentValue + 1
                    t['x_set'](d, new_value)
                    # d) get value again, to make sure it works
                    self.checkVal('{} set value'.format(datarefname), t['x_get'](d), new_value)
                    self.log("{} updated to   {}".format(datarefname, t['x_get'](d)))

        self.log("SHARING...")
        for t in self.types:
            self.checkVal('XPLMShareData fail', XPLMShareData(t['dataRefName'], t['dataTypeID'], self.callback, t['refCon']), 1)
            t['sharedDataRefID'] = XPLMFindDataRef(t['dataRefName'])
            self.checkVal("SharedData Ref is zero", t['sharedDataRefID'] in (0, None), False)
            XPLMUnshareData(t['dataRefName'], t['dataTypeID'], self.callback, t['refCon'])
            XPLMUnregisterDataAccessor(t['localDataRefID'])
        return

        # self.shdiName = 'test/intShd'
        # self.shdiType = xplmType_Int
        # self.shdCbk = self.sharedDataCallback
        # self.shdiCbkCalled = [self.msgCounter]
        # self.shdiRefcon = self.shdiCbkCalled
        # shdRes = XPLMShareData(self.shdiName, self.shdiType, self.shdCbk, self.shdiRefcon)
        # self.checkVal('XPLMShareData failed', shdRes, 1)

        # self.shdfName = 'test/floatShd'
        # self.shdfType = xplmType_Float
        # self.shdfCbkCalled = [self.msgCounter + 10]
        # self.shdfRefcon = self.shdfCbkCalled
        # shdRes = XPLMShareData(self.shdfName, self.shdfType, self.shdCbk, self.shdfRefcon)
        # self.checkVal('XPLMShareData failed', shdRes, 1)

        # # Test shared data
        # shdi = XPLMFindDataRef(self.shdiName)
        # self.checkVal('Int shared data callback called prematurely.', self.shdiCbkCalled, [self.msgCounter])
        # shdf = XPLMFindDataRef(self.shdfName)
        # self.checkVal('Float shared data callback called prematurely.', self.shdfCbkCalled, [self.msgCounter + 10])
        # self.msgCounter += 1
        # XPLMSetDatai(shdi, 16)
        # self.checkVal('Int shared data callback call number wrong.', self.shdiCbkCalled, [self.msgCounter])
        # self.checkVal('Int shared data value doesn\'t match.', XPLMGetDatai(shdi), 16)
        # XPLMSetDataf(shdf, 444.444)
        # self.checkVal('Float shared data callback call number wrong.', self.shdfCbkCalled, [self.msgCounter + 10])
        # self.checkVal('Float shared data value doesn\'t match.', XPLMGetDataf(shdf), 444.444)

        # i = XPLMFindDataRef('test/int')
        # f = XPLMFindDataRef('test/float')
        # d = XPLMFindDataRef('test/double')
        # iv = XPLMFindDataRef('test/intVec')
        # fv = XPLMFindDataRef('test/floatVec')
        # bv = XPLMFindDataRef('test/byteVec')

        # self.checkVal('Int dataref has should be good.', XPLMIsDataRefGood(i), 1)
        # self.checkVal('Int dataref has should be writable.', XPLMCanWriteDataRef(i), 1)
        # self.checkVal('Int dataref has unexpected type.', XPLMGetDataRefTypes(i), xplmType_Int)
        # newVal = XPLMGetDatai(i)
        # self.checkVal('Int dataref value doesn\'t match.', newVal, self.intVal)
        # newVal += 1
        # XPLMSetDatai(i, newVal)
        # self.checkVal('Int dataref new value doesn\'t match.', newVal, self.intVal)

        # self.checkVal('Float dataref has should be writable.', XPLMCanWriteDataRef(f), 1)
        # self.checkVal('Float dataref has unexpected type.', XPLMGetDataRefTypes(f), xplmType_Float)
        # newVal = XPLMGetDataf(f)
        # self.checkVal('Float dataref value doesn\'t match.', newVal, self.floatVal)
        # newVal += .1
        # XPLMSetDataf(f, newVal)
        # self.checkVal('Float dataref new value doesn\'t match.', newVal, self.floatVal)

        # self.checkVal('Double dataref has should be writable.', XPLMCanWriteDataRef(d), 1)
        # self.checkVal('Double dataref has unexpected type.', XPLMGetDataRefTypes(d), xplmType_Double)
        # newVal = XPLMGetDatad(d)
        # self.checkVal('Double dataref value doesn\'t match.', newVal, self.doubleVal)
        # newVal += 0.2
        # XPLMSetDatad(d, newVal)
        # self.checkVal('Double dataref new value doesn\'t match.', newVal, self.doubleVal)

        # # Int vectors
        # self.checkVal('Int array dataref has should be writable.', XPLMCanWriteDataRef(iv), 1)
        # self.checkVal('Int array dataref has unexpected type.', XPLMGetDataRefTypes(iv), xplmType_IntArray)

        # # Check array length retrieval
        # newVal = XPLMGetDatavi(iv, None, 0, 0)
        # self.checkVal('Int array dataref length doesn\'t match', newVal, len(self.intVecVal))
        # # Get all data
        # newVec = []
        # newVal = XPLMGetDatavi(iv, newVec, 0, newVal)
        # self.checkVal('Int array dataref get data result length doesn\'t match', newVal, len(self.intVecVal))
        # self.checkVal('Int array dataref get data result doesn\'t match', newVec, self.intVecVal)
        # # Try getting too much data
        # newVal = XPLMGetDatavi(iv, newVec, 0, newVal + 1)
        # self.checkVal('Int array dataref length doesn\'t match (inMax too big)', newVal, len(self.intVecVal))
        # self.checkVal('Int array dataref data doesn\'t match (inMax too big)', newVec, self.intVecVal)
        # # Try getting data beyond the array
        # newVec = [0]
        # newVal = XPLMGetDatavi(iv, newVec, len(self.intVecVal), 1)
        # self.checkVal('Int array dataref length doesn\'t match (inOffset too big)', newVal, 0)
        # self.checkVal('Int array dataref data doesn\'t match (inOffset too big)', newVec, [])
        # # Set array subset
        # newVec = [15]
        # XPLMSetDatavi(iv, newVec, 2, len(newVec))
        # self.checkVal('Int array dataref elem doesn\'t match', newVec, self.intVecVal[2:3])
        # # Set full array
        # newVec = [33, 34, 35]
        # XPLMSetDatavi(iv, newVec, 0, len(newVec))
        # self.checkVal('Int array dataref data doesn\'t match', newVec, self.intVecVal)
        # # Try setting too much data
        # newVec = [44, 45, 46, 47]
        # XPLMSetDatavi(iv, newVec, 0, len(newVec))
        # self.checkVal('Int array dataref data doesn\'t match (too much data)', newVec[:3], self.intVecVal)
        # # Try setting beyond the border with offset
        # newVec = [55, 56, 57]
        # XPLMSetDatavi(iv, newVec, 1, len(newVec))
        # self.checkVal('Int array dataref data doesn\'t match (too much data + offset)', newVec[:2], self.intVecVal[1:])

        # # Float vectors
        # self.checkVal('Float array dataref has should be writable.', XPLMCanWriteDataRef(fv), 1)
        # self.checkVal('Float array dataref has unexpected type.', XPLMGetDataRefTypes(fv), xplmType_FloatArray)
        # # Check array length retrieval
        # newVal = XPLMGetDatavf(fv, None, 0, 0)
        # self.checkVal('Float array dataref length doesn\'t match', newVal, len(self.floatVecVal))
        # # Get all data
        # newVec = []
        # newVal = XPLMGetDatavf(fv, newVec, 0, newVal)
        # self.checkVal('Float array dataref get data result length doesn\'t match', newVal, len(self.floatVecVal))
        # self.checkVal('Float array dataref get data result doesn\'t match', newVec, self.floatVecVal)
        # # Try getting too much data
        # newVal = XPLMGetDatavf(fv, newVec, 0, newVal+1)
        # self.checkVal('Float array dataref length doesn\'t match (inMax too big)', newVal, len(self.floatVecVal))
        # self.checkVal('Float array dataref data doesn\'t match (inMax too big)', newVec, self.floatVecVal)
        # # Try getting data beyond the array
        # newVec = [0]
        # newVal = XPLMGetDatavf(fv, newVec, len(self.floatVecVal), 1)
        # self.checkVal('Float array dataref length doesn\'t match (inOffset too big)', newVal, 0)
        # self.checkVal('Float array dataref data doesn\'t match (inOffset too big)', newVec, [])
        # # Set array subset
        # newVec = [5.8]
        # pos = len(self.floatVecVal) - 2
        # XPLMSetDatavf(fv, newVec, pos, len(newVec))
        # self.checkVal('Float array dataref elem doesn\'t match', newVec, self.floatVecVal[pos:pos + 1])
        # # Set full array
        # newVec = [11.2, 12.3, 13.4, 14.5]
        # XPLMSetDatavf(fv, newVec, 0, len(newVec))
        # self.checkVal('Float array dataref data doesn\'t match', newVec, self.floatVecVal)
        # # Try setting too much data
        # newVec = [21.3, 22.4, 23.5, 24.6]
        # XPLMSetDatavf(fv, newVec, 0, len(newVec))
        # self.checkVal('Float array dataref data doesn\'t match (too much data)', newVec[:4], self.floatVecVal)
        # # Try setting beyond the border with offset
        # newVec = [31.4, 32.5, 33.6, 34.7]
        # XPLMSetDatavf(fv, newVec, 1, len(newVec))
        # self.checkVal('Float array dataref data doesn\'t match (too much data + offset)', newVec[:3], self.floatVecVal[1:])

        # # Byte vectors
        # self.checkVal('Byte array dataref has should be writable.', XPLMCanWriteDataRef(bv), 1)
        # self.checkVal('Byte array dataref has unexpected type.', XPLMGetDataRefTypes(bv), xplmType_Data)
        # # Check array length retrieval
        # newVal = XPLMGetDatab(bv, None, 0, 0)
        # self.checkVal('Byte array dataref length doesn\'t match', newVal, len(self.byteVecVal))
        # # Get all data
        # newVec = []
        # newVal = XPLMGetDatab(bv, newVec, 0, newVal)
        # self.checkVal('Byte array dataref get data result length doesn\'t match', newVal, len(self.byteVecVal))
        # self.checkVal('Byte array dataref get data result doesn\'t match', newVec, self.byteVecVal)
        # # Try getting too much data
        # newVal = XPLMGetDatab(bv, newVec, 0, newVal + 1)
        # self.checkVal('Byte array dataref length doesn\'t match (inMax too big)', newVal, len(self.byteVecVal))
        # self.checkVal('Byte array dataref data doesn\'t match (inMax too big)', newVec, self.byteVecVal)
        # # Try getting data beyond the array
        # newVec = [0]
        # newVal = XPLMGetDatab(bv, newVec, len(self.byteVecVal), 1)
        # self.checkVal('Byte array dataref length doesn\'t match (inOffset too big)', newVal, 0)
        # self.checkVal('Byte array dataref data doesn\'t match (inOffset too big)', newVec, [])
        # # Set array subset
        # newVec = [78]
        # pos = len(self.byteVecVal) - 2
        # XPLMSetDatab(bv, newVec, pos, len(newVec))
        # self.checkVal('Byte array dataref elem doesn\'t match', newVec, self.byteVecVal[pos:pos+1])
        # # Set full array
        # newVec = [88, 89, 90, 91, 92]
        # XPLMSetDatab(bv, newVec, 0, len(newVec))
        # self.checkVal('Byte array dataref data doesn\'t match', newVec, self.byteVecVal)
        # # Try setting too much data
        # newVec = [93, 94, 95, 96, 97, 98]
        # XPLMSetDatab(bv, newVec, 0, len(newVec))
        # self.checkVal('Byte array dataref data doesn\'t match (too much data)', newVec[:5], self.byteVecVal)
        # # Try setting beyond the border with offset
        # newVec = [99, 100, 101, 102, 103]
        # XPLMSetDatab(bv, newVec, 1, len(newVec))
        # self.checkVal('Byte array dataref data doesn\'t match (too much data + offset)', newVec[:4], self.byteVecVal[1:])

        # XPLMUnregisterDataAccessor(self.intDataref)
        # XPLMUnregisterDataAccessor(self.floatDataref)
        # XPLMUnregisterDataAccessor(self.doubleDataref)
        # XPLMUnregisterDataAccessor(self.intVecDataref)
        # XPLMUnregisterDataAccessor(self.floatVecDataref)
        # XPLMUnregisterDataAccessor(self.byteVecDataref)
        # XPLMUnshareData(self.shdiName, self.shdiType, self.shdCbk, self.shdiRefcon)
        # XPLMUnshareData(self.shdfName, self.shdfType, self.shdCbk, self.shdfRefcon)

    # def sharedDataCallback(self, inRefcon):
    #     inRefcon[0] += 1

    def get_type_info(self, xplm_type):
        return [x for x in self.types if xplm_type == x['dataTypeID']][0]

    def getDatai(self, inRefcon):
        t = self.get_type_info(xplmType_Int)
        self.checkVal('getDatai refcon doesn\'t match.', inRefcon, 'get' + t['refCon'])
        return t['value']

    def setDatai(self, inRefcon, inValue):
        t = self.get_type_info(xplmType_Int)
        self.checkVal('setDatai refcon doesn\'t match.', inRefcon, 'set' + t['refCon'])
        t['value'] = inValue
        return

    def getDataf(self, inRefcon):
        t = self.get_type_info(xplmType_Float)
        self.checkVal('getDataf refcon doesn\'t match.', inRefcon, 'get' + t['refCon'])
        return t['value']

    def setDataf(self, inRefcon, inValue):
        t = self.get_type_info(xplmType_Float)
        self.checkVal('setDataf refcon doesn\'t match.', inRefcon, 'set' + t['refCon'])
        t['value'] = inValue
        return

    def getDatad(self, inRefcon):
        t = self.get_type_info(xplmType_Double)
        self.checkVal('getDatad refcon doesn\'t match.', inRefcon, 'get' + t['refCon'])
        return t['value']

    def setDatad(self, inRefcon, inValue):
        t = self.get_type_info(xplmType_Double)
        self.checkVal('setDatad refcon doesn\'t match.', inRefcon, 'set' + t['refCon'])
        t['value'] = inValue
        return

    def getDatavi(self, inRefcon, outValues, inOffset, inMax):
        t = self.get_type_info(xplmType_IntArray)
        if outValues is None:
            return len(t['value'])
        self.checkVal('getDatavi refcon doesn\'t match.', inRefcon, 'get' + t['refCon'])
        return self.getArrayData(t['value'], outValues, inOffset, inMax)

    def setDatavi(self, inRefcon, inValues, inOffset, inCount):
        t = self.get_type_info(xplmType_IntArray)
        self.checkVal('setDatavi refcon doesn\'t match.', inRefcon, 'set' + t['refCon'])
        self.setArrayData(t['value'], inValues, inOffset, inCount)

    def getDatavf(self, inRefcon, outValues, inOffset, inMax):
        t = self.get_type_info(xplmType_FloatArray)
        if outValues is None:
            return len(t['value'])
        self.checkVal('getDatavf refcon doesn\'t match.', inRefcon, 'get' + t['refCon'])
        return self.getArrayData(t['value'], outValues, inOffset, inMax)

    def setDatavf(self, inRefcon, inValues, inOffset, inCount):
        t = self.get_type_info(xplmType_FloatArray)
        self.checkVal('setDatavf refcon doesn\'t match.', inRefcon, 'set' + t['refCon'])
        self.setArrayData(t['value'], inValues, inOffset, inCount)

    def getDatab(self, inRefcon, outValue, inOffset, inMax):
        t = self.get_type_info(xplmType_Data)
        if outValue is None:
            return len(t['value'])
        self.checkVal('getDatab refcon doesn\'t match.', inRefcon, 'get' + t['refCon'])
        return self.getArrayData(t['value'], outValue, inOffset, inMax)

    def setDatab(self, inRefcon, inValue, inOffset, inCount):
        t = self.get_type_info(xplmType_Data)
        self.checkVal('setDatab refcon doesn\'t match.', inRefcon, 'set' + t['refCon'])
        self.setArrayData(t['value'], inValue, inOffset, inCount)

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

    def callback(self, *args):
        self.log("callback called with {} args".format(len(*args)))
