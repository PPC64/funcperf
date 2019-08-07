#pragma once

#include "IFunctionTest.hpp"

#include <vector>
#include <memory>

namespace funcperf {
namespace string {

class StrcpyFunctionTest : public IFunctionTest
{
public:
	StrcpyFunctionTest() :
		IFunctionTest(false)
	{}

	std::vector<std::shared_ptr<ITestParams>> getTestsParams() override
	{
		unimplemented();
	}

	std::shared_ptr<ITest> getTest(const ITestParams& testParams) override
	{
		unimplemented();
	}

	std::string headers() override
	{
		return "n";
	}

	ITest* nextTest() override;

private:
	int srcOffset = 0;
	int dstOffset = 0;
	int n = 2;

	std::unique_ptr<ITest> test;
	bool last = false;
};

}
}
