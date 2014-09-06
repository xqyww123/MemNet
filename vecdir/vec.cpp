#include "vec.h"
#include "../freq/freq.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <curl/curl.h>
#include <cstdio>
#include "../config.h"
#include <queue>
using namespace std;

namespace Xero
{
	namespace MemNet
	{
		struct Engine { bool utf; short brc; char word[128]; int wi; Freqs* re;};
		void pushin(Engine* eng)
		{
			if (eng->wi)
			{
				eng->word[eng->wi]='\0';
				string str(eng->word); 
				if (eng->re->find(str) == eng->re->end())
					eng->re->insert(make_pair<string,int>((string)str, (int)0));
				eng->re->at(str) ++;
				eng->wi = 0;
			}
		}
		size_t andata(char *ptr, size_t wk_size, size_t nmemb, void *userdata)
		{
			Engine* eng = (Engine*) userdata;
			size_t sz=wk_size*nmemb,  rm; rm = sz; char *now;
			for (now=ptr;rm;--rm,now++)
			{
				if (*now < 0) 
				{
					eng->utf = true;
					pushin(eng);
				}
				else 
				{
					if (eng->utf) { eng->utf = false; continue; }
					switch (*now)
					{
						case '<' :
							if (! eng->brc++) pushin(eng);  break;
						case '>' : 
							eng->brc--; break;
					}
					if (!eng->brc)  {
						if ((*now <= 'Z' && *now >= 'A' || *now <= 'z' && *now >= 'a' || *now == '-'))
							{ eng->word[eng->wi++] = *now; }
						else pushin(eng);
					}
				}
			}
		}

		char* prt_WVec(WVec* wvec)
		{
			int cnt = 0;
			for (auto a : *wvec)
				cnt += a.length();
			char * re = new char[cnt+1];
			cnt = 0;
			for (auto a : *wvec)
				strcpy(re+(cnt+=a.length()), a.c_str());
			return re;
		}
		Freqs* Vec::cal_freq(WVec* wvec)
		{
			CURL* curl=curl_easy_init();
			char* kwd = prt_WVec(wvec);
			char* url_buf = new char[strlen(kwd)+Config::search_engine_url.length()+1];
			sprintf(url_buf, Config::search_engine_url.c_str(), kwd);
			ce_setopt(curl, CURLOPT_URL, url_buf);
			Freqs * re = new Freqs();
			Engine eng; memset(&eng, 0, sizeof(eng));
			eng.re = re;
			ce_setopt(curl,  CURLOPT_WRITEFUNCTION, andata);
			ce_setopt(curl,  CURLOPT_WRITEDATA, &eng);
			char * errbuf = new char[CURL_ERROR_SIZE];
			ce_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
			if (!curl_easy_perform(curl))
			{ fputs("Error, curl failed:", stderr); fputs(errbuf, stderr); }
			curl_easy_cleanup(curl);
			delete errbuf, kwd, url_buf;
			return re;
		}
		float Vec::cal_rate(int num, string word)
		{
			float t = freq_query(word);
			if (t < 1E-10) t = freq_default;
			return (float)num*log10(t/freq_most);
		}
		Frates* Vec::cal_rate(Freqs* frq)
		{
			Frates* re = new Frates();
			for (auto a : *frq)
			{
				re->insert(make_pair<string, float>((string)a.first, cal_rate((int)a.second,a.first)));
			}
			return re;
		}
		Vec* Vec::cal_spread(Frates* fra, WVec* wvec)
		{
			Vec* re = new Vec();
			re->wvec = *wvec;
			re->spread.reserve(Config::spread_num);
			re->freq.reserve(Config::spread_num);
			priority_queue<pair<float,string*>, vector<pair<float,string*> >, greater<pair<float, string*> > > heap;
			for (auto a : *fra)
				heap.push(make_pair<float, string*>((float)a.second, (string*)&a.first));
			for (int i=0;i<Config::spread_num;++i)
			{
				auto a = heap.top(); heap.pop();
				re->spread.push_back(*a.second);
				re->freq.push_back(a.first);
			}
			return re;
		}
		Vec* Vec::cal_spread(WVec* wvec)
		{
			Freqs* freq; Frates *fra;
			auto re = cal_spread(fra=cal_rate(freq=cal_freq(wvec)), wvec);
			delete freq, fra;
			return re;
		}
	}
}
