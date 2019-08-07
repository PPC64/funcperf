#pragma once

#include "ITestParams.hpp"
#include "ITest.hpp"

#include <iostream>
#include <vector>
#include <memory>

namespace funcperf {

class IFunctionTest
{
public:
	IFunctionTest(bool c = true) :
		compat(c)
	{}

	virtual ~IFunctionTest() = default;

	virtual std::vector<std::shared_ptr<ITestParams>> getTestsParams() = 0;
	virtual std::shared_ptr<ITest> getTest(const ITestParams& testParams) = 0;

	void setLength(TestLength len)
	{
		this->len = len;
	}

	virtual std::string headers()
	{
		assertNotCompat("headers");
		unimplemented();
	}

	virtual ITest* nextTest()
	{
		assertNotCompat("nextTest");
		unimplemented();
	}

	bool compat;
	std::string sep;
protected:
	TestLength len;

	void assertNotCompat(const std::string& id)
	{
		if (compat)
			throw std::logic_error(id + ": !compat!");
	}

	[[noreturn]] void unimplemented()
	{
		throw std::logic_error("unimplemented!");
	}
};

}
