#include "common.h"
#include "freq/freq.h"
#include <curl/curl.h>
#include "config.h"
#include "act.h"
#include "vecdir/vec.h"
using namespace std;
using namespace Xero::MemNet;

Status main_st;

void test()
{
	WVec* wv = Vec::read_wvec(stdin);
	Freqs* fr = Vec::cal_freq(wv);
	Frates* rt = Vec::cal_rate(fr);
	Vec* vec = Vec::cal_spread(wv);
	puts("tested");
}
void init(int argc, char* argv[])
{
	freq_init(TO_STR(FREQ_FILE));
	xssert(!curl_global_init(CURL_GLOBAL_ALL));
	Config::init(argc, argv);
	main_st.name = "main";
	main_st.actions.push_back(Action("test", test));
	StatusMng::init(main_st);
	Vec::init();
}

void termi()
{
	freq_free();
	curl_global_cleanup();
}

int main(int argc, char* argv[])
{
	init(argc, argv);
	StatusMng::invoke(Config::action(1, "help"));

	termi();
	return 0;
}
