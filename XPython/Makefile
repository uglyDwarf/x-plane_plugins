.PHONY : clean

CC=clang
CXX=clang++

CFLAGS+=-Wall -Wextra -g
PY_CFLAGS=$(shell python3-config --cflags)
XPLM_CPPFLAGS=-I./SDK/CHeaders -DIBM=0 -DAPL=0 -DLIN=1
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


main : LDFLAGS+= -Wl,--export-dynamic -ldl -g
main : CPPFLAGS+= ${XPLM_CPPFLAGS} -g
main : main.cpp chkDisplay.cpp chkGraphics.cpp chk_helper.cpp chkDataAccess.cpp

plugin : CFLAGS+= ${PY_CFLAGS}
plugin : CPPFLAGS+= ${XPLM_CPPFLAGS}
plugin : LDFLAGS+= ${PY_LDFLAGS} -shared -fPIC -fpic
plugin : plugin.c defs.c display.c utils.c graphics.c data_access.c

plugin.so : plugin
	cp plugin plugin.so

clean :
	rm -f *.so *.o main plugin default.prof* coverage.txt

