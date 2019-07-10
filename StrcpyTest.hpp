#pragma once

#include "ITest.hpp"

#include "MemcpyTestParams.hpp"

#include <cstddef>

namespace funcperf {
namespace string {

class StrcpyTest : public funcperf::ITest
{
public:
	StrcpyTest(const MemcpyTestParams& testParams);
	virtual ~StrcpyTest();

	std::string getId();
	void run(void* func);
	bool verify();

private:
	MemcpyTestParams m_testParams;

	char* m_srcBuffer;
	char* m_dstBuffer;
	char* m_verifyBuffer;
};

}
}
