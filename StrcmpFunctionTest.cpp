#include "StrcmpFunctionTest.hpp"

#include "StrcmpTest.hpp"
#include "StrcmpTestParams.hpp"

namespace funcperf {
namespace string {

std::string StrcmpFunctionTest::getFunctionName()
{
	return "strcmp";
}

std::vector<std::shared_ptr<funcperf::ITestParams>> StrcmpFunctionTest::getTestsParams()
{
	std::vector<std::shared_ptr<funcperf::ITestParams>> result;

	for (int strALen = 1; strALen <= 1024; strALen *= 2) {
		for (int strBLen = 1; strBLen <= 1024; strBLen *= 2) {
			for (int expectedResult = -1; expectedResult <= 1; expectedResult++) {
				for (int misalignment = 0; misalignment < 8; misalignment++) {
					result.push_back(std::make_shared<StrcmpTestParams>(strALen, strBLen, expectedResult, misalignment));
				}
			}
		}
	}

	return result;
}

std::shared_ptr<funcperf::ITest> StrcmpFunctionTest::getTest(const funcperf::ITestParams& testParams)
{
	return std::make_shared<StrcmpTest>(dynamic_cast<const StrcmpTestParams&>(testParams));
}

}
}
