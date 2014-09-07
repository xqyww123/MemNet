#include "vec.h"
#include "../freq/freq.h"
#include <stdexcept>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <curl/curl.h>
#include <cstdio>
#include "../config.h"
#include <queue>
#include <map>
using namespace std;

namespace Xero
{
	namespace MemNet
	{
		const int like_cnt=4;
		const char* like_lst[]={"script", "/script", "style", "/style"};
		const char like_stat[]={1,-1,2,-2};
		map<string, string> word_map;
		void init_word_map() 
		{
			word_map.insert({{"an","a"},{"are", "be"}, {"is", "be"},{"co",""}, {"ca","-."}, {"www",""},{"com","-."},{"cn","-."},{"net","-."},{"org","-."}, {"nbsp", "-&"}, {"amp","-&"}, {"quot", "-&"}, {"gt","-&"}, {"lt","-&"}, {"cached", "->"}, {"similar","->"}, {"https","->"}});
		}
		void Vec::init()
		{
			init_word_map();
		}
		struct Engine { short brc; char word[128]; int wi; Freqs* re; int flags; int like[like_cnt]; char last_sign;};
		void pushin(Engine* eng)
		{
			if (eng->wi)
			{
				eng->word[eng->wi]='\0';
				for (int i=0;i<eng->wi;++i) eng->word[i] = tolower(eng->word[i]);
				puts(eng->word);
				string str(eng->word); 
				if (word_map.find(str) != word_map.end())
				{
					str = word_map[str];
					if (str[0]=='-')
					{
						if 	(eng->last_sign==str[1]) 
						{ eng->wi=0; return; }
						else 
							str = string(eng->word); 
					}
				}
				if (str.length() < 2) { eng->wi=0; return; }
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
			//puts(ptr);
			printf("XxX start %d \n", eng->flags);
			for (now=ptr;rm;--rm,now++)
			{
				//fflush(stdout);
				if (*now < 0) 
				{
					pushin(eng);
				}
				else 
				{
					switch (*now)
					{
						case '<' :
							if (eng->flags) break;
							if (! eng->brc++) pushin(eng);
							eng->last_sign = *now;
						  	break;
						case '>' : 
							if (eng->flags) break;
							eng->brc--; 
							memset(eng->like, 0xff, sizeof(int[like_cnt]));
							eng->last_sign = *now;
							break;
					}
					if (!eng->brc)  {
						if (eng->flags) continue;
						if ((*now <= 'Z' && *now >= 'A' || *now <= 'z' && *now >= 'a'))
							{ 
								eng->word[eng->wi++] = *now; }
						else 
						{
							pushin(eng);
							eng->last_sign = *now;
						}
					}
						if (*now == '<') { for (int j=0;j<like_cnt;++j) eng->like[j]=0; }
						else
						{
							for (int j=0;j<like_cnt;++j)
							{
								if (eng->like[j]==-1) continue;
								if (*now != like_lst[j][eng->like[j]++])
								{ eng->like[j]=-1; continue; }
								if (!like_lst[j][eng->like[j]])
								{
									eng->like[j] = -1;
									if (like_stat[j]>0)
										eng->flags |= 1<<(like_stat[j]);
									else eng->flags &= (-1 ^ (1<<(-like_stat[j])));
									printf("XxX %d : ", eng->flags);
									for (int i=0;i<20;++i) putchar(*(now-10+i));
									putchar('\n');
								}
							}
						}
				}
			}
			printf("Enter : %d\n", sz);
			return sz;
		}

		char* prt_WVec(WVec* wvec)
		{
			int cnt = 0;
			for (auto a : *wvec)
				cnt += a.length()+1;
			char * re = new char[cnt];
			cnt = 0;
			for (auto a : *wvec)
			{
				strcpy(re+cnt, a.c_str());
				re[(cnt+=a.length())++]='+';
			}
			re[cnt-1] = '\0';
			return re;
		}
		Freqs* Vec::cal_freq(WVec* wvec)
		{
			CURL* curl=curl_easy_init();
			char* kwd = prt_WVec(wvec);
			char* url_buf = new char[strlen(kwd)+Config::search_engine_url.length()+1];
			sprintf(url_buf, Config::search_engine_url.c_str(), kwd);
			printf("Url : \n%s\n", url_buf);
			ce_setopt(curl, CURLOPT_URL, url_buf);
			Freqs * re = new Freqs();
			Engine eng; memset(&eng, 0, sizeof(eng));
			eng.re = re;
			ce_setopt(curl,  CURLOPT_WRITEFUNCTION, andata);
			ce_setopt(curl,  CURLOPT_WRITEDATA, &eng);
			ce_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 6.1; Windows XP)");
			char * errbuf = new char[CURL_ERROR_SIZE];
			ce_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
			if (curl_easy_perform(curl))
			{ fputs("Error, curl failed:", stderr); fputs(errbuf, stderr); }
			curl_easy_cleanup(curl);
			int code; curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
			printf("Code : %d\n", code);
			delete errbuf, kwd, url_buf;
			return re;
		}
		float Vec::cal_rate(int num, string word)
		{
			float t = freq_query(word);
			if (t < 1E-10) t = freq_default;
			return (float)num*log10(freq_most/t);
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
			priority_queue<pair<float,string> > heap;
			for (auto a : *fra)
				heap.push(make_pair<float, string>((float)a.second,(string)a.first));
			for (int i=0;i<Config::spread_num && !heap.empty();++i)
			{
				auto a = heap.top(); heap.pop();
				re->spread.push_back((string)a.second);
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
		void pushc(WVec& to, char* buf, int& bi)
		{
			if (bi == 0) return;
			buf[bi] = '\0';
			to.push_back(string(buf));
			bi = 0;
		}
		WVec* Vec::read_wvec(FILE* f)
		{
			WVec *re = new WVec();
			bool enter = false;
			char *buf = new char[255];
			int bi=0;
			while (true)
			{
				int c = fgetc(f);
				if (c == -1) throw runtime_error("Unexpected EOF");
				if (!enter)
				{
						if (c==' '||c=='\t') continue;
					    else if (c != '(')	throw runtime_error("A Bad Word Vector. Expecting a '(' to start the vector.");
						enter = true; 
						continue;
				}
				switch(c)
				{
					case ')':
						pushc(*re, buf, bi);
						return re;
					case ' ':
					case '\t':
						pushc(*re, buf, bi);
						break;
					default:
						buf[bi++] = c;
						if (bi == 255) throw runtime_error("Too long a word = =");
				}
			}
		}
	}
}
