#pragma once

#include <cassert>
#include <cstring>

static void initBuffer(std::unique_ptr<char[]> &buf, int sz, bool zero = false)
{
	buf.reset(new char[sz]);
	assert((uintptr_t)buf.get() % 8 == 0);
	if (zero)
		memset(buf.get(), 0, sz);
}
