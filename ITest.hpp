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

	virtual void run(void* func) = 0;
	virtual void runC() = 0;
	virtual bool verify() = 0;

	virtual std::string id() const = 0;
	virtual std::string values() const = 0;
	virtual int iterations(TestLength length) const = 0;

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
