#include "IFunctionTest.hpp"
#include "MemcpyFunctionTest.hpp"
#include "StrcpyFunctionTest.hpp"
#include "StrncpyFunctionTest.hpp"
#include "StrcmpFunctionTest.hpp"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>

#include <dlfcn.h>

using namespace funcperf;
using namespace funcperf::string;


static const char* ids[] = {
	"memcpy", "strcpy", "strncpy", "strcmp", NULL
};

static std::unique_ptr<IFunctionTest>
getFTest(const std::string& id)
{

	if (id == "memcpy")
		return std::make_unique<MemcpyFunctionTest>();
	else if (id == "strcpy")
		return std::make_unique<StrcpyFunctionTest>();
	else if (id == "strncpy")
		return std::make_unique<StrncpyFunctionTest>();
	else if (id == "strcmp")
		return std::make_unique<StrcmpFunctionTest>();
	else
		return nullptr;
}

[[noreturn]] void usage()
{
	std::cerr <<
		"Usage: tester <libFilepath> <testId> [short|normal|long]\n\n"
		"Possible values for 'testId':\n";
	for (const char** id = ids; *id; id++)
		std::cerr << "\t" << *id << std::endl;
	exit(1);
}


class Test
{
public:
	~Test();
	void parseArgs(int argc, char** argv);
	void prepare();
	void run();
	void runCompat();
	std::tuple<bool, int64_t> runTest(ITest& test, int iterations);

private:
	std::string lib;
	std::shared_ptr<IFunctionTest> ftest;
	TestLength len = TestLength::shortTest;

	std::string fname;
	void* soHandle = nullptr;
	void* func = nullptr;
};


void Test::parseArgs(int argc, char** argv)
{
	if (argc < 3 || argc > 4)
		usage();

	lib = argv[1];

	fname = argv[2];
	ftest = getFTest(fname);

	if (!ftest || lib.empty())
		usage();

	if (argc == 4) {
		std::string arg = argv[3];

		if (arg == "short")
			len = TestLength::shortTest;
		else if (arg == "normal")
			len = TestLength::normalTest;
		else if (arg == "long")
			len = TestLength::longTest;
		else
			usage();
	}

	ftest->setLength(len);
}


void Test::prepare()
{
	// load shared library
	soHandle = dlopen(lib.c_str(), RTLD_NOW);
	if (!soHandle)
		throw std::runtime_error("Error loading shared library " + lib);

	// get pointer to tested function
	func = dlsym(soHandle, fname.c_str());
	if (!func)
		throw std::runtime_error("Symbol " + fname +
			" not found in " + lib);
}


void Test::runCompat()
{
	bool printHeader = true;
	for (const auto& param : ftest->getTestsParams()) {
		if (printHeader) {
			std::cout << "id\t" << param->getCSVHeaders("\t")
				<< "\titerations\tavgNanos\ttestResult" << std::endl;
			printHeader = false;
		}

		auto test = ftest->getTest(*param);
		int iterations = param->getIterations(len);
		bool testResult;
		int64_t nanos;
		std::tie(testResult, nanos) = runTest(*test, iterations);

		std::cout << test->getId() << "\t"
			<< param->getCSVValues("\t") << "\t"
			<< iterations << "\t";
		std::cout << nanos << "\t"
			<< (testResult ? "SUCCESS" : "FAILURE")
			<< std::endl;
	}
}

namespace {

struct Aggr {
	std::string id;
	std::string values;
	int iterations;
	int64_t nanos;
};

}

void Test::run()
{
	if (ftest->compat)
		runCompat();

	std::cout << "id\t\t" << ftest->headers()
		<< "\titerations\tavgNanos" << std::endl;

	std::map<std::string, std::vector<Aggr>> aggrNanos;
	while (ITest* test = ftest->nextTest()) {
		int iterations = test->iterations(len);
		std::string aggrId = test->aggrId();
		bool testResult;
		int64_t nanos;
		std::tie(testResult, nanos) = runTest(*test, iterations);

		if (aggrId.empty() || !testResult)
			std::cout << test->id() << "\t" << test->values("\t\t") << "\t\t"
				<< iterations << "\t\t" << nanos << '\n';
		else
			aggrNanos[aggrId].push_back(
				{test->id(), test->values("\t\t"), iterations, nanos});

		if (!testResult) {
			std::cout << "FAILURE!\n";
			return;
		}

		if (test->flush()) {
			for (auto& p : aggrNanos) {
				int64_t sum = 0;
				int iters = 0;
				int64_t nanos;
				for (const auto& a : p.second) {
					iters += a.iterations;
					sum += a.nanos;
				}
				nanos = sum / p.second.size();

				auto a = p.second[0];
				std::cout << a.id << "\t" << a.values << "\t\t"
					<< iters << "\t\t" << nanos << '\n';
			}
			aggrNanos.clear();
		}
	}

}


std::tuple<bool, int64_t> Test::runTest(ITest& test, int iterations)
{
	int res;
	struct timespec tp0, tp1;
	int64_t nanos[iterations];
	int64_t totalSum = 0;
	int totalValidMeasures = 0;
	bool verifyResult;

	for (int iteration = 0; iteration < iterations; iteration++) {
		res = clock_gettime(CLOCK_MONOTONIC_PRECISE, &tp0);
		test.run(func);
		res |= clock_gettime(CLOCK_MONOTONIC_PRECISE, &tp1);

		if (res != 0) {
			std::cout << "clock_gettime() failed!\n";
			exit(1);
		}

		uint64_t nano0 = (1000000000LL * tp0.tv_sec) + tp0.tv_nsec;
		uint64_t nano1 = (1000000000LL * tp1.tv_sec) + tp1.tv_nsec;
		nanos[totalValidMeasures++] = nano1 - nano0;
		totalSum += (nano1 - nano0);

		if (iteration == 0)
			verifyResult = test.verify();
	}

	// return an approximation of the median
	std::sort(nanos, nanos + totalValidMeasures);
	return std::tie(verifyResult, nanos[totalValidMeasures / 2]);
}


Test::~Test()
{
	if (soHandle)
		dlclose(soHandle);
}


int main(int argc, char** argv)
{
	Test test;
	test.parseArgs(argc, argv);
	test.prepare();
	test.run();

	return 0;
}
