#pragma once

#include "ITest.hpp"

#include "MemcpyTestParams.hpp"

#include <cstddef>

namespace funcperf {
namespace string {

class MemcpyTest : public funcperf::ITest
{
public:
	MemcpyTest(const MemcpyTestParams& testParams);
	virtual ~MemcpyTest();

	std::string getId();
	void run(void* func);
	bool verify();

private:
	MemcpyTestParams m_testParams;

	char* m_buffer;
	char* m_verifyBuffer;
	int m_bufferSize;
};

}
}
