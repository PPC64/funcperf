#include "StrcmpTest.hpp"

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <algorithm>

namespace funcperf {
namespace string {

StrcmpTest::StrcmpTest(const StrcmpTestParams& testParams)
: m_testParams(testParams)
{
	m_strABuffer = (char *)malloc(m_testParams.getStrALen() + m_testParams.getMisalignment() + 1);
	m_strBBuffer = (char *)malloc(m_testParams.getStrBLen() + m_testParams.getMisalignment() + 1);

	char strA, strB;
	int maxStrLen = std::max(m_testParams.getStrALen(), m_testParams.getStrBLen());
	int i;

	if (m_testParams.getExpectedResult() == 0) {
		strA = 'a';
		strB = 'a';
	} else if (m_testParams.getExpectedResult() < 0) {
		strA = 'a';
		strB = 'b';
	} else {
		strA = 'b';
		strB = 'a';
	}

	// Initialize buffer.
	for (i = 0; i < maxStrLen; i++) {
		if (i < m_testParams.getStrALen()) {
			m_strABuffer[i + m_testParams.getMisalignment()] = strA;
		}

		if (i < m_testParams.getStrBLen()) {
			m_strBBuffer[i + m_testParams.getMisalignment()] = strA;
		}
	}

	m_strABuffer[m_testParams.getStrALen() + m_testParams.getMisalignment() - 1] = strA;
	m_strBBuffer[m_testParams.getStrBLen() + m_testParams.getMisalignment() - 1] = strB;
	m_strABuffer[m_testParams.getStrALen() + m_testParams.getMisalignment()] = '\0';
	m_strBBuffer[m_testParams.getStrBLen() + m_testParams.getMisalignment()] = '\0';
}

StrcmpTest::~StrcmpTest()
{
	free(m_strABuffer);
	free(m_strBBuffer);
}

std::string StrcmpTest::getId()
{
	std::stringstream ss;

	ss << "STRCMP_" << m_testParams.getCSVValues("_");

	return ss.str();
}

void StrcmpTest::run(void* func)
{
	int (*strcmp_func)(const char* s1, const char* s2) = (int (*)(const char*, const char*))func;

	m_result = (*strcmp_func)(m_strABuffer + m_testParams.getMisalignment(), m_strBBuffer + m_testParams.getMisalignment());
}

bool StrcmpTest::verify()
{
	int verifyResult = strcmp(m_strABuffer + m_testParams.getMisalignment(), m_strBBuffer + m_testParams.getMisalignment());

	if ((m_result < 0 && verifyResult < 0) ||
	    (m_result > 0 && verifyResult > 0) ||
	    (m_result == 0 && verifyResult == 0)) {

		return true;
	}
		
	return false;
}

}
}
