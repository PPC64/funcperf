#include "StrncpyFunctionTest.hpp"

#include <cstring>
#include <sstream>

namespace funcperf {
namespace string {

class StrncpyTest : public ITest
{
public:
	StrncpyTest(int b, int s, int d);

	std::string id() const override
	{
		return "STRNCPY_" + values("_");
	}

	std::string values(const char* sep = "\t") const override
	{
		std::ostringstream ss;
		ss << bytesToCopy << sep << srcOffset << sep << dstOffset;
		return ss.str();
	}

	int iterations(TestLength length) const override;

	std::string getId() override
	{
		return id();
	}

	void run(void* func) override;
	bool verify() override;

private:
	int bytesToCopy;
	int srcOffset;
	int dstOffset;

	std::unique_ptr<char[]> m_srcBuffer;
	std::unique_ptr<char[]> m_dstBuffer;
	std::unique_ptr<char[]> m_verifyBuffer;
};

static void initBuffer(std::unique_ptr<char[]> &buf, int sz, bool zero = false)
{
	buf.reset(new char[sz]);
	assert((uintptr_t)buf.get() % 8 == 0);
	if (zero)
		memset(buf.get(), 0, sz);
}

StrncpyTest::StrncpyTest(int b, int s, int d) :
	bytesToCopy(b),
	srcOffset(s),
	dstOffset(d)
{
	int bufsz = bytesToCopy + 8;

	initBuffer(m_srcBuffer, bufsz);
	initBuffer(m_dstBuffer, bufsz, true);
	initBuffer(m_verifyBuffer, bufsz, true);

	// pattern fill src buffer
	int i;
	for (i = 0; i < bufsz - 1; i++) {
		char c = i % 0xff;
		if (c == 0)
			c++;
		m_srcBuffer[i] = c;
	}
	m_srcBuffer[i] = 0;

	// prepare verification buffer
	strncpy(m_verifyBuffer.get() + dstOffset,
		m_srcBuffer.get() + srcOffset,
		bytesToCopy);
}


void StrncpyTest::run(void* func)
{
	typedef char* (*strncpy_func_t)(char* dst, const char* src, size_t len);

	strncpy_func_t strncpy_func = strncpy_func_t(func);

	(*strncpy_func)(m_dstBuffer.get() + dstOffset,
			m_srcBuffer.get() + srcOffset,
			bytesToCopy);
}


bool StrncpyTest::verify()
{
	return memcmp(m_dstBuffer.get(), m_verifyBuffer.get(),
		bytesToCopy + 8) == 0;
}


int StrncpyTest::iterations(TestLength length) const
{
	switch (length) {
	case TestLength::shortTest:
		return 1;

	case TestLength::normalTest:
		if (bytesToCopy < 256)
			return 2000;
		else if (bytesToCopy < 1024*1024)
			return 2000;
		else
			return 100;

	case TestLength::longTest:
		if (bytesToCopy < 256)
			return 200000;
		else if (bytesToCopy < 1024*1024)
			return 20000;
		else
			return 1000;
	}
}


ITest* StrncpyFunctionTest::nextTest()
{
	if (last)
		return nullptr;

	test.reset(new StrncpyTest(bytesToCopy, srcOffset, dstOffset));

	if (++dstOffset == 8) {
		dstOffset = 0;
		if (++srcOffset == 8) {
			srcOffset = 0;
			bytesToCopy += 1024;
			if (bytesToCopy > 8*1024*1024)
				last = true;
		}
	}

	return test.get();
}

}
}
