#include "MemcpyFunctionTest.hpp"

#include "MemcpyTest.hpp"
#include "MemcpyTestParams.hpp"

namespace funcperf {
namespace string {

std::string MemcpyFunctionTest::getFunctionName()
{
	return "memcpy";
}

std::vector<std::shared_ptr<funcperf::ITestParams>> MemcpyFunctionTest::getTestsParams()
{
	std::vector<std::shared_ptr<funcperf::ITestParams>> result;

	for (int bytesToCopy = 1; bytesToCopy <= 8*1024*1024; bytesToCopy *= 2) {
		int srcOffset = 0;
		for (int srcOffsetIdx = 0; srcOffsetIdx < 8; srcOffsetIdx++) {
			int dstOffset = 0;
			for (int dstOffsetIdx = 0; dstOffsetIdx < 8; dstOffsetIdx++) {
				// overlapping test
				result.push_back(std::make_shared<MemcpyTestParams>(bytesToCopy, srcOffset, dstOffset));

				// non-overlapping test
				result.push_back(std::make_shared<MemcpyTestParams>(bytesToCopy, srcOffset, 8*1024*1024 + dstOffset));
				result.push_back(std::make_shared<MemcpyTestParams>(bytesToCopy, 8*1024*1024 + srcOffset, dstOffset));

				if (dstOffset > 0) {
					dstOffset *= 2;
				} else {
					dstOffset = 1;
				}
			}

			if (srcOffset > 0) {
				srcOffset *= 2;
			} else {
				srcOffset = 1;
			}
		}
	}

	return result;
}

std::shared_ptr<funcperf::ITest> MemcpyFunctionTest::getTest(const funcperf::ITestParams& testParams)
{
	return std::make_shared<MemcpyTest>(dynamic_cast<const MemcpyTestParams&>(testParams));
}

}
}
