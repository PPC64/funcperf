CXXFLAGS+=-std=c++14 -I${.CURDIR} -Wall -Werror -g

SRCS=Tester.cpp \
	MemcpyFunctionTest.cpp \
	MemmoveFunctionTest.cpp \
	StrcpyFunctionTest.cpp \
	StrncpyFunctionTest.cpp

OBJS:=${SRCS:S/.cpp/.o/}

all: tester

MemcpyFunctionTest.o: MemcpyFunctionTest.cpp \
	IFunctionTest.hpp ITest.hpp Util.hpp
MemmoveFunctionTest.o: MemmoveFunctionTest.cpp \
	IFunctionTest.hpp ITest.hpp Util.hpp
StrcpyFunctionTest.o: StrcpyFunctionTest.cpp \
	IFunctionTest.hpp ITest.hpp Util.hpp
StrncpyFunctionTest.o: StrncpyFunctionTest.cpp \
	IFunctionTest.hpp ITest.hpp Util.hpp

tester: ${OBJS}
	${CXX} ${LDFLAGS} -o tester ${OBJS} ${LDLIBS}

clean:
	rm -f ${OBJS} tester
