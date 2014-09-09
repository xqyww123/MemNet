#include "common.h"
#include "app.h"
#include "freq/freq.h"
#include <curl/curl.h>
#include "config.h"
#include "act.h"
#include "vecdir/vec.h"
#include "vecdir/dic.h"
using namespace std;
using namespace Xero::MemNet;

Status main_st;

Core* mst;
void init(int argc, char* argv[])
{
	freq_init(TO_STR(FREQ_FILE));
	xssert(!curl_global_init(CURL_GLOBAL_ALL));
	Config::init(argc, argv);
	//main_st.name = "main";
	//main_st.add_action(Action("test", [](Status& a){test();}));
	//main_st.add_action(Action("tread", [](Status& a){tread();}));
	mst = new Core();
	StatusMng::init(*mst);
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
	StatusMng::run_cmd();
	delete mst;
	termi();
	return 0;
}
