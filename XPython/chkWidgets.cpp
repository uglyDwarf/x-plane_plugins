#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sstream>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMDataAccess.h>
#include <Widgets/XPWidgetDefs.h>
#include <Widgets/XPWidgets.h>

#include "chk_helper.h"

static std::map<class widget *, int> widgets;


static int int0, int1, int2, int3;
static XPLMDataRef d0, d1, d2, d3;

class widget;
static widget *frontWidget = NULL;
static widget *focused = NULL;

static int widgetCbk(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2);



void initWidgetsModule()
{
  d0 = registerROAccessor("widgets/int0", int0);
  d1 = registerROAccessor("widgets/int1", int1);
  d2 = registerROAccessor("widgets/int2", int2);
  d3 = registerROAccessor("widgets/int3", int3);
}

void cleanupWidgetsModule()
{
  XPLMUnregisterDataAccessor(d0);
  XPLMUnregisterDataAccessor(d1);
  XPLMUnregisterDataAccessor(d2);
  XPLMUnregisterDataAccessor(d3);
}

class widget{
  static intptr_t globalId;
  int left, top, right, bottom, visible, isRoot, widgetClass;
  std::string descriptor;
  intptr_t id;
  std::vector<class widget *> children;
  class widget* parent;
  std::map<XPWidgetPropertyID, intptr_t> properties;
  std::list<XPWidgetFunc_t> callbacks;
  bool remove_children;
 public:
  widget(int inLeft, int inTop, int inRight, int inBottom, int inVisible, std::string inDescriptor, int inIsRoot, 
         void *inContainer, int inClass = 11, XPWidgetFunc_t inCallback = NULL);
  ~widget();
  void *getID(){return (void*)id;};
  int message(XPWidgetMessage inMessage, intptr_t inParam1, intptr_t inParam2);
  void addChild(widget *ch){children.push_back(ch);ch->setParent(this);};
  void removeChild(const widget *ch);
  int getChildCount(){return children.size();};
  class widget* getNthChild(int n){return children.at(n);};
  void setParent(class widget *p){parent = p;};
  class widget *getParent(){return parent;};
  void setVisible(int v){visible = v;};
  int getVisible(){return visible * widgetClass;};
  void getGeometry(int *outLeft, int *outTop, int *outRight, int *outBottom){*outLeft = left; *outTop = top;
                                                                             *outRight = right; *outBottom = bottom;};
  void setGeometry(int inLeft, int inTop, int inRight, int inBottom){left = inLeft; top = inTop;
                                                                     right = inRight; bottom = inBottom;};
  widget *getChild(int inXOffset, int inYOffset, int inRecursive, int inVisibleOnly);
  bool contains_pt(int pt_x, int pt_y);
  void exposed(int *l, int *t, int *r, int *b);
  void setDescriptor(const char *d){descriptor = d;};
  const std::string &getDescriptor(){return descriptor;};
  void setProperty(XPWidgetPropertyID property, intptr_t value){properties[property] = value;};
  intptr_t getProperty(XPWidgetPropertyID property, int *exists);
  void addCallback(XPWidgetFunc_t cbk){callbacks.push_front(cbk);};
  void setRemoveChildren(){remove_children = true;};
};

intptr_t widget::globalId = 444;

static bool removeWidget(XPWidgetID inWidget)
{
  std::map<class widget *, int>::iterator i = widgets.find(static_cast<widget *>(inWidget));
  if(i == widgets.end()){
    return false;
  }
  //widget *tmp = i->first;
  widgets.erase(i);
  return true;
}


widget::widget(int inLeft, int inTop, int inRight, int inBottom, int inVisible, std::string inDescriptor,
         int inIsRoot, void *inContainer, int inClass, XPWidgetFunc_t inCallback):
           left(inLeft), top(inTop), right(inRight), bottom(inBottom),
           visible(inVisible), isRoot(inIsRoot), widgetClass(inClass), descriptor(inDescriptor),
           parent(static_cast<widget*>(inContainer)), remove_children(false)
{
  if(inCallback){
    addCallback(inCallback);

  }
  if(isRoot){
    frontWidget = this;
  }
  id = globalId++;
  widgets[this] = 1;
  setProperty(xpProperty_UserStart, isRoot);
  setProperty(xpProperty_UserStart + 1, inClass);
}

widget::~widget()
{
  if(parent){
    parent->removeChild(this);
  }
  if(remove_children){
    std::vector<class widget *>::iterator i;
    // Children remove themselves from the list, so normal iteration
    //   is not possible
    while((i = children.begin()) != children.end()){
      (*i)->setRemoveChildren();
      removeWidget(*i);
      delete *i;
      *i = NULL;
    }
  }
  message(xpMsg_Destroy, (remove_children ? 1 : 0), 0);
  removeWidget(this);
}

void widget::removeChild(const widget *ch)
{
  std::vector<widget*>::iterator i;
  for(i = children.begin(); i != children.end(); ++i){
    if(*i == ch){
      children.erase(i);
      break;
    }
  }
}

int widget::message(XPWidgetMessage inMessage, intptr_t inParam1, intptr_t inParam2)
{
  std::list<XPWidgetFunc_t>::iterator i = callbacks.begin();
  int res = 0;

  while((res == 0) && (i != callbacks.end())){
    res = (*i)(inMessage, static_cast<void *>(this), inParam1, inParam2);
    ++i;
  }
  return res;
}

widget *widget::getChild(int inXOffset, int inYOffset, int inRecursive, int inVisibleOnly)
{
  widget *res = NULL;
  for(size_t i = 0; i < children.size(); ++i){
    if(inVisibleOnly && (!children[i]->getVisible())){
      continue;
    }
    if(children[i]->contains_pt(inXOffset, inYOffset)){
      res = children[i];
      break;
    }
  }
  if(res && inRecursive){
    widget *tmp = res->getChild(inXOffset, inYOffset, inRecursive, inVisibleOnly);
    if(tmp != NULL){
      res = tmp;
    }
  }
  return res;
}

bool widget::contains_pt(int pt_x, int pt_y)
{
  return (pt_y >= bottom) && (pt_y <= top) && (pt_x >= left) && (pt_x <= right);
}

void widget::exposed(int *l, int *t, int *r, int *b)
{
  if(*l < left){
    *l = left;
  }
  if(*r > right){
    *r = right;
  }
  if(*t > top){
    *t = top;
  }
  if(*b < bottom){
    *b = bottom;
  }

  if(parent){
    parent->exposed(l, t, r, b);
  }
}


intptr_t widget::getProperty(XPWidgetPropertyID property, int *exists)
{
  std::map<XPWidgetPropertyID, intptr_t>::iterator i = properties.find(property);
  if(i != properties.end()){
    if(exists){
      *exists = 1;
    }
    return i->second;
  }else{
    if(exists){
      *exists = 0;
    }
  }
  return 0;
}

/*
typedef int XPWidgetClass;
typedef void * XPWidgetID;
*/

XPWidgetID XPCreateWidget(int inLeft, int inTop, int inRight, int inBottom, int inVisible, const char * inDescriptor,
                          int inIsRoot, XPWidgetID inContainer, XPWidgetClass inClass)
{
  widget *tmp = new widget(inLeft, inTop, inRight, inBottom, inVisible, inDescriptor, inIsRoot, inContainer, inClass);
  tmp->addCallback(widgetCbk);
  int0 = inIsRoot;
  int1 = inClass;
  if(inContainer != NULL){
    static_cast<widget*>(inContainer)->addChild(tmp);
  }
  return static_cast<void*>(tmp);
}

XPWidgetID XPCreateCustomWidget(int inLeft, int inTop, int inRight, int inBottom, int inVisible, const char * inDescriptor,
                          int inIsRoot, XPWidgetID inContainer, XPWidgetFunc_t inCallback)
{
  widget *tmp = new widget(inLeft, inTop, inRight, inBottom, inVisible, inDescriptor, inIsRoot, inContainer, 11, inCallback);
  int0 = inIsRoot;
  if(inContainer != NULL){
    static_cast<widget*>(inContainer)->addChild(tmp);
  }
  return static_cast<void*>(tmp);
}

void XPDestroyWidget(XPWidgetID inWidget, int inDestroyChildren)
{
  widget *w = static_cast<widget *>(inWidget);
  if(inDestroyChildren){
    w->setRemoveChildren();
  }
  delete w;
  int0 = inDestroyChildren;
  int1 = widgets.size();

  /*
  std::cout << "Remaining widgets:" << std::endl;
  std::map<class widget *, int>::iterator i;
  for(i = widgets.begin(); i != widgets.end(); ++i){
    std::cout << "  \"" << i->first->getDescriptor() << "\"" << std::endl;
  }
  std::cout << std::endl;
  */
}

int XPSendMessageToWidget(XPWidgetID inWidget, XPWidgetMessage inMessage, XPDispatchMode inMode, intptr_t inParam1,
                          intptr_t inParam2)
{
  widget *tgt = static_cast<widget *>(inWidget);
  int0 = inMode;
  return tgt->message(inMessage, inParam1, inParam2);
}

void XPPlaceWidgetWithin(XPWidgetID inSubWidget, XPWidgetID inContainer)
{
  widget *tgt = static_cast<widget *>(inContainer);
  tgt->addChild(static_cast<widget *>(inSubWidget));
}

int XPCountChildWidgets(XPWidgetID inWidget)
{
  widget *tgt = static_cast<widget *>(inWidget);
  return tgt->getChildCount();
}

XPWidgetID XPGetNthChildWidget(XPWidgetID inWidget, int inIndex)
{
  widget *tgt = static_cast<widget *>(inWidget);
  return tgt->getNthChild(inIndex);
}

XPWidgetID XPGetParentWidget(XPWidgetID inWidget)
{
  widget *tgt = static_cast<widget *>(inWidget);
  return tgt->getParent();
}

void XPShowWidget(XPWidgetID inWidget)
{
  widget *tgt = static_cast<widget *>(inWidget);
  tgt->setVisible(true);
}

void XPHideWidget(XPWidgetID inWidget)
{
  widget *tgt = static_cast<widget *>(inWidget);
  tgt->setVisible(false);
}

int XPIsWidgetVisible(XPWidgetID inWidget)
{
  widget *tgt = static_cast<widget *>(inWidget);
  return tgt->getVisible();
}

XPWidgetID XPFindRootWidget(XPWidgetID inWidget)
{
  widget *tgt = static_cast<widget *>(inWidget);
  widget *child;
  while((child = tgt->getParent())){
    tgt = child;
  }
  return tgt;
}

void XPBringRootWidgetToFront(XPWidgetID inWidget)
{
  frontWidget = static_cast<widget *>(XPFindRootWidget(inWidget));
}

int XPIsWidgetInFront(XPWidgetID inWidget)
{
  return static_cast<XPWidgetID>(frontWidget) == inWidget;
}

void XPGetWidgetGeometry(XPWidgetID inWidget, int *outLeft, int *outTop, int *outRight, int *outBottom)
{
  widget *tgt = static_cast<widget *>(inWidget);
  tgt->getGeometry(outLeft, outTop, outRight, outBottom);
}

void XPSetWidgetGeometry(XPWidgetID inWidget, int inLeft, int inTop, int inRight, int inBottom)
{
  widget *tgt = static_cast<widget *>(inWidget);
  tgt->setGeometry(inLeft, inTop, inRight, inBottom);
}

XPWidgetID XPGetWidgetForLocation(XPWidgetID inContainer, int inXOffset, int inYOffset, int inRecursive, int inVisibleOnly)
{
  int0 = inXOffset;
  int1 = inYOffset;
  int2 = inRecursive;
  int3 = inVisibleOnly;
  widget *tgt = static_cast<widget *>(inContainer);
  return tgt->getChild(inXOffset, inYOffset, inRecursive, inVisibleOnly);
}

void XPGetWidgetExposedGeometry(XPWidgetID inWidgetID, int *outLeft, int *outTop, int *outRight, int *outBottom)
{
  widget *tgt = static_cast<widget *>(inWidgetID);
  tgt->getGeometry(outLeft, outTop, outRight, outBottom);
  tgt->exposed(outLeft, outTop, outRight, outBottom);
}

void XPSetWidgetDescriptor(XPWidgetID inWidget, const char *inDescriptor)
{
  widget *tgt = static_cast<widget *>(inWidget);
  tgt->setDescriptor(inDescriptor);
}

int XPGetWidgetDescriptor(XPWidgetID inWidget, char *outDescriptor, int inMaxDescLength)
{
  widget *tgt = static_cast<widget *>(inWidget);
  const std::string &tmp = tgt->getDescriptor();
  if(outDescriptor != NULL){
    strncpy(outDescriptor, tmp.c_str(), inMaxDescLength);
  }
  return tmp.length();
}

void XPSetWidgetProperty(XPWidgetID inWidget, XPWidgetPropertyID inProperty, intptr_t inValue)
{
  widget *tgt = static_cast<widget *>(inWidget);
  tgt->setProperty(inProperty, inValue);
}

intptr_t XPGetWidgetProperty(XPWidgetID inWidget, XPWidgetPropertyID inProperty, int *inExists)
{
  widget *tgt = static_cast<widget *>(inWidget);
  return tgt->getProperty(inProperty, inExists);
}

XPWidgetID XPSetKeyboardFocus(XPWidgetID inWidget)
{
  widget *tgt = static_cast<widget *>(inWidget);
  focused = tgt;
  return tgt;
}

void XPLoseKeyboardFocus(XPWidgetID inWidget)
{
  widget *tgt = static_cast<widget *>(inWidget);
  while(tgt->getParent()){
    tgt = tgt->getParent();
  }
  focused = tgt;
}

XPWidgetID XPGetWidgetWithFocus(void)
{
  return focused;
}

void XPAddWidgetCallback(XPWidgetID inWidget, XPWidgetFunc_t inNewCallback)
{
  widget *tgt = static_cast<widget *>(inWidget);
  tgt->addCallback(inNewCallback);
}

static int widgetCbk(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2)
{
  widget *tgt = static_cast<widget *>(inWidget);
  int0 = inMessage;
  int1 = inParam1;
  int2 = inParam2;
  int3 = (intptr_t) tgt;
  return int0 + 2 * int1 + 3 * int2;
}

XPWidgetFunc_t XPGetWidgetClassFunc(XPWidgetClass inWidgetClass)
{
  int0 = inWidgetClass;
  return widgetCbk;
}


#ifdef TESTING

int main(int argc, char *argv[])
{
  initWidgetsModule();
  std::cout << "Testing" << std::endl;

  XPWidgetID parent = XPCreateWidget(0, 1, 2, 3, 1, "Parent", 1, 0, 0);
  XPWidgetID child = XPCreateWidget(0, 1, 2, 3, 1, "Child", 0, parent, 0);

  XPDestroyWidget(parent, 1);
  std::cout << "Testing end" << std::endl;

  XPLMUnregisterDataAccessor(d0);
  XPLMUnregisterDataAccessor(d1);
  XPLMUnregisterDataAccessor(d2);
  XPLMUnregisterDataAccessor(d3);
  std::cout << "Finished" << std::endl;
}



#endif

