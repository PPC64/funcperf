#pragma once

#include <string>

namespace funcperf {

enum class TestLength {
	shortTest,
	normalTest,
	longTest,
};

class ITestParams
{
public:
	ITestParams(const std::string& h, const std::string& v, int i) :
		headers(h),
		values(v),
		iters(i)
	{}

	ITestParams() = default;

	virtual ~ITestParams() = default;

	virtual std::string getCSVHeaders(const std::string& sep)
	{
		return headers;
	}

	virtual std::string getCSVValues(const std::string& sep)
	{
		return values;
	}

	virtual int getIterations(TestLength length)
	{
		return iters;
	}

private:
	std::string headers;
	std::string values;
	int iters;
};

}
