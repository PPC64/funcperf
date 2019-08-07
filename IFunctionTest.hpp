#pragma once

#include "ITest.hpp"

#include <iostream>
#include <vector>
#include <memory>

namespace funcperf {

class IFunctionTest
{
public:
	virtual ~IFunctionTest() = default;

	void setLength(TestLength len)
	{
		this->len = len;
	}

	virtual std::string headers() const = 0;
	virtual ITest* nextTest() = 0;

	std::string sep;
protected:
	TestLength len;
};

}
