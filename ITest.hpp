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

	virtual void runC()
	{
		throw std::logic_error("Unimplemented!");
	}

	virtual bool verify() = 0;

	virtual std::string id() const
	{
		throw std::logic_error("Unimplemented!");
	}

	virtual std::string values() const
	{
		throw std::logic_error("Unimplemented!");
	}


	virtual int iterations(TestLength length) const
	{
		throw std::logic_error("Unimplemented!");
	}

	const std::string& aggrId() const
	{
		return _aggrId;
	}

	bool flush() const
	{
		return _flush;
	}

protected:
	std::string _aggrId;
	bool _flush = false;
};

}
