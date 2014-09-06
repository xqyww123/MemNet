#pragma once
#include "../common.h"
#include <string>

namespace Xero
{
	namespace MemNet
	{
		void freq_init(const char* adress);
		double freq_query(string str);
		void freq_free();
	}
}
