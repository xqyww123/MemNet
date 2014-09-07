#pragma once
#include "common.h"
#include <unordered_map>
using namespace std;

typedef unordered_map<string,string> ConDic;
#define FREQ_FILE "./freq.dat"

namespace Xero
{
	namespace MemNet
	{
		class Config
		{
		public:
			static constexpr const char* DEFAULT_HEAD="_";
			static const int spread_num = 1000;

			static ConDic dic;
			static string search_engine_url;

			void init(int argc, char* argv[]);
			string get(string name, string defaul);
			int geti(string name, int defaul);
			bool getb(string name, bool defaul);
			double getd(string name, double defaul);
		};
	}
}

