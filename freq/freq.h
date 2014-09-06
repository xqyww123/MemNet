#pragma once
#include "../common.h"
#include <string>

namespace Xero
{
	namespace MemNet
	{
		const float freq_least=1E-5;
		extern float freq_most;
		const float freq_default=1E-5;

		void freq_init(const char* adress);
		double freq_query(string str);
		void freq_free();
	}
}
