CXXFLAGS+=-std=c++14 -I${.CURDIR} -Wall -Werror -g

SRCS=Tester.cpp \
	MemcpyFunctionTest.cpp MemcpyTestParams.cpp MemcpyTest.cpp \
	StrcpyFunctionTest.cpp \
	StrncpyFunctionTest.cpp \
	StrcmpFunctionTest.cpp StrcmpTestParams.cpp StrcmpTest.cpp

OBJS:=${SRCS:S/.cpp/.o/}

all: tester

StrcpyFunctionTest.o: StrcpyFunctionTest.cpp StrcpyFunctionTest.hpp ITest.hpp Util.hpp
StrncpyFunctionTest.o: StrncpyFunctionTest.cpp StrncpyFunctionTest.hpp ITest.hpp Util.hpp

tester: ${OBJS}
	${CXX} ${LDFLAGS} -o tester ${OBJS} ${LDLIBS}

clean:
	rm -f ${OBJS} tester
