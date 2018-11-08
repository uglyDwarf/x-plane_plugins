.PHONY : clean

CC=clang
CXX=clang++

CFLAGS+=-Wall -Wextra -g
PY_CFLAGS=$(shell python3-config --cflags)
XPLM_CPPFLAGS=-I./SDK/CHeaders -I./SDK/CHeaders/XPLM -I./SDK/CHeaders/Widgets -DIBM=0 -DAPL=0 -DLIN=1
#COV_CFLAGS+= -fprofile-instr-generate -fcoverage-mapping
COV_FLAGS += 
PY_LDFLAGS=$(shell python3-config --ldflags)

all : main_200 main_210 main_300 plugin.so plugin.txt
	./main_300
	cat XPython.log
	cp XPython.log XPython300.log
	echo "======================================="
	./main_210
	cat XPython.log
	cp XPython.log XPython210.log
	echo "======================================="
	./main_200
	cat XPython.log
	cp XPython.log XPython200.log
	echo "======================================="

all_cov : CXXFLAGS+= ${COV_CFLAGS}
all_cov : CFLAGS+= ${COV_CFLAGS}
all_cov : main plugin.so
	./main
	llvm-profdata-3.8 merge default.profraw -o default.profdata
	llvm-cov-3.8 report ./plugin.so -instr-profile=default.profdata | tee coverage.txt
	llvm-cov-3.8 show ./plugin.so -instr-profile=default.profdata | c++filt -n >> coverage.txt

#main_300 : main.cpp chkDisplay.o300 chkGraphics.o300 chk_helper.o300 chkDataAccess.o300 chkUtilities.o300 \
#       chkScenery.o300 chkMenus.o300 chkNavigation.o300 chkPlugin.o300 chkPlanes.o300 chkProcessing.o300 \
#       chkCamera.o300 chkWidgets.o300 chkUIGraphics.o300 chkWidgetUtils.o300 chkInstance.o300 chkMap.o300 -ldl

#main : LDFLAGS+= -Wl,--export-dynamic -g 
#main : CPPFLAGS+= ${XPLM_CPPFLAGS} -g -DXPLM200 -DXPLM210 -DXPLM300
#main : CXXFLAGS+= ${COV_CFLAGS}
#main : CFLAGS+= ${COV_CFLAGS}
#main : main.cpp chkDisplay.o chkGraphics.o chk_helper.o chkDataAccess.o chkUtilities.o \
#       chkScenery.o chkMenus.o chkNavigation.o chkPlugin.o chkPlanes.o chkProcessing.o \
#       chkCamera.o chkWidgets.o chkUIGraphics.o chkWidgetUtils.o chkInstance.o chkMap.o -ldl

#main_200 : main.cpp chkDisplay.o200 chkGraphics.o200 chk_helper.o200 chkDataAccess.o200 chkUtilities.o200 \
#       chkScenery.o200 chkMenus.o200 chkNavigation.o200 chkPlugin.o200 chkPlanes.o200 chkProcessing.o200 \
#       chkCamera.o200 chkWidgets.o200 chkUIGraphics.o200 chkWidgetUtils.o200 chkInstance.o200 chkMap.o200 -ldl
#%.o200 : %.cpp
#	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $< -o $@




main_300 : LDFLAGS+= -Wl,--export-dynamic -g -ldl
main_300 : CPPFLAGS+= ${XPLM_CPPFLAGS} -g -DXPLM200 -DXPLM210 -DXPLM300
main_300 : CXXFLAGS+= ${COV_CFLAGS}
main_300 : CFLAGS+= ${COV_CFLAGS}
main_300 : main.cpp chkDisplay300.o chkGraphics300.o chk_helper300.o chkDataAccess300.o chkUtilities300.o \
       chkScenery300.o chkMenus300.o chkNavigation300.o chkPlugin300.o chkPlanes300.o chkProcessing300.o \
       chkCamera300.o chkWidgets300.o chkUIGraphics300.o chkWidgetUtils300.o chkInstance300.o chkMap300.o -ldl
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CFLAGS) $(LDFLAGS) $^ -o $@
chk%300.o  : chk%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $< -o $@


main_210 : LDFLAGS+= -Wl,--export-dynamic -g -o main_210
main_210 : CPPFLAGS+= ${XPLM_CPPFLAGS} -g -DXPLM200 -DXPLM210
main_210 : CXXFLAGS+= ${COV_CFLAGS}
main_210 : CFLAGS+= ${COV_CFLAGS}
main_210 : main.cpp chkDisplay210.o chkGraphics210.o chk_helper210.o chkDataAccess210.o chkUtilities210.o \
       chkScenery210.o chkMenus210.o chkNavigation210.o chkPlugin210.o chkPlanes210.o chkProcessing210.o \
       chkCamera210.o chkWidgets210.o chkUIGraphics210.o chkWidgetUtils210.o chkInstance210.o chkMap210.o -ldl
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CFLAGS) $(LDFLAGS) $^ -o $@
chk%210.o : chk%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $< -o $@


main_200 : LDFLAGS+= -Wl,--export-dynamic -g -o main_200
main_200 : CPPFLAGS+= ${XPLM_CPPFLAGS} -g -DXPLM200
main_200 : CXXFLAGS+= ${COV_CFLAGS}
main_200 : CFLAGS+= ${COV_CFLAGS}
main_200 : main.cpp chkDisplay200.o chkGraphics200.o chk_helper200.o chkDataAccess200.o chkUtilities200.o \
       chkScenery200.o chkMenus200.o chkNavigation200.o chkPlugin200.o chkPlanes200.o chkProcessing200.o \
chkCamera200.o chkWidgets200.o chkUIGraphics200.o chkWidgetUtils200.o chkInstance200.o chkMap200.o -ldl
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CFLAGS) $(LDFLAGS) $^ -o $@
chk%200.o : chk%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

plugin : CFLAGS+= ${PY_CFLAGS} -fPIC -g #-fsanitize=address
plugin : CPPFLAGS+= ${XPLM_CPPFLAGS} "-DWEAK_ATTR=__attribute__((weak))"
plugin : LDFLAGS+= ${PY_LDFLAGS} -shared -fPIC -fpic -ldl -g #-fsanitize=address
#plugin : plugin.c defs.c display.c utils.c graphics.c data_access.c utilities.c scenery.c menus.c
plugin : plugin.o defs.o display.o utils.o graphics.o data_access.o utilities.o scenery.o menus.o \
         navigation.o plugins.o planes.o processing.o camera.o widget_defs.o widgets.o \
         standard_widgets.o uigraphics.o widgetutils.o instance.o map.o plugin_dl.o

valenda : LDFLAGS+= -Wl,--export-dynamic -g
valenda : CPPFLAGS+= ${XPLM_CPPFLAGS} -g -DTESTING
valenda : chkWidgets.o chk_helper.o chkDataAccess.o
	clang++ -o valenda chk_helper.o chkWidgets.o chkDataAccess.o

plugin.txt : plugin.so
	objdump -d plugin.so > plugin.txt

plugin.so : plugin
	cp plugin plugin.so

valgrind : main plugin.so plugin.txt
	valgrind --tool=memcheck --suppressions=valgrind-python.supp --leak-check=full ./main

clean :
	rm -f *.so *.o main main_300 main_210 main_200 plugin default.prof* coverage.txt *.log

