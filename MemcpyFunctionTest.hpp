#pragma once

#include "IFunctionTest.hpp"

#include <vector>
#include <memory>

namespace funcperf {
namespace string {

class MemcpyFunctionTest : public funcperf::IFunctionTest
{
public:
	std::string getFunctionName();
	std::vector<std::shared_ptr<funcperf::ITestParams>> getTestsParams();
	std::shared_ptr<funcperf::ITest> getTest(const funcperf::ITestParams& testParams);
};

}
}
