.PHONY : clean darwin lin windows

PLUGIN_PATH=Resources/plugins/XPPython3
XP_SDK=../SDK

#########
# COMMON opts
CFLAGS=-Wall -Wextra -g -fpic -fPIC -fvisibility=hidden
XPLM_CPPFLAGS = -DXPLM200 -DXPLM210 -DXPLM300 -DXPLM301 -DXPLM302 -DXPLM303 \
                -I$(XP_SDK)/CHeaders -I$(XP_SDK)/CHeaders/XPLM -I$(XP_SDK)/CHeaders/Widgets

##########
# PYTHON opts
# with python 3.8, new python-config option --embed.. I've not tried it yet

ifeq ($(TARGET), Windows)
  # perhaps
  PYDEV=/c/Program\ Files/Python38
  PY_CFLAGS = -I${PYDEV}/include -Wno-unused-result -Wsign-compare -Wunreachable-code -fno-common -DNDEBUG -fwrapv -O3
  PY_LDFLAGS = -L. -lpython3.dll
  CFLAGS+= ${PY_CFLAGS} ${XPLM_CPPFLAGS} -DIBM=1 -DAPL=0 -DLIN=0 -I/c/msys64/mingw64/include -I/usr/include
  LDFLAGS+= -shared -static-libgcc -static-libstdc++ -static -lpthread  ${PY_LDFLAGS} -L/c/msys64/mingw64/lib -ldl -lregex -ltre -lintl -liconv $(XP_SDK)/Libraries/Win/XPLM_64.lib $(XP_SDK)/Libraries/Win/XPWidgets_64.lib
  PLUGIN_DIR=win_x64
  LD=g++
endif

ifeq ($(TARGET), Linux)
  PY_CFLAGS = $(shell python3-config --cflags)
  PY_LDFLAGS = $(shell python3-config --ldflags)
  CFLAGS+= ${PY_CFLAGS} ${XPLM_CPPFLAGS} -DIBM=0 -DAPL=0 -DLIN=1
  LDFLAGS += ${PY_LDFLAGS} -shared -fPIC -fpic -ldl -g -fvisibility=hidden -Wl,--export-dynamic
  PLUGIN_DIR=lin_x64
  LD=cc
endif

ifeq ($(TARGET), Darwin)
  # PY_CFLAGS = $(shell /Users/pbuck/bin/python3-config --cflags)
  # PY_LDFLAGS = $(shell /Users/pbuck/bin/python3-config --ldflags)
  PY_CFLAGS = $(shell python-config --cflags)
  PY_LDFLAGS = $(shell python-config --ldflags)
  CFLAGS += ${PY_CFLAGS} ${XPLM_CPPFLAGS} -DIBM=0 -DAPL=1 -DLIN=0 -arch x86_64
  LDFLAGS += ${PY_LDFLAGS} -shared -fPIC -fpic -ldl -g -fvisibility=hidden -undefined dynamic_lookup
  PLUGIN_DIR=mac_x64
  LD=cc
endif

#########
# Build targets
windows:
	cd win && $(MAKE)
	cd ..

darwin:
	cd mac && $(MAKE)
	cd ..

linux:
	cd lin && $(MAKE)
	cd ..

%.o  : %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@

xppython3.xpl : plugin.o defs.o display.o utils.o graphics.o data_access.o utilities.o scenery.o menus.o \
	navigation.o plugins.o planes.o processing.o camera.o widget_defs.o widgets.o \
	standard_widgets.o uigraphics.o widgetutils.o instance.o map.o plugin_dl.o sb.o utils.o
	$(LD) $^ $(LDFLAGS) -o $@ 

SUBDIRS = . mac lin win
SUBDIRSCLEAN=$(addsuffix .clean,$(SUBDIRS))

clean : $(SUBDIRSCLEAN)

clean_current_directory:
	rm -f *.so *.o main_??? xppython3.xpl default.prof* coverage.txt *.log *~ a.out

%.clean: %
	$(MAKE) -C $< clean_current_directory


