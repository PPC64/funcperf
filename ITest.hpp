#pragma once

#include <string>

namespace funcperf {

enum class TestLength {
	shortTest,
	normalTest,
	longTest
};

class ITest
{
public:
	virtual ~ITest() {}
	virtual std::string getId() = 0;
	virtual void run(void* func) = 0;

	virtual void runC()
	{
		throw std::logic_error("runC: Unimplemented!");
	}

	virtual bool verify() = 0;

	virtual std::string id() const
	{
		throw std::logic_error("id: Unimplemented!");
	}

	virtual std::string values() const
	{
		throw std::logic_error("values: Unimplemented!");
	}


	virtual int iterations(TestLength length) const
	{
		throw std::logic_error("iterations: Unimplemented!");
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
