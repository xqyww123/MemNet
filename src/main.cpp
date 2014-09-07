#include "common.h"
#include "freq/freq.h"
#include <curl/curl.h>
using namespace std;
using namespace Xero::MemNet;

void init(int argc, char* argv[])
{
	freq_init(FREQ_FILE);
	xssert(!curl_global_init(CURL_GLOBAL_ALL));
}

void termi()
{
	freq_free();
	curl_global_cleanup();
}

int main(int argc, char* argv[])
{
	init(argc, argv);

	termi();
	return 0;
}
