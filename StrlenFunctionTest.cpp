#include "IFunctionTest.hpp"

#include "Util.hpp"

#include <cstring>
#include <memory>
#include <sstream>

namespace funcperf {

class StrlenFunctionTest : public IFunctionTest
{
public:
	std::string headers() const override
	{
		return "n";
	}

	ITest* nextTest() override;

private:
	int srcOffset = 0;
	int dstOffset = 0;
	int n = 2;

	std::unique_ptr<ITest> test;
	bool last = false;
};


class StrlenTest : public ITest
{
public:
	StrlenTest(const std::string& sep, int s, int d, int n);

	std::string id() const override
	{
		return "STRLEN_" + values() + "_" + aggrId() +
			(n < 10000 && sep == "\t"? sep : "");
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
	int calc_sz;

	std::unique_ptr<char[]> m_srcBuffer;
};


StrlenTest::StrlenTest(const std::string& sep, int s, int d, int n) :
	sep(sep),
	srcOffset(s),
	dstOffset(d),
	n(n),
	calc_sz(0)
{
	initBuffer(m_srcBuffer, n + 8);

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


	if (srcOffset == 0)
		_aggrId = "aligned    ";
	else
		_aggrId = "misaligned ";

	if (srcOffset == 7 && dstOffset == 7)
		_flush = true;
}


void StrlenTest::run(void* func)
{
	typedef int (*strlen_func_t)(const char* s);

	strlen_func_t strlen_func = strlen_func_t(func);

	calc_sz = (*strlen_func)(m_srcBuffer.get() + srcOffset);
}


void StrlenTest::runC()
{
	int _;
	_ = strlen(m_srcBuffer.get() + srcOffset);
}


bool StrlenTest::verify()
{
	bool rc = n == calc_sz;

	if (!rc) {
		std::cout << "strlen(dst+" << dstOffset
			<< ", " << n << ")\n";
		std::cout << "src=[" << m_srcBuffer.get() + srcOffset << "]\n";
		std::cout << "calculated len: " << calc_sz << "\n";
	}

	return rc;
}


int StrlenTest::iterations(TestLength length) const
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


ITest* StrlenFunctionTest::nextTest()
{
	if (last)
		return nullptr;

	test.reset(new StrlenTest(sep, srcOffset, dstOffset, n));

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

static IFunctionTest* newStrlenFunctionTest()
{
	return new StrlenFunctionTest;
}

static int dummy = (FunctionTestFactory::instance()
	.registerFunction("strlen", newStrlenFunctionTest), 1);

}
