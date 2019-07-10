#pragma once

#include "IFunctionTest.hpp"

#include <vector>
#include <memory>

namespace funcperf {
namespace string {

class StrcmpFunctionTest : public funcperf::IFunctionTest
{
public:
	std::vector<std::shared_ptr<funcperf::ITestParams>> getTestsParams();
	std::shared_ptr<funcperf::ITest> getTest(const funcperf::ITestParams& testParams);
};

}
}
