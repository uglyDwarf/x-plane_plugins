.PHONY : clean

CC=clang
CXX=clang++

PY_CFLAGS=$(shell python3-dbg-config --cflags)
PY_LDFLAGS=$(shell python3-dbg-config --ldflags)
XPLM_CPPFLAGS=-I./SDK/CHeaders -I./SDK/CHeaders/XPLM -I./SDK/CHeaders/Widgets -DIBM=0 -DAPL=0 -DLIN=1
COV_FLAGS += 
#COV_CFLAGS+= -fprofile-instr-generate -fcoverage-mapping
LDFLAGS+= -Wl,--export-dynamic -g -ldl
#LDFLAGS+= -fsanitize=address -fno-omit-frame-pointer
CPPFLAGS+= ${XPLM_CPPFLAGS}
CFLAGS+=-Wall -Wextra -g

all : lin.xpl

test : main_200 main_210 main_300 main_301 lin.xpl
	XPYTHON_LOG=XPython301.log ./main_301
	XPYTHON_LOG=XPython300.log ./main_300
	XPYTHON_LOG=XPython210.log ./main_210
	XPYTHON_LOG=XPython200.log ./main_200
	tail -n1 XPython*.log

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
lin.xpl : CPPFLAGS+= ${XPLM_CPPFLAGS}
lin.xpl : LDFLAGS+= ${PY_LDFLAGS} -shared -fPIC -fpic -ldl -g -fvisibility=hidden
lin.xpl : plugin.o defs.o display.o utils.o graphics.o data_access.o utilities.o scenery.o menus.o \
          navigation.o plugins.o planes.o processing.o camera.o widget_defs.o widgets.o \
          standard_widgets.o uigraphics.o widgetutils.o instance.o map.o plugin_dl.o sb.o utils.o
	$(CC) $(CXXFLAGS) $(CFLAGS) $^ $(LDFLAGS) -o $@

valgrind : main_301 lin.xpl
	valgrind --tool=memcheck --suppressions=valgrind-python.supp --leak-check=full ./main_301

clean :
	rm -f *.so *.o main_??? lin.xpl default.prof* coverage.txt *.log

