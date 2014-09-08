#pragma once
#include "common.h"
#include <unordered_map>
using namespace std;

typedef unordered_map<string,string> ConDic;
#define FREQ_FILE ./freq.dat
#define CONFIG_FILE ~/.memnet

namespace Xero
{
	namespace MemNet
	{
		class Config
		{
		public:
			static constexpr const char* DEFAULT_HEAD="_";
			static string DEFAULT_HEAD_STR;
			static const int spread_num = 1000;

			static ConDic dic;
			static string search_engine_url;
			static string text_editor;
			static string web_browser;
			static string dir_browser;
			static bool q;

			static void init(int argc, char* argv[]);
			static string get(string name, string defaul);
			static int geti(string name, int defaul);
			static bool getb(string name, bool defaul);
			static double getd(string name, double defaul);

			static string action(int i, string defaul);
		};
	}
}

