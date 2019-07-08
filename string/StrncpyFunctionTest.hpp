#pragma once

#include "IFunctionTest.hpp"

#include <vector>
#include <memory>

namespace funcperf {
namespace string {

class StrncpyFunctionTest : public funcperf::IFunctionTest
{
public:
	std::string getFunctionName() override;
	std::vector<std::shared_ptr<funcperf::ITestParams>> getTestsParams() override;
	std::shared_ptr<funcperf::ITest> getTest(const funcperf::ITestParams& testParams) override;
};

}
}
