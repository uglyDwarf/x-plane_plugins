.PHONY : clean

CC=clang
CXX=clang++

CFLAGS+=-Wall -Wextra -g
PY_CFLAGS=$(shell python3-config --cflags)
XPLM_CPPFLAGS=-I./SDK/CHeaders -I./SDK/CHeaders/XPLM -I./SDK/CHeaders/Widgets -DIBM=0 -DAPL=0 -DLIN=1
COV_CFLAGS+= -fprofile-instr-generate -fcoverage-mapping
#COV_FLAGS += 
PY_LDFLAGS=$(shell python3-config --ldflags)

all : main_200 main_210 main_300 main_301 plugin.so plugin.txt
	./main_301
	cat XPython.log
	cp XPython.log XPython301.log
	echo "======================================="
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
all_cov : main_301 plugin.so
	llvm-profdata-6.0 merge default.profraw -o default.profdata
	llvm-cov-6.0 report ./main_301 -instr-profile=default.profdata | tee coverage.txt
	llvm-cov-6.0 show ./main_301 -instr-profile=default.profdata | c++filt -n >> coverage.txt

main_301 : LDFLAGS+= -Wl,--export-dynamic -g -ldl
main_301 : CPPFLAGS+= ${XPLM_CPPFLAGS} -g -DXPLM200 -DXPLM210 -DXPLM300 -DXPLM301
main_301 : CXXFLAGS+= ${COV_CFLAGS}
main_301 : CFLAGS+= ${COV_CFLAGS}
main_301 : main.cpp chkDisplay301.o chkGraphics301.o chk_helper301.o chkDataAccess301.o chkUtilities301.o \
       chkScenery301.o chkMenus301.o chkNavigation301.o chkPlugin301.o chkPlanes301.o chkProcessing301.o \
       chkCamera301.o chkWidgets301.o chkUIGraphics301.o chkWidgetUtils301.o chkInstance301.o chkMap301.o -ldl
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CFLAGS) $(LDFLAGS) $^ -o $@
chk%301.o  : chk%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

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

plugin : CFLAGS+= ${PY_CFLAGS} -fPIC -g
plugin : CPPFLAGS+= ${XPLM_CPPFLAGS}
plugin : LDFLAGS+= ${PY_LDFLAGS} -shared -fPIC -fpic -ldl -g
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

