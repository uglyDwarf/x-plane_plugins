.PHONY : clean

CC=clang
CXX=clang++

CFLAGS+=-Wall -Wextra -g
PY_CFLAGS=$(shell python3-config --cflags)
XPLM_CPPFLAGS=-I./SDK/CHeaders -I./SDK/CHeaders/XPLM -I./SDK/CHeaders/Widgets -DIBM=0 -DAPL=0 -DLIN=1
COV_CFLAGS+= -fprofile-instr-generate -fcoverage-mapping

PY_LDFLAGS=$(shell python3-config --ldflags)

all : main plugin.so
	./main

#all_cov : CXXFLAGS+= ${COV_CFLAGS}
all_cov : CFLAGS+= ${COV_CFLAGS}
all_cov : main plugin.so
	./main
	llvm-profdata-3.8 merge default.profraw -o default.profdata
	llvm-cov-3.8 report ./plugin.so -instr-profile=default.profdata | tee coverage.txt
	llvm-cov-3.8 show ./plugin.so -instr-profile=default.profdata | c++filt -n >> coverage.txt


main : LDFLAGS+= -Wl,--export-dynamic -g
main : CPPFLAGS+= ${XPLM_CPPFLAGS} -g
main : main.cpp chkDisplay.o chkGraphics.o chk_helper.o chkDataAccess.o chkUtilities.o \
       chkScenery.o chkMenus.o chkNavigation.o chkPlugin.o chkPlanes.o chkProcessing.o \
       chkCamera.o chkWidgets.o chkUIGraphics.o chkWidgetUtils.o -ldl
#main : main.cpp chkDisplay.cpp chkGraphics.cpp chk_helper.cpp chkDataAccess.cpp chkUtilities.cpp \
#       chkScenery.cpp chkMenus.cpp chkPlugin.cpp -ldl

plugin : CFLAGS+= ${PY_CFLAGS} -fPIC #-fsanitize=address
plugin : CPPFLAGS+= ${XPLM_CPPFLAGS}
plugin : LDFLAGS+= ${PY_LDFLAGS} -shared -fPIC -fpic -ldl #-fsanitize=address
#plugin : plugin.c defs.c display.c utils.c graphics.c data_access.c utilities.c scenery.c menus.c
plugin : plugin.o defs.o display.o utils.o graphics.o data_access.o utilities.o scenery.o menus.o \
         navigation.o plugins.o planes.o processing.o camera.o widget_defs.o widgets.o \
         standard_widgets.o uigraphics.o widgetutils.o

plugin.so : plugin
	cp plugin plugin.so

clean :
	rm -f *.so *.o main plugin default.prof* coverage.txt

