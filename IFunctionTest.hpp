#pragma once

#include "ITest.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <vector>

namespace funcperf {

class IFunctionTest
{
public:
	virtual ~IFunctionTest() = default;

	void setLength(TestLength len)
	{
		this->len = len;
	}

	virtual std::string headers() const = 0;
	virtual ITest* nextTest() = 0;

	std::string sep;
protected:
	TestLength len;
};

class FunctionTestFactory
{
public:
	static FunctionTestFactory& instance()
	{
		static FunctionTestFactory _instance;
		return _instance;
	}

	std::unique_ptr<IFunctionTest>
	getFTest(const std::string& id)
	{
		auto it = _funcs.find(id);
		if (it == _funcs.end())
			return nullptr;
		return std::unique_ptr<IFunctionTest>((*it->second)());
	}

	void registerFunction(const std::string& name,
		IFunctionTest* (*factory)())
	{
		_funcs[name] = factory;
	}

	std::vector<std::string> ids() const
	{
		std::vector<std::string> vec;

		for (const auto& p : _funcs)
			vec.push_back(p.first);
		return vec;
	}

private:
	FunctionTestFactory() = default;
	FunctionTestFactory(const FunctionTestFactory&) = delete;
	FunctionTestFactory& operator=(const FunctionTestFactory&) = delete;

	std::map<std::string, IFunctionTest* (*)()> _funcs;
};

}
