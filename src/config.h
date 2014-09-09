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
			static bool q;
			static string default_excu;
			static string save_adress;
			const constexpr static char* default_default_excu = "nohup xdg-open \"%s\" 2>/dev/null 1>/dev/null &";

			static void init(int argc, char* argv[]);
			static string get(string name, string defaul);
			static int geti(string name, int defaul);
			static bool getb(string name, bool defaul);
			static double getd(string name, double defaul);

			static string action(int i, string defaul);
			static string action_must (int i, string name);
			static string get_must(string name, string parm_name);
		};
	}
}

