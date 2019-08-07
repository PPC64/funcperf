#include "IFunctionTest.hpp"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>

#include <dlfcn.h>

using namespace funcperf;


static bool vs = true;


[[noreturn]] void usage()
{
	std::cerr <<
		"Usage: tester [flags] <libFilepath> <testId> [short|normal|long]\n"
		"\n"
		"Flags:\n"
			"--no-vs: do not compare against libC impl\n"
		"--csv: output in .csv format\n"
		"--func <name>: function name to use\n"
		"\n"
		"Possible values for 'testId':\n";
	for (const auto& id : FunctionTestFactory::instance().ids())
		std::cerr << "\t" << id << std::endl;
	exit(1);
}


class Test
{
public:
	struct TestResult {
		bool rc;
		int64_t asm_nanos;
		int64_t c_nanos;
		double speedup;
	};

	struct Aggr {
		std::string id;
		std::string values;
		int iterations;
		TestResult res;
	};

	~Test();
	void parseArgs(int argc, char** argv);
	void prepare();
	void run();
	void runCompat();
	TestResult runTest(ITest& test, int iterations);

private:
	std::string lib;
	std::shared_ptr<IFunctionTest> ftest;
	TestLength len = TestLength::shortTest;
	std::string sep = "\t";
	std::string dsep;
	std::string tsep;
	std::string qsep;

	std::string fname;
	void* soHandle = nullptr;
	void* func = nullptr;

	void printResult(const Aggr& aggr) const;
};


void Test::parseArgs(int argc, char** argv)
{
	std::string afname;

	if (argc < 3)
		usage();

	int i = 1;

	// flags
	if (argv[i] == std::string("--no-vs")) {
		vs = false;
		i++;
	}
	if (argv[i] == std::string("--csv")) {
		sep = ",";
		i++;
	}
	if (argv[i] == std::string("--func")) {
		i++;
		afname = argv[i++];
	}

	lib = argv[i++];

	if (i >= argc)
		usage();

	fname = argv[i++];
	ftest = FunctionTestFactory::instance().getFTest(fname);
	if (!afname.empty())
		fname = afname;

	if (!ftest || lib.empty()) {
		std::cerr << "ERROR: lib or function not found\n";
		usage();
	}

	if (i < argc) {
		std::string arg = argv[i++];

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
	ftest->sep = sep;
}


void Test::prepare()
{
	if (sep == "\t") {
		dsep = sep + sep;
		tsep = dsep + sep;
		qsep = dsep + dsep;
	} else
		qsep = tsep = dsep = sep;

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


void Test::run()
{
	std::cout << "id" << qsep << ftest->headers() << dsep
		<< "iterations" << sep;
	if (vs)
		std::cout << "avgNanosC" << sep << "avgNanosASM"
			<< sep << "speedup" << std::endl;
	else
		std::cout << "avgNanos" << std::endl;

	std::map<std::string, std::vector<Test::Aggr>> aggrNanos;
	while (ITest* test = ftest->nextTest()) {
		int iterations = test->iterations(len);
		std::string aggrId = test->aggrId();
		auto res = runTest(*test, iterations);

		Aggr a = {test->id(), test->values(), iterations, res};
		if (aggrId.empty() || !res.rc)
			printResult(a);
		else
			aggrNanos[aggrId].push_back(a);

		if (!res.rc) {
			std::cout << "FAILURE!\n";
			return;
		}

		if (test->flush()) {
			for (auto& p : aggrNanos) {
				auto& vec = p.second;
				Aggr aggr = vec.front();
				aggr.iterations = 0;
				Test::TestResult& res = aggr.res;
				res = {true, 0, 0, 0};

				for (const auto& a : vec) {
					aggr.iterations += a.iterations;
					res.c_nanos += a.res.c_nanos;
					res.asm_nanos += a.res.asm_nanos;
					res.speedup += a.res.speedup;
				}
				res.c_nanos /= vec.size();
				res.asm_nanos /= vec.size();
				if (vs)
					res.speedup /= vec.size();
				printResult(aggr);
			}
			aggrNanos.clear();
		}
	}
}


void Test::printResult(const Aggr& aggr) const
{
	std::cout << aggr.id << sep << aggr.values << dsep
		<< aggr.iterations << dsep;
	if (vs)
		std::cout << aggr.res.c_nanos << dsep;
	std::cout << aggr.res.asm_nanos;
	if (vs)
		std::cout << dsep << aggr.res.speedup;
	std::cout << '\n';
}


Test::TestResult Test::runTest(ITest& test, int iterations)
{
	TestResult tres;
	int64_t nanos[2][iterations];

	tres.c_nanos = 0;
	tres.speedup = 0;
	for (int j = 0, jn = vs? 2 : 1; j < jn; j++) {
		std::function<void()> f;
		if (j == 0)
			f = std::bind(&ITest::run, &test, func);
		else
			f = std::bind(&ITest::runC, &test);
		for (int i = 0; i < iterations; i++) {
			int res;
			struct timespec tp0, tp1;

			res = clock_gettime(CLOCK_MONOTONIC_PRECISE, &tp0);
			f();
			res |= clock_gettime(CLOCK_MONOTONIC_PRECISE, &tp1);

			if (res != 0) {
				std::cout << "clock_gettime() failed!\n";
				exit(1);
			}

			uint64_t nano0 = (1000000000LL * tp0.tv_sec) + tp0.tv_nsec;
			uint64_t nano1 = (1000000000LL * tp1.tv_sec) + tp1.tv_nsec;
			nanos[j][i] = nano1 - nano0;

			if (i == 0 && j == 0)
				tres.rc = test.verify();
		}

		// get median
		std::sort(nanos[j], nanos[j] + iterations);
		int64_t *nanosptr = j == 0? &tres.asm_nanos : &tres.c_nanos;
		*nanosptr = nanos[j][iterations / 2];
	}

	if (vs)
		tres.speedup = ((double)tres.c_nanos / tres.asm_nanos);

	return tres;
}


Test::~Test()
{
	if (soHandle)
		dlclose(soHandle);
}


int main(int argc, char** argv)
{
	try {
		Test test;
		test.parseArgs(argc, argv);
		test.prepare();
		test.run();
	} catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
