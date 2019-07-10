#pragma once

#include "ITestParams.hpp"

#include <string>

namespace funcperf {

class ITest
{
public:
	virtual ~ITest() {}
	virtual std::string getId() = 0;
	virtual void run(void* func) = 0;
	virtual bool verify() = 0;

	virtual std::string id() const
	{
		throw std::logic_error("Unimplemented!");
	}

	virtual std::string values(const char* sep = "\t") const
	{
		throw std::logic_error("Unimplemented!");
	}


	virtual int iterations(TestLength length) const
	{
		throw std::logic_error("Unimplemented!");
	}

};

}
