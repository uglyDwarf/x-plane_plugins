#include <iostream>
#include <map>
#include <vector>
#include <string.h>
#include <stdint.h>
#include <cassert>
#define XPLM200
#define XPLM210
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>

#include "chk_helper.h"

typedef std::pair<XPLMDataChanged_f, void *> notificator;

class dataAccessor{
  std::string     name;
  XPLMDataTypeID  datatype;
  int             isWritable;
  XPLMGetDatai_f  readInt;
  XPLMSetDatai_f  writeInt;
  XPLMGetDataf_f  readFloat;
  XPLMSetDataf_f  writeFloat;
  XPLMGetDatad_f  readDouble;
  XPLMSetDatad_f  writeDouble;
  XPLMGetDatavi_f readIntArray;
  XPLMSetDatavi_f writeIntArray;
  XPLMGetDatavf_f readFloatArray;
  XPLMSetDatavf_f writeFloatArray;
  XPLMGetDatab_f  readData;
  XPLMSetDatab_f  writeData;
  void *          readRefcon;
  void *          writeRefcon;
  bool            good;
  std::vector<notificator> notificators;

  void notify()const;
 public:
  dataAccessor(std::string     inDataName,
               XPLMDataTypeID  inDataType,
               int             inIsWritable,
               XPLMGetDatai_f  inReadInt,
               XPLMSetDatai_f  inWriteInt,
               XPLMGetDataf_f  inReadFloat,
               XPLMSetDataf_f  inWriteFloat,
               XPLMGetDatad_f  inReadDouble,
               XPLMSetDatad_f  inWriteDouble,
               XPLMGetDatavi_f inReadIntArray,
               XPLMSetDatavi_f inWriteIntArray,
               XPLMGetDatavf_f inReadFloatArray,
               XPLMSetDatavf_f inWriteFloatArray,
               XPLMGetDatab_f  inReadData,
               XPLMSetDatab_f  inWriteData,
               void *          inReadRefcon,
               void *          inWriteRefcon
              ):name(inDataName), datatype(inDataType), isWritable(inIsWritable),
                readInt(inReadInt), writeInt(inWriteInt), readFloat(inReadFloat),
                writeFloat(inWriteFloat), readDouble(inReadDouble),
                writeDouble(inWriteDouble), readIntArray(inReadIntArray),
                writeIntArray(inWriteIntArray), readFloatArray(inReadFloatArray),
                writeFloatArray(inWriteFloatArray), readData(inReadData),
                writeData(inWriteData), readRefcon(inReadRefcon), 
                writeRefcon(inWriteRefcon), good(true){};
  ~dataAccessor(){good = false;}
  void *get_w_refcon()const {return writeRefcon;};
  void *get_r_refcon()const {return readRefcon;};
  int get_int()const;
  float get_float()const;
  double get_double()const;
  int get_int_array(int v[], int offset, int max)const;
  int get_float_array(float v[], int offset, int max)const;
  int get_data(uint8_t v[], int offset, int max)const;
  void set_int(int v)const;
  void set_float(float v)const;
  void set_double(double v)const;
  void set_int_array(int v[], int offset, int max)const;
  void set_float_array(float v[], int offset, int max)const;
  void set_data(uint8_t v[], int offset, int max)const;

  void getName(std::string &s)const{s = name;};
  bool is_good()const{return good;};
  bool is_writable()const{return isWritable;};
  XPLMDataTypeID get_data_type()const{return datatype;};
  void add_notificator(XPLMDataChanged_f inNotificationFunc,
                                     void *inNotificationRefcon);
  int remove_notificator(XPLMDataChanged_f inNotificationFunc,
                                        void *inNotificationRefcon);
};


void dataAccessor::notify()const{
  std::vector<notificator>::const_iterator i;
  for(i = notificators.begin(); i != notificators.end(); ++i){
    (i->first)(i->second);
  }
}

void dataAccessor::add_notificator(XPLMDataChanged_f inNotificationFunc, void *inNotificationRefcon)
{
  notificators.push_back(std::pair<XPLMDataChanged_f, void *>(inNotificationFunc, inNotificationRefcon));
}

int dataAccessor::remove_notificator(XPLMDataChanged_f inNotificationFunc, void *inNotificationRefcon)
{
  std::vector<notificator>::iterator i;
  for(i = notificators.begin(); i != notificators.end(); ++i){
    if((i->first == inNotificationFunc) && (i->second == inNotificationRefcon)){
      notificators.erase(i);
      return 1;
    }
  }
  return 0;
}

int dataAccessor::get_int()const
{
  if((datatype & xplmType_Int) && readInt){
    return readInt(readRefcon);
  }else{
    return 0;
  }
}


float dataAccessor::get_float()const
{
  if((datatype & xplmType_Float) && readFloat){
    return readFloat(readRefcon);
  }else{
    return 0.0f;
  }
}

double dataAccessor::get_double()const
{
  if((datatype & xplmType_Double) && readDouble){
    return readDouble(readRefcon);
  }else{
    return 0.0;
  }
}

int dataAccessor::get_int_array(int v[], int offset, int max)const
{
  if((datatype & xplmType_IntArray) && readIntArray){
    return readIntArray(readRefcon, v, offset, max);
  }
  return 0;
}

int dataAccessor::get_float_array(float v[], int offset, int max)const
{
  if((datatype & xplmType_FloatArray) && readFloatArray){
    return readFloatArray(readRefcon, v, offset, max);
  }
  return 0;
}

int dataAccessor::get_data(uint8_t v[], int offset, int max)const
{
  if((datatype & xplmType_Data) && readData){
    return readData(readRefcon, (void *)v, offset, max);
  }
  return 0;
}

void dataAccessor::set_int(int v)const
{
  if((datatype & xplmType_Int) && isWritable && writeInt){
    writeInt(writeRefcon, v);
    notify();
  }
}

void dataAccessor::set_float(float v)const
{
  if((datatype & xplmType_Float) && isWritable && writeFloat){
    writeFloat(writeRefcon, v);
    notify();
  }
}

void dataAccessor::set_double(double v)const
{
  if((datatype & xplmType_Double) && isWritable && writeDouble){
    writeDouble(writeRefcon, v);
    notify();
  }
}

void dataAccessor::set_int_array(int v[], int offset, int max)const
{
  if((datatype & xplmType_IntArray) && writeIntArray){
    writeIntArray(readRefcon, v, offset, max);
    notify();
  }
}

void dataAccessor::set_float_array(float v[], int offset, int max)const
{
  if((datatype & xplmType_FloatArray) && writeFloatArray){
    writeFloatArray(writeRefcon, v, offset, max);
    notify();
  }
}

void dataAccessor::set_data(uint8_t v[], int offset, int max)const
{
  if((datatype & xplmType_Data) && writeData){
    writeData(writeRefcon, (void *)v, offset, max);
    notify();
  }
}

typedef std::map<std::string, dataAccessor *> daMap_t;
static daMap_t dataAccessors;

XPLMDataRef XPLMRegisterDataAccessor(const char *    inDataName,
                                     XPLMDataTypeID  inDataType,
                                     int             inIsWritable,
                                     XPLMGetDatai_f  inReadInt,
                                     XPLMSetDatai_f  inWriteInt,
                                     XPLMGetDataf_f  inReadFloat,
                                     XPLMSetDataf_f  inWriteFloat,
                                     XPLMGetDatad_f  inReadDouble,
                                     XPLMSetDatad_f  inWriteDouble,
                                     XPLMGetDatavi_f inReadIntArray,
                                     XPLMSetDatavi_f inWriteIntArray,
                                     XPLMGetDatavf_f inReadFloatArray,
                                     XPLMSetDatavf_f inWriteFloatArray,
                                     XPLMGetDatab_f  inReadData,
                                     XPLMSetDatab_f  inWriteData,
                                     void *          inReadRefcon,
                                     void *          inWriteRefcon
)
{
  std::string name(inDataName);
  dataAccessor *da = new dataAccessor(name, inDataType, inIsWritable, 
                                      inReadInt, inWriteInt, inReadFloat,
                                      inWriteFloat, inReadDouble, inWriteDouble,
                                      inReadIntArray, inWriteIntArray, 
                                      inReadFloatArray, inWriteFloatArray,
                                      inReadData, inWriteData, inReadRefcon,
                                      inWriteRefcon);
  dataAccessors.insert(std::pair<std::string, dataAccessor *>(name, da));
  //std::cout << name << da << std::endl;
  return da;
}

void XPLMUnregisterDataAccessor(XPLMDataRef inDataRef)
{
  //std::cout << "Unregister " << inDataRef << std::endl;
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  std::string name;
  da->getName(name);
  daMap_t::iterator i = dataAccessors.find(name);
  if(i != dataAccessors.end()){
    dataAccessors.erase(i);
  }
  delete da;
}

XPLMDataRef XPLMFindDataRef(const char *inDataRefName)
{
  std::string name(inDataRefName);
  daMap_t::iterator i = dataAccessors.find(name);
  if(i == dataAccessors.end()){
    return NULL;
  }
  return i->second;
}

int XPLMCanWriteDataRef(XPLMDataRef inDataRef)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  return da->is_writable();
}

int XPLMIsDataRefGood(XPLMDataRef inDataRef)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  return da->is_good();
}

XPLMDataTypeID XPLMGetDataRefTypes(XPLMDataRef inDataRef)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  return da->get_data_type();
}

int XPLMGetDatai(XPLMDataRef inDataRef)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  return da->get_int();
}

void XPLMSetDatai(XPLMDataRef inDataRef, int inValue)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  da->set_int(inValue);
}

float XPLMGetDataf(XPLMDataRef inDataRef)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  return da->get_float();
}

void XPLMSetDataf(XPLMDataRef inDataRef, float inValue)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  da->set_float(inValue);
}

double XPLMGetDatad(XPLMDataRef inDataRef)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  return da->get_double();
}

void XPLMSetDatad(XPLMDataRef inDataRef, double inValue)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  da->set_double(inValue);
}

int XPLMGetDatavi(XPLMDataRef inDataRef, int *outValues, int inOffset,
                  int inMax)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  return da->get_int_array(outValues, inOffset, inMax);
}

void XPLMSetDatavi(XPLMDataRef inDataRef, int *inValues, int inOffset,
                  int inCount)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  da->set_int_array(inValues, inOffset, inCount);
}

int XPLMGetDatavf(XPLMDataRef inDataRef, float *outValues, int inOffset,
                  int inMax)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  return da->get_float_array(outValues, inOffset, inMax);
}

void XPLMSetDatavf(XPLMDataRef inDataRef, float *inValues, int inOffset,
                  int inCount)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  da->set_float_array(inValues, inOffset, inCount);
}

int XPLMGetDatab(XPLMDataRef inDataRef, void *outValue, int inOffset,
                  int inMaxBytes)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  return da->get_data((uint8_t *)outValue, inOffset, inMaxBytes);
}

void XPLMSetDatab(XPLMDataRef inDataRef, void *inValue, int inOffset,
                  int inLength)
{
  dataAccessor *da = (dataAccessor *)inDataRef;
  assert(da != NULL);
  return da->set_data((uint8_t *)inValue, inOffset, inLength);
}


int shdReadInt(void *inRefcon)
{
  return *(int *)inRefcon;
}

void shdWriteInt(void *inRefcon, int inValue)
{
  *(int *)inRefcon = inValue;
}

float shdReadFloat(void *inRefcon)
{
  return *(float *)inRefcon;
}

void shdWriteFloat(void *inRefcon, float inValue)
{
  *(float *)inRefcon = inValue;
}

double shdReadDouble(void *inRefcon)
{
  return *(double *)inRefcon;
}

void shdWriteDouble(void *inRefcon, double inValue)
{
  *(double *)inRefcon = inValue;
}

int shdReadFloatArray(void *inRefcon, float *outValues, int inOffset, int inMax)
{
  (void)inRefcon;
  (void)outValues;
  (void)inOffset;
  (void)inMax;
  return 0;
}

void shdWriteFloatArray(void *inRefcon, float *inValues, int inOffset, int inCount)
{
  (void)inRefcon;
  (void)inValues;
  (void)inOffset;
  (void)inCount;
}

int shdReadIntArray(void *inRefcon, int *outValues, int inOffset, int inMax)
{
  (void)inRefcon;
  (void)outValues;
  (void)inOffset;
  (void)inMax;
  return 0;
}

void shdWriteIntArray(void *inRefcon, int *inValues, int inOffset, int inCount)
{
  (void)inRefcon;
  (void)inValues;
  (void)inOffset;
  (void)inCount;
}

int shdReadData(void *inRefcon, void *outValue, int inOffset, int inMax)
{
  (void)inRefcon;
  (void)outValue;
  (void)inOffset;
  (void)inMax;
  return 0;
}

void shdWriteData(void *inRefcon, void *inValue, int inOffset, int inCount)
{
  (void)inRefcon;
  (void)inValue;
  (void)inOffset;
  (void)inCount;
}





int XPLMShareData(const char *inDataName, XPLMDataTypeID inDataType,
                  XPLMDataChanged_f inNotificationFunc, 
                  void *inNotificationRefcon)
{
  dataAccessor *da = (dataAccessor *)XPLMFindDataRef(inDataName);
  if(da == NULL){
    void *ref = NULL;
    switch(inDataType){
      case xplmType_Int:
        ref = (void *)new int;
        break;
      case xplmType_Float:
        ref = (void *)new float;
        break;
      case xplmType_Double:
        ref = (void *)new double;
        break;
      default:
        std::cout << "Can't create array shared data..." << std::endl;
        return 0;
        //no way to know what size of arrays to alloc...
        break;
    }
    
    XPLMRegisterDataAccessor(inDataName, inDataType, 1,
      (inDataType == xplmType_Int)        ? shdReadInt         : NULL,
      (inDataType == xplmType_Int)        ? shdWriteInt        : NULL,
      (inDataType == xplmType_Float)      ? shdReadFloat       : NULL,
      (inDataType == xplmType_Float)      ? shdWriteFloat      : NULL,
      (inDataType == xplmType_Double)     ? shdReadDouble      : NULL,
      (inDataType == xplmType_Double)     ? shdWriteDouble     : NULL,
      (inDataType == xplmType_IntArray)   ? shdReadIntArray    : NULL,
      (inDataType == xplmType_IntArray)   ? shdWriteIntArray   : NULL,
      (inDataType == xplmType_FloatArray) ? shdReadFloatArray  : NULL,
      (inDataType == xplmType_FloatArray) ? shdWriteFloatArray : NULL,
      (inDataType == xplmType_Data)       ? shdReadData        : NULL,
      (inDataType == xplmType_Data)       ? shdWriteData       : NULL,
      ref, ref
    );
    da = (dataAccessor *)XPLMFindDataRef(inDataName);
  }
  if(da->get_data_type() != inDataType){
    std::cout << "Dataref " << inDataName << "type doesn't match; got " << 
                 inDataType << "but dataref is of type " << da->get_data_type() << std::endl;
    return 0;
  }
  da->add_notificator(inNotificationFunc, inNotificationRefcon);
  return 1;
}


int XPLMUnshareData(const char *inDataName, XPLMDataTypeID inDataType,
                  XPLMDataChanged_f inNotificationFunc, 
                  void *inNotificationRefcon)
{
  dataAccessor *da = (dataAccessor *)XPLMFindDataRef(inDataName);
  if(da == NULL){
    std::cout << "Dataref " << inDataName << "does not exist." << std::endl;
    return 0;
  }
  if(da->get_data_type() != inDataType){
    std::cout << "Dataref " << inDataName << "type doesn't match; got " << 
                 inDataType << "but dataref is of type " << da->get_data_type() << std::endl;
    return 0;
  }

  switch(inDataType){
    case xplmType_Int:
      delete (int *)da->get_w_refcon();
      break;
    case xplmType_Float:
      delete (float *)da->get_w_refcon();
      break;
    case xplmType_Double:
      delete (double *)da->get_w_refcon();
      break;
    default:
      break;
  }
  int res = da->remove_notificator(inNotificationFunc, inNotificationRefcon);
  XPLMUnregisterDataAccessor(da);
  return res;
}

