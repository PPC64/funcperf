#include "TestRunner.hpp"

#include "IFunctionTest.hpp"
#include "string/MemcpyFunctionTest.hpp"
#include "string/StrcpyFunctionTest.hpp"
#include "string/StrncpyFunctionTest.hpp"
#include "string/StrcmpFunctionTest.hpp"

#include <string>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <map>
#include <memory>

#include <dlfcn.h>

using namespace funcperf;
using namespace funcperf::string;

static std::map<std::string, std::shared_ptr<IFunctionTest>> funcTestMap = {
	// possible tests
	{"memcpy", std::make_shared<MemcpyFunctionTest>()},
	{"strcpy", std::make_shared<StrcpyFunctionTest>()},
	{"strncpy", std::make_shared<StrncpyFunctionTest>()},
	{"strcmp", std::make_shared<StrcmpFunctionTest>()}
};


[[noreturn]] void usage()
{
	std::cerr <<
		"Usage: tester --lib <libFilename> --test <testId> "
		"[--length short|normal|long] [--show all|failures]\n\n"
		"Possible values for 'testId':\n";
	for (const auto& kv : funcTestMap)
		std::cerr << "\t" << kv.first << std::endl;
	exit(1);
}


class Test
{
public:
	~Test();
	void parseArgs(int argc, char** argv);
	void prepare();
	void run();

private:
	std::string lib;
	std::shared_ptr<IFunctionTest> ftest;
	TestLength len = TestLength::normalTest;
	bool showFailuresOnly = false;

	void* soHandle = nullptr;
	void* func = nullptr;
};


void Test::parseArgs(int argc, char** argv)
{
	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];

		if (arg == "--lib") {
			if (++i >= argc)
				usage();
			lib = argv[i];

		} else if (arg == "--test") {
			arg = argv[++i];
			auto it = funcTestMap.find(arg);
			if (it == funcTestMap.end())
				usage();
			ftest = it->second;

		} else if (arg == "--length") {
			if (++i >= argc)
				usage();

			arg = argv[i];
			if (arg == "short")
				len = TestLength::shortTest;
			else if (arg == "normal")
				len = TestLength::normalTest;
			else if (arg == "long")
				len = TestLength::longTest;
			else
				usage();

		} else if (arg == "--show") {
			if (++i >= argc)
				usage();

			arg = argv[i];
			if (arg == "all")
				showFailuresOnly = false;
			else if (arg == "failures")
				showFailuresOnly = true;
			else
				usage();

		} else
			usage();
	}

	if (!ftest || lib.empty())
		usage();
}


void Test::prepare()
{
	// load function to be tested
	if (!lib.empty()) {
		// load shared library
		soHandle = dlopen(lib.c_str(), RTLD_NOW);
		if (!soHandle)
			throw std::runtime_error("Error loading shared library " + lib);

		// get pointer to tested function
		std::string fname = ftest->getFunctionName();
		func = dlsym(soHandle, fname.c_str());
		if (!func)
			throw std::runtime_error("Symbol " + fname +
				" not found in " + lib);
	}
}


void Test::run()
{
	bool printHeader = true;
	TestRunner testRunner;
	for (const auto& param : ftest->getTestsParams()) {
		if (printHeader) {
			std::cout << "id\t" << param->getCSVHeaders("\t")
				<< "\titerations\tavgNanos\ttestResult" << std::endl;
			printHeader = false;
		}

		auto test = ftest->getTest(*param);
		int iterations = param->getIterations(len);
		bool testResult;
		int64_t nanos = testRunner.runTest(*test, func, iterations, &testResult);

		if (showFailuresOnly == false || !testResult) {
			std::cout << test->getId() << "\t"
				<< param->getCSVValues("\t") << "\t"
				<< iterations << "\t";
			std::cout << nanos << "\t"
				<< (testResult ? "SUCCESS" : "FAILURE")
				<< std::endl;
		}
	}
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
