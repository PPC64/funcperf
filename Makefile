CXXFLAGS+=-std=c++14 -I${.CURDIR}

SUBDIRS=string
.PATH.cpp : ${SUBDIRS}

SRCS=TestRunner.cpp Tester.cpp

# add sources from subdirs
.for _subdir in ${SUBDIRS}
.include <${_subdir}/Makefile.inc>
.endfor

OBJS=${SRCS:S/.cpp/.o/}

all: tester

tester: ${OBJS}
	${CXX} ${LDFLAGS} -o tester ${OBJS} ${LDLIBS}

clean:
	rm -f ${OBJS} tester
