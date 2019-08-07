#include "MemcpyFunctionTest.hpp"

#include "Util.hpp"

#include <cstring>
#include <sstream>

namespace funcperf {
namespace string {

class MemcpyTest : public ITest
{
public:
	MemcpyTest(const std::string& sep, int s, int d, int n);

	std::string id() const override
	{
		return "MEMCPY_" + values() + "_" + aggrId() +
			(n < 10000 && sep == "\t"? sep : "");
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
	int srcOffset;
	int dstOffset;
	int n;

	std::unique_ptr<char[]> m_srcBuffer;
	std::unique_ptr<char[]> m_dstBuffer;
	std::unique_ptr<char[]> m_verifyBuffer;
};


MemcpyTest::MemcpyTest(const std::string& sep, int s, int d, int n) :
	sep(sep),
	srcOffset(s),
	dstOffset(d),
	n(n)
{
	initBuffer(m_srcBuffer, n + 8);
	initBuffer(m_dstBuffer, n + 8, true);
	initBuffer(m_verifyBuffer, n + 8, true);

	// pattern fill src buffer
	int i;
	for (i = 0; i < srcOffset; i++)
		m_srcBuffer[i] = 0;
	for (; i < srcOffset + n; i++) {
		char c = i % 0x7f + 33;
		m_srcBuffer[i] = c;
	}
	for (; i < n + 8; i++)
		m_srcBuffer[i] = 0;

	// prepare verification buffer
	memcpy(m_verifyBuffer.get() + dstOffset,
		m_srcBuffer.get() + srcOffset,
		n);

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


void MemcpyTest::run(void* func)
{
	typedef void* (*memcpy_func_t)(void* dst, const void* src, size_t len);

	memcpy_func_t memcpy_func = memcpy_func_t(func);

	(*memcpy_func)(m_dstBuffer.get() + dstOffset,
			m_srcBuffer.get() + srcOffset,
			n);
}


void MemcpyTest::runC()
{
	memcpy(m_dstBuffer.get() + dstOffset,
		m_srcBuffer.get() + srcOffset,
		n);
}


bool MemcpyTest::verify()
{
	bool rc = memcmp(m_dstBuffer.get(), m_verifyBuffer.get(),
		n + 8) == 0;

	if (!rc) {
		std::cout << "memcpy(dst+" << dstOffset
			<< ", src+" << srcOffset
			<< ", " << n << ")\n";
		std::cout << "src=[" << m_srcBuffer.get() + srcOffset << "]\n";
		std::cout << "dst=[" << m_dstBuffer.get() + dstOffset << "]\n";
		std::cout << "ver=[" << m_verifyBuffer.get() + dstOffset << "]\n";
	}

	return rc;
}


int MemcpyTest::iterations(TestLength length) const
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


ITest* MemcpyFunctionTest::nextTest()
{
	if (last)
		return nullptr;

	test.reset(new MemcpyTest(sep, srcOffset, dstOffset, n));

	if (++dstOffset == 8) {
		dstOffset = 0;
		if (++srcOffset == 8) {
			srcOffset = 0;
			n *= 2;
			if (n > 1024*1024)
				last = true;
		}
	}

	return test.get();
}

}
}
