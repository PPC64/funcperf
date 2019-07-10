#pragma once

#include "ITestParams.hpp"

#include <string>

namespace funcperf {
namespace string {

class StrcmpTestParams : public funcperf::ITestParams
{
public:
	StrcmpTestParams(int strALen, int strBLen, int expectedResult, int misalignment);
	int getStrALen();
	int getStrBLen();
	int getExpectedResult();
	int getMisalignment();

	std::string getCSVHeaders(const std::string& sep);
	std::string getCSVValues(const std::string& sep);
	int getIterations(TestLength length);

private:
	int m_strALen;
	int m_strBLen;
	int m_expectedResult;
	int m_misalignment;
};

}
}
