#include "StrncpyFunctionTest.hpp"

#include <cstring>
#include <sstream>

namespace funcperf {
namespace string {

class StrncpyTest : public ITest
{
public:
	StrncpyTest(const std::string& sep, int b, int s, int d);

	std::string id() const override
	{
		return "STRNCPY_" + values() + "_" + aggrId() +
			(bytesToCopy < 1000 && sep == "\t"? sep : "");
	}

	std::string values() const override
	{
		std::ostringstream ss;
		ss << bytesToCopy;
		return ss.str();
	}

	int iterations(TestLength length) const override;

	std::string getId() override
	{
		return id();
	}

	void run(void* func) override;
	void runC() override;
	bool verify() override;

private:
	std::string sep;
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

StrncpyTest::StrncpyTest(const std::string& sep, int b, int s, int d) :
	sep(sep),
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

	if (srcOffset == 0 && dstOffset == 0)
		_aggrId = "aligned    ";
	else if (srcOffset == 0)
		_aggrId = "src_aligned";
	else if (dstOffset == 0)
		_aggrId = "dst_aligned";
	else
		_aggrId = "misaligned ";

	if (srcOffset == 7 && dstOffset == 7)
		_flush = true;
}


void StrncpyTest::run(void* func)
{
	typedef char* (*strncpy_func_t)(char* dst, const char* src, size_t len);

	strncpy_func_t strncpy_func = strncpy_func_t(func);

	(*strncpy_func)(m_dstBuffer.get() + dstOffset,
			m_srcBuffer.get() + srcOffset,
			bytesToCopy);
}


void StrncpyTest::runC()
{
	strncpy(m_dstBuffer.get() + dstOffset,
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
		if (bytesToCopy < 1024)
			return 2000;
		else if (bytesToCopy < 100*1024)
			return 500;
		else if (bytesToCopy < 1024*1024)
			return 100;
		else
			return 50;

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

	test.reset(new StrncpyTest(sep, bytesToCopy, srcOffset, dstOffset));

	if (++dstOffset == 8) {
		dstOffset = 0;
		if (++srcOffset == 8) {
			srcOffset = 0;
			bytesToCopy *= 2;
			if (bytesToCopy > 4*1024*1024)
				last = true;
		}
	}

	return test.get();
}

}
}
