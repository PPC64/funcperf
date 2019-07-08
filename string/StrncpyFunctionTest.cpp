#include "StrncpyFunctionTest.hpp"

#include <cstring>
#include <sstream>

namespace funcperf {
namespace string {

static int iters(TestLength length, int bytesToCopy)
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

namespace {

class Params : public ITestParams
{
public:
	Params(const std::string& h, const std::string& v, int i, int b, int s, int d) :
		ITestParams(h, v, i),
		bytesToCopy(b),
		srcOffset(s),
		dstOffset(d)
	{}

	int bytesToCopy;
	int srcOffset;
	int dstOffset;
};

}

std::string StrncpyFunctionTest::getFunctionName()
{
	return "strncpy";
}

std::vector<std::shared_ptr<ITestParams>> StrncpyFunctionTest::getTestsParams()
{
	std::vector<std::shared_ptr<ITestParams>> result;
	std::string headers = "bytesToCopy\tsrcOffset\tdstOffset";
	const char *sep = "\t";

	for (int bytesToCopy = 2; bytesToCopy <= 8*1024*1024; bytesToCopy *= 2)
		for (int srcOffset = 0; srcOffset < 8; srcOffset++)
			for (int dstOffset = 0; dstOffset < 8; dstOffset++) {
				std::ostringstream ss;
				ss << bytesToCopy << sep << srcOffset << sep << dstOffset;

				result.push_back(std::make_shared<Params>(
					headers, ss.str(), iters(len, bytesToCopy),
					bytesToCopy, srcOffset, dstOffset));
			}

	return result;
}

class StrncpyTest : public funcperf::ITest
{
public:
	StrncpyTest(const Params& params);

	std::string getId();
	void run(void* func);
	bool verify();

private:
	Params params;

	std::unique_ptr<char[]> m_srcBuffer;
	std::unique_ptr<char[]> m_dstBuffer;
	std::unique_ptr<char[]> m_verifyBuffer;
};

StrncpyTest::StrncpyTest(const Params& params)
	: params(params)
{
	int srcBufferSize = params.srcOffset + params.bytesToCopy;
	int dstBufferSize = params.dstOffset + params.bytesToCopy;
	m_srcBuffer.reset(new char[srcBufferSize]);
	m_dstBuffer.reset(new char[dstBufferSize]);
	m_verifyBuffer.reset(new char[dstBufferSize]);

        m_srcBuffer[srcBufferSize - 1] = '\0';
        m_dstBuffer[dstBufferSize - 1] = '\0';

        for (int i = params.srcOffset; i < srcBufferSize - 1; i++) {
                char rand = i % 0xfe;
                if (rand == '\0')
			rand = 'a';
                m_srcBuffer[i] = rand;
        }

	// prepare verification buffer
	memcpy(m_verifyBuffer.get(), m_dstBuffer.get(), dstBufferSize);

	strncpy(m_verifyBuffer.get() + params.dstOffset,
		m_srcBuffer.get() + params.srcOffset,
		params.bytesToCopy);
}

std::string StrncpyTest::getId()
{
	return "STRNCPY_" + params.getCSVValues("_");
}

void StrncpyTest::run(void* func)
{
	char* (*strncpy_func)(char* dst, const char* src, size_t len) = (char* (*)(char*, const char*, size_t len))func;

	(*strncpy_func)(m_dstBuffer.get() + params.dstOffset,
			m_srcBuffer.get() + params.srcOffset,
			params.bytesToCopy);
}

bool StrncpyTest::verify()
{
	return memcmp(m_dstBuffer.get(), m_verifyBuffer.get(),
		params.dstOffset + params.bytesToCopy) == 0;
}

std::shared_ptr<ITest>
StrncpyFunctionTest::getTest(const ITestParams& testParams)
{
	return std::make_shared<StrncpyTest>(dynamic_cast<const Params&>(testParams));
}

}
}
