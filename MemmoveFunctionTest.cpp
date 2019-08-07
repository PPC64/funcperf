#include "IFunctionTest.hpp"

#include "Util.hpp"

#include <cstring>
#include <memory>
#include <sstream>

namespace funcperf {

class MemmoveFunctionTest : public IFunctionTest
{
public:
	MemmoveFunctionTest(bool bcopy) :
		_bcopy(bcopy)
	{}

	std::string headers() const override
	{
		return "n";
	}

	ITest* nextTest() override;

private:
	int srcOffset = 0;
	int dstOffset = 0;
	int n = 2;
	int overlap = -4;

	std::unique_ptr<ITest> test;
	bool last = false;
	bool _bcopy;
};


class MemmoveTest : public ITest
{
public:
	MemmoveTest(const std::string& sep, int s, int d, int n, int o, bool bcopy);

	std::string id() const override
	{
		return "MEMMOVE_" + values() + "_" + aggrId() +
			(n < 1000 && sep == "\t"? sep : "");
	}

	std::string values() const override
	{
		std::ostringstream ss;
		ss << n;
		return ss.str();
	}

	int iterations(TestLength length) const override;

	void run(void* func) override;
	void runC() override;
	bool verify() override;

private:
	std::string sep;
	int srcOffset;
	int dstOffset;
	int n;
	int overlap;
	bool _bcopy;
	const int skip = 8;
	const int extra = 24;

	std::unique_ptr<char[]> m_sbuf;
	std::unique_ptr<char[]> m_dbuf;

	char* m_srcBuffer;
	char* m_dstBuffer;
	std::unique_ptr<char[]> m_verifyBuffer;
};


MemmoveTest::MemmoveTest(const std::string& sep, int s, int d, int n, int o, bool b) :
	sep(sep),
	srcOffset(s),
	dstOffset(d),
	n(n),
	overlap(o),
	_bcopy(b)
{
	// set src buffer
	initBuffer(m_sbuf, n + extra);
	int i;
	for (i = 0; i < skip + srcOffset; i++)
		m_sbuf[i] = 0;
	for (; i < skip + srcOffset + n; i++) {
		char c = i % 0x7f + 33;
		m_sbuf[i] = c;
	}
	for (; i < n + extra; i++)
		m_sbuf[i] = 0;
	m_srcBuffer = m_sbuf.get() + skip;

	// set dst buffer
	if (overlap)
		m_dstBuffer = m_srcBuffer + overlap;
	else {
		initBuffer(m_dbuf, n + extra, true);
		m_dstBuffer = m_dbuf.get();
	}

	// prepare verification buffer
	initBuffer(m_verifyBuffer, n + extra, true);
	if (!overlap) {
		if (_bcopy)
			bcopy(m_srcBuffer + srcOffset,
				m_verifyBuffer.get() + dstOffset,
				n);
		else
			memmove(m_verifyBuffer.get() + dstOffset,
				m_srcBuffer + srcOffset,
				n);
	} else {
		initBuffer(m_dbuf, n + extra);
		memcpy(m_dbuf.get(), m_sbuf.get(), n + extra);
		if (_bcopy)
			bcopy(m_dbuf.get() + skip + srcOffset,
				m_verifyBuffer.get() + dstOffset,
				n);
		else
			memmove(m_verifyBuffer.get() + dstOffset,
				m_dbuf.get() + skip + srcOffset,
				n);
	}

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


void MemmoveTest::run(void* func)
{
	typedef void* (*memmove_func_t)(void* dst, const void* src, size_t len);
	typedef void (*bcopy_func_t)(const void* src, void* dst, size_t len);

	if (_bcopy) {
		bcopy_func_t bcopy_func = bcopy_func_t(func);
		(*bcopy_func)(m_srcBuffer + srcOffset,
				m_dstBuffer + dstOffset,
				n);
	} else {
		memmove_func_t memmove_func = memmove_func_t(func);
		(*memmove_func)(m_dstBuffer + dstOffset,
				m_srcBuffer + srcOffset,
				n);
	}
}


void MemmoveTest::runC()
{
	if (_bcopy)
		bcopy(m_srcBuffer + srcOffset,
			m_dstBuffer + dstOffset,
			n);
	else
		memmove(m_dstBuffer + dstOffset,
			m_srcBuffer + srcOffset,
			n);
}


bool MemmoveTest::verify()
{
	bool rc = memcmp(m_dstBuffer + dstOffset,
			m_verifyBuffer.get() + dstOffset,
			n) == 0;

	if (!rc) {
		if (_bcopy)
			std::cout << "bcopy(src+" << srcOffset
				<< ", dst+" << dstOffset
				<< ", " << n << ")\n";
		else
			std::cout << "memmove(dst+" << dstOffset
				<< ", src+" << srcOffset
				<< ", " << n << ")\n";
		std::cout << "overlap=" << overlap << std::endl;
		std::cout << "src=[" << m_srcBuffer + srcOffset << "]\n";
		std::cout << "dst=[" << m_dstBuffer + dstOffset << "]\n";
		std::cout << "ver=[" << m_verifyBuffer.get() + dstOffset << "]\n";
	}

	return rc;
}


int MemmoveTest::iterations(TestLength length) const
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


ITest* MemmoveFunctionTest::nextTest()
{
	if (last)
		return nullptr;

	test.reset(new MemmoveTest(sep, srcOffset, dstOffset, n, overlap, _bcopy));

	if (++dstOffset == 8) {
		dstOffset = 0;
		if (++srcOffset == 8) {
			srcOffset = 0;
			if (++overlap == 4) {
				overlap = -4;
				n *= 2;
				if (n > 1024*1024)
					last = true;
			}
		}
	}

	return test.get();
}

static IFunctionTest* newMemmoveFunctionTest()
{
	return new MemmoveFunctionTest(false);
}

static IFunctionTest* newBcopyFunctionTest()
{
	return new MemmoveFunctionTest(true);
}

static int dummy1 = (FunctionTestFactory::instance()
	.registerFunction("memmove", newMemmoveFunctionTest), 1);

static int dummy2 = (FunctionTestFactory::instance()
	.registerFunction("bcopy", newBcopyFunctionTest), 1);

}
