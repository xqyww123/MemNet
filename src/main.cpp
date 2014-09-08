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

void test()
{
	WVec* wv = Vec::read_wvec(stdin);
	Dic<string> dic;
	string a("aaaaaa");
	dic.insert(*wv,a); delete wv;
	wv = Vec::read_wvec(stdin);
	string b("bbbbbb");
	dic.insert(*wv, b); delete wv;
	wv = Vec::read_wvec(stdin);
	dic.insert(*wv, b); 
	auto re = dic.query(*wv);
	auto ra = re->top(); re->pop();
	auto rb = re->top(); re->pop();
	auto rc = re->top(); re->pop();
	puts("tested");
	fstream f("test", fstream::out | fstream::binary);
	dic.write(f, wrt_str);
	f.close();
	delete wv;
}
void tread()
{
	fstream f("test", fstream::in | fstream::binary);
	Dic<string>* dic = Dic<string>::read(f, rd_str);
	auto ta = dic->data[0];
	auto tb = dic->data[1];
	auto tc = dic->data[2];
	puts("read\n");
	delete dic;
}
void init(int argc, char* argv[])
{
	freq_init(TO_STR(FREQ_FILE));
	xssert(!curl_global_init(CURL_GLOBAL_ALL));
	Config::init(argc, argv);
	//main_st.name = "main";
	//main_st.add_action(Action("test", [](Status& a){test();}));
	//main_st.add_action(Action("tread", [](Status& a){tread();}));
	MainSta mst;
	StatusMng::init(mst);
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
