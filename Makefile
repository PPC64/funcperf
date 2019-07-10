CXXFLAGS+=-std=c++14 -I${.CURDIR} -Wall -Werror

SRCS=Tester.cpp \
	MemcpyFunctionTest.cpp MemcpyTestParams.cpp MemcpyTest.cpp \
	StrcpyFunctionTest.cpp StrcpyTest.cpp \
	StrncpyFunctionTest.cpp \
	StrcmpFunctionTest.cpp StrcmpTestParams.cpp StrcmpTest.cpp

OBJS:=${SRCS:S/.cpp/.o/}

all: tester

StrncpyFunctionTest.o: StrncpyFunctionTest.cpp StrncpyFunctionTest.hpp ITest.hpp

tester: ${OBJS}
	${CXX} ${LDFLAGS} -o tester ${OBJS} ${LDLIBS}

clean:
	rm -f ${OBJS} tester
