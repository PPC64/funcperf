#include "MemcpyTest.hpp"

#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <sstream>

namespace funcperf {
namespace string {

MemcpyTest::MemcpyTest(const MemcpyTestParams& testParams)
: m_testParams(testParams)
{
	m_bufferSize = std::max(m_testParams.getSrcOffset(), m_testParams.getDstOffset()) + m_testParams.getBytesToCopy() + sizeof(long);
	m_buffer = (char *)malloc(m_bufferSize);
	m_verifyBuffer = (char *)malloc(m_bufferSize);

	long* longPtr = (long*)m_buffer;
	for (int i = 0; i < m_bufferSize / sizeof(long); i++) {
		//longPtr[i] = random();
		longPtr[i] = i % 0xfe;
	}

	// prepare verification buffer
	memcpy(m_verifyBuffer, m_buffer, m_bufferSize);
	memcpy(m_verifyBuffer + m_testParams.getDstOffset(), m_verifyBuffer + m_testParams.getSrcOffset(), m_testParams.getBytesToCopy());
}

MemcpyTest::~MemcpyTest()
{
	free(m_buffer);
	free(m_verifyBuffer);
}

std::string MemcpyTest::getId()
{
	std::stringstream ss;

	ss << "MEMCPY_" << m_testParams.getCSVValues("_");

	return ss.str();
}

void MemcpyTest::run(void* func)
{
	void* (*memcpy_func)(void* dst, const void* src, size_t len) = (void* (*)(void*, const void*, size_t))func;

	(*memcpy_func)(m_buffer + m_testParams.getDstOffset(), m_buffer + m_testParams.getSrcOffset(), m_testParams.getBytesToCopy());
}

bool MemcpyTest::verify()
{
	return memcmp(m_buffer, m_verifyBuffer, m_bufferSize) == 0;
}

}
}
