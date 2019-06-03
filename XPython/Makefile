.PHONY : clean all 32bit 64bit darwin test

PLUGIN_PATH="Resources/plugins/XPythonRevival"
PY_CFLAGS = $(shell python3-config --cflags)
PY_LDFLAGS = $(shell python3-config --ldflags)
CPPFLAGS += -I../SDK/CHeaders -I../SDK/CHeaders/XPLM -I../SDK/CHeaders/Widgets

all : main_200 main_210 main_300 main_301

ifeq ($(TARGET), Linux)
LDFLAGS += -Wl,--export-dynamic
CPPFLAGS += -DIBM=0 -DAPL=0 -DLIN=1
all : lin.xpl
test : 32bit 64bit
	rm -f XPython*.log
	XPYTHON_LOG=XPython301_32.log ./32/main_301
	XPYTHON_LOG=XPython300_32.log ./32/main_300
	XPYTHON_LOG=XPython210_32.log ./32/main_210
	XPYTHON_LOG=XPython200_32.log ./32/main_200
	XPYTHON_LOG=XPython301_64.log ./64/main_301
	XPYTHON_LOG=XPython300_64.log ./64/main_300
	XPYTHON_LOG=XPython210_64.log ./64/main_210
	XPYTHON_LOG=XPython200_64.log ./64/main_200
	tail -n1 XPython*.log
endif

ifeq ($(BITS), 64)
CFLAGS += -m64
endif

ifeq ($(BITS), 32)
CFLAGS += -m32
endif

ifeq ($(TARGET), Darwin)
CXXFLAGS += -arch i386 -arch x86_64
CPPFLAGS += -DIBM=0 -DAPL=1 -DLIN=0
all : mac.xpl
test : darwin
	rm -f XPython*.log
	XPYTHON_LOG=XPython301_mac.log ./mac/main_301
	XPYTHON_LOG=XPython300_mac.log ./mac/main_300
	XPYTHON_LOG=XPython210_mac.log ./mac/main_210
	XPYTHON_LOG=XPython200_mac.log ./mac/main_200
	tail -n1 XPython*.log
endif

COV_FLAGS += 
LDFLAGS+= -g -ldl
CFLAGS+=-Wall -Wextra -g

32bit:
	cd 32 && $(MAKE)

64bit:
	cd 64 && $(MAKE)

darwin:
	cd mac && $(MAKE)


all_cov : CXXFLAGS+= ${COV_CFLAGS}
all_cov : CFLAGS+= ${COV_CFLAGS}
all_cov : main_301 lin.xpl
	llvm-profdata-6.0 merge default.profraw -o default.profdata
	llvm-cov-6.0 report ./main_301 -instr-profile=default.profdata | tee coverage.txt
	llvm-cov-6.0 show ./main_301 -instr-profile=default.profdata | c++filt -n >> coverage.txt

%301.o  : %.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -DXPLM200 -DXPLM210 -DXPLM300 -DXPLM301 $< -o $@
%300.o  : %.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -DXPLM200 -DXPLM210 -DXPLM300 $< -o $@
%210.o : %.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -DXPLM200 -DXPLM210 $< -o $@
%200.o : %.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -DXPLM200 $< -o $@

CHK_OBJ =  mainXXX.o chkDisplayXXX.o chkGraphicsXXX.o chk_helperXXX.o chkDataAccessXXX.o chkUtilitiesXXX.o \
           chkSceneryXXX.o chkMenusXXX.o chkNavigationXXX.o chkPluginXXX.o chkPlanesXXX.o chkProcessingXXX.o \
           chkCameraXXX.o chkWidgetsXXX.o chkUIGraphicsXXX.o chkWidgetUtilsXXX.o chkInstanceXXX.o chkMapXXX.o

main_301 : $(subst XXX,301,$(CHK_OBJ))
	$(CXX) $(CXXFLAGS) $(CFLAGS) $^ $(LDFLAGS) -o $@

main_300 : $(subst XXX,300,$(CHK_OBJ))
	$(CXX) $(CXXFLAGS) $(CFLAGS) $^ $(LDFLAGS) -o $@

main_210 : $(subst XXX,210,$(CHK_OBJ))
	$(CXX) $(CXXFLAGS) $(CFLAGS) $^ $(LDFLAGS) -o $@

main_200 : $(subst XXX,200,$(CHK_OBJ))
	$(CXX) $(CXXFLAGS) $(CFLAGS) $^ $(LDFLAGS) -o $@


lin.xpl : CFLAGS+= ${PY_CFLAGS} -fpic -fPIC -g -fvisibility=hidden
lin.xpl : CPPFLAGS+= ${XPLM_CPPFLAGS} -DIBM=0 -DAPL=0 -DLIN=1
lin.xpl : LDFLAGS+= ${PY_LDFLAGS} -shared -fPIC -fpic -ldl -g -fvisibility=hidden
lin.xpl : plugin.o defs.o display.o utils.o graphics.o data_access.o utilities.o scenery.o menus.o \
          navigation.o plugins.o planes.o processing.o camera.o widget_defs.o widgets.o \
          standard_widgets.o uigraphics.o widgetutils.o instance.o map.o plugin_dl.o sb.o utils.o
	$(CC) $(CXXFLAGS) $(CFLAGS) $^ $(LDFLAGS) -o $@

mac.xpl : CFLAGS+= ${PY_CFLAGS} -fpic -fPIC -g -fvisibility=hidden
mac.xpl : CPPFLAGS+= ${XPLM_CPPFLAGS} -DIBM=0 -DAPL=1 -DLIN=0
mac.xpl : LDFLAGS+= ${PY_LDFLAGS} -shared -fPIC -fpic -ldl -g -fvisibility=hidden -undefined dynamic_lookup
mac.xpl : plugin.o defs.o display.o utils.o graphics.o data_access.o utilities.o scenery.o menus.o \
          navigation.o plugins.o planes.o processing.o camera.o widget_defs.o widgets.o \
          standard_widgets.o uigraphics.o widgetutils.o instance.o map.o plugin_dl.o sb.o utils.o
	$(CC) $(CXXFLAGS) $(CFLAGS) $^ $(LDFLAGS) -o $@

SUBDIRS = . 32 mac 64
SUBDIRSCLEAN=$(addsuffix .clean,$(SUBDIRS))

clean : $(SUBDIRSCLEAN)

clean_current_directory:
	rm -f *.so *.o main_??? ???.xpl default.prof* coverage.txt *.log

%.clean: %
	$(MAKE) -C $< clean_current_directory

