#pragma once

#include "IFunctionTest.hpp"

#include <memory>
#include <vector>

namespace funcperf {
namespace string {

class StrncpyFunctionTest : public IFunctionTest
{
public:
	std::string headers() const override
	{
		return "n";
	}

	ITest* nextTest() override;

private:
	int srcOffset = 0;
	int dstOffset = 0;
	int n = 2;
	int srcLenI = 0;

	std::unique_ptr<ITest> test;
	bool last = false;


	int srcLen() const
	{
		return n / (4 -srcLenI);
	}
};

}
}
