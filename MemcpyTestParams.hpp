#pragma once

#include "ITestParams.hpp"

#include <string>

namespace funcperf {
namespace string {

class MemcpyTestParams : public funcperf::ITestParams
{
public:
	MemcpyTestParams(int bytesToCopy, int srcOffset, int dstOffset);
	int getBytesToCopy();
	int getSrcOffset();
	int getDstOffset();

	std::string getCSVHeaders(const std::string& sep);
	std::string getCSVValues(const std::string& sep);
	int getIterations(TestLength length);

private:
	int m_bytesToCopy;
	int m_srcOffset;
	int m_dstOffset;
};

}
}
