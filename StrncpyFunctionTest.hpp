#pragma once

#include "IFunctionTest.hpp"

#include <memory>
#include <vector>

namespace funcperf {
namespace string {

class StrncpyFunctionTest : public IFunctionTest
{
public:
	StrncpyFunctionTest() :
		IFunctionTest(false)
	{}

	std::string getFunctionName() override
	{
		unimplemented();
	}

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
		return "bytesToCopy\tsrcOffset\tdstOffset";
	}

	ITest* nextTest() override;

private:
	int bytesToCopy = 2;
	int srcOffset = 0;
	int dstOffset = 0;

	std::unique_ptr<ITest> test;
	bool last = false;
};

}
}
