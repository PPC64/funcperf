#include "StrcmpTestParams.hpp"

#include <sstream>
#include <algorithm>

namespace funcperf {
namespace string {

StrcmpTestParams::StrcmpTestParams(int strALen, int strBLen, int expectedResult, int misalignment)
: m_strALen(strALen)
, m_strBLen(strBLen)
, m_expectedResult(expectedResult)
, m_misalignment(misalignment)
{
}

int StrcmpTestParams::getStrALen()
{
	return m_strALen;
}

int StrcmpTestParams::getStrBLen()
{
	return m_strBLen;
}

int StrcmpTestParams::getExpectedResult()
{
	return m_expectedResult;
}

int StrcmpTestParams::getMisalignment()
{
	return m_misalignment;
}

std::string StrcmpTestParams::getCSVHeaders(const std::string& sep)
{
	std::stringstream ss;

	ss << "strALen" << sep << "strBLen" << sep << "expectedResult" << sep << "misalignment";

	return ss.str();
}

std::string StrcmpTestParams::getCSVValues(const std::string& sep)
{
	std::stringstream ss;

	ss << m_strALen << sep << m_strBLen << sep << m_expectedResult << sep << m_misalignment;

	return ss.str();
}

int StrcmpTestParams::getIterations(TestLength length)
{
	int iterations;

	int maxLen = std::max(m_strALen, m_strBLen);

	switch (length) {
	case TestLength::shortTest:
		iterations = 1;
		break;

	case TestLength::normalTest:
		if (maxLen < 256) {
			iterations = 2000;
		} else if (maxLen < 1024*1024) {
			iterations = 2000;
		} else {
			iterations = 100;
		}
		break;

	case TestLength::longTest:
		if (maxLen < 256) {
			iterations = 200000;
		} else if (maxLen < 1024*1024) {
			iterations = 20000;
		} else {
			iterations = 1000;
		}
		break;
	}

	return iterations;
}

}
}
