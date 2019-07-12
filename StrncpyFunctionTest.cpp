#include "StrncpyFunctionTest.hpp"

#include <cstring>
#include <sstream>

namespace funcperf {
namespace string {

class StrncpyTest : public ITest
{
public:
	StrncpyTest(const std::string& sep, int l, int s, int d, int n);

	std::string id() const override
	{
		return "STRNCPY_" + values() + "_" + aggrId() +
			(n < 1000 && sep == "\t"? sep : "");
	}

	std::string values() const override
	{
		std::ostringstream ss;
		ss << n;
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
	int srcLen;
	int srcOffset;
	int dstOffset;
	int n;

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

StrncpyTest::StrncpyTest(const std::string& sep, int l, int s, int d, int n) :
	sep(sep),
	srcLen(l),
	srcOffset(s),
	dstOffset(d),
	n(n)
{
	initBuffer(m_srcBuffer, srcLen + 8);
	initBuffer(m_dstBuffer, n + 8, true);
	initBuffer(m_verifyBuffer, n + 8, true);

	// pattern fill src buffer
	int i;
	for (i = 0; i < srcOffset; i++)
		m_srcBuffer[i] = 0;
	for (; i < srcLen + 7; i++) {
		char c = i % 0x7f + 33;
		m_srcBuffer[i] = c;
	}
	m_srcBuffer[i] = 0;

	// prepare verification buffer
	strncpy(m_verifyBuffer.get() + dstOffset,
		m_srcBuffer.get() + srcOffset,
		n);

	if (srcLen != n)
		_aggrId = "srcLen != n";
	else if (srcOffset == 0 && dstOffset == 0)
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
			n);
}


void StrncpyTest::runC()
{
	strncpy(m_dstBuffer.get() + dstOffset,
		m_srcBuffer.get() + srcOffset,
		n);
}


bool StrncpyTest::verify()
{
	bool rc = memcmp(m_dstBuffer.get(), m_verifyBuffer.get(),
		n + 8) == 0;

	if (!rc) {
		std::cout << "strncpy(dst+" << dstOffset
			<< ", src+" << srcOffset
			<< ", " << n << ")\n";
		std::cout << "src=[" << m_srcBuffer.get() + srcOffset << "]\n";
		std::cout << "dst=[" << m_dstBuffer.get() + dstOffset << "]\n";
		std::cout << "ver=[" << m_verifyBuffer.get() + dstOffset << "]\n";
	}

	return rc;
}


int StrncpyTest::iterations(TestLength length) const
{
	switch (length) {
	case TestLength::shortTest:
		return 1;

	case TestLength::normalTest:
		if (n < 1024)
			return 2000;
		else if (n < 100*1024)
			return 500;
		else if (n < 1024*1024)
			return 100;
		else
			return 50;

	case TestLength::longTest:
		if (n < 256)
			return 200000;
		else if (n < 1024*1024)
			return 20000;
		else
			return 1000;
	}
}


ITest* StrncpyFunctionTest::nextTest()
{
	if (last)
		return nullptr;

	test.reset(new StrncpyTest(sep, srcLen(), srcOffset, dstOffset, n));

	if (++dstOffset == 8) {
		dstOffset = 0;
		if (++srcOffset == 8) {
			srcOffset = 0;
			if (++srcLenI == 4) {
				srcLenI = 0;
				n *= 2;
				if (n > 1024*1024)
					last = true;
			}
		}
	}

	return test.get();
}

}
}
