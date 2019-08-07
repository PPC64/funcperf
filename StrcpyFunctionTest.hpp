#pragma once

#include "IFunctionTest.hpp"

#include <vector>
#include <memory>

namespace funcperf {
namespace string {

class StrcpyFunctionTest : public IFunctionTest
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

	std::unique_ptr<ITest> test;
	bool last = false;
};

}
}
