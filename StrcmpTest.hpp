#pragma once

#include "ITest.hpp"

#include "StrcmpTestParams.hpp"

#include <cstddef>

namespace funcperf {
namespace string {

class StrcmpTest : public funcperf::ITest
{
public:
	StrcmpTest(const StrcmpTestParams& testParams);
	virtual ~StrcmpTest();

	std::string getId();
	void run(void* func);
	bool verify();

private:
	StrcmpTestParams m_testParams;

	char* m_strABuffer;
	char* m_strBBuffer;
	int m_result;
};

}
}
