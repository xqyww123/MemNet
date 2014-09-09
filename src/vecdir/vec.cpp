#include "vec.h"
#include "../tools/file.h"
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
#include <unordered_set>
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
			word_map.insert({{"an","a"},{"are", "be"}, {"is", "be"},{"co",""}, {"ca","-."}, {"www",""},{"com","-."},{"cn","-."},{"net","-."},{"org","-."}, {"nbsp", "-&"}, {"amp","-&"}, {"quot", "-&"}, {"gt","-&"}, {"lt","-&"}, {"cached", "->"}, {"similar","->"}, {"https","->"}, {"jan","janurary"},{"feb","february"},{"mar", "march"},{"apr", "april"},{"jun", "june"},{"jul","july"},{"aug","august"},{"sep","september"},{"oct","october"},{"nov","november"},{"dec","december"},{"books","book"}});
		}
		void Vec::init()
		{
			init_word_map();
		}
		struct Engine { short brc; char word[128]; int wi; Freqs* re; int flags; int like[like_cnt]; char last_sign;};
		void de_kwd(Freqs* re, WVec* kwdvec)
		{
			int mv = 0, kv=0;
			unordered_set<string> kwd(kwdvec->begin(), kwdvec->end(), kwdvec->size());
			for (auto a : *re)
			{
				if (kwd.find(a.first) == kwd.end())
					mv = max(mv, a.second);
				else kv = max(kv, a.second);

			}
			for (auto a : *kwdvec)
			{
				if (re->find(a) != re->end())
					(*re)[a] = (*re)[a]*mv/kv;
			}
		}
		void pushin(Engine* eng)
		{
			if (eng->wi)
			{
				eng->word[eng->wi]='\0';
				for (int i=0;i<eng->wi;++i) eng->word[i] = tolower(eng->word[i]);
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
				eng->wi = 0;
				eng->re->at(str) ++;
			}
		}
		size_t andata(char *ptr, size_t wk_size, size_t nmemb, void *userdata)
		{
			Engine* eng = (Engine*) userdata;
			size_t sz=wk_size*nmemb,  rm; rm = sz; char *now;
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
						 eng->word[eng->wi++] = *now;
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
								}
							}
						}
				}
			}
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
			if (code != 200)
				printf("Web require code : %d\n", code);
			delete errbuf, kwd, url_buf;
			de_kwd(re, wvec);
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
		template<typename Func>
		WVec* read_wvec_inner(Func func)
		{
			WVec *re = new WVec();
			bool enter = false;
			char *buf = new char[255];
			int bi=0;
			while (true)
			{
				int c = func();
				if (c == -1) throw format_error("Unexpected EOF");
				if (!enter)
				{
						if (c==' '||c=='\t'||c=='\n'||c=='\r') continue;
					    else if (c != '(')	throw format_error("A Bad Word Vector. Expecting a '(' to start the vector. like (a b c), and use space to sperate the words.");
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
						if (bi == 255) throw format_error("Too long a word = =");
				}
			}
		}
		WVec* Vec::read_wvec(FILE* f)
	   	{ return read_wvec_inner([=]()->int{ return fgetc(f); }); }
		WVec* Vec::read_wvec(string str)
		{ int i=0; return read_wvec_inner([&]()->int{ return str[i++]; });}
		Frates* Vec::cal_rate(WVec* wvec)
		{
			Freqs* req = cal_freq(wvec);
			Frates* re = cal_rate(req);
			delete req;
			return re;
		}

#define wrt(x) write((char*)&x,sizeof(x))
#define wr2(a,b) write((char*)a,sizeof(b))
#define rd(x) read((char*)&x, (streamsize)sizeof(x))
#define rd2(x,b) read((char*)x, sizeof(b))
		Vec* Vec::read(fstream& f)
		{
			char buf[10];
			f.rd2(buf,char[4]);
			if (strcmp(buf, "Vec")) throw runtime_error("Bad file, there isn't Vec.");
			Vec* re = new Vec();
			re->wvec = rd_vec<string>(f);
			re->spread = rd_vec<string>(f);
			re->freq = rd_vec<float>(f);
			return re;
		}
		WVec* Vec::ui_read_wvec(const char* what)
		{
			WVec* re;
			while (1) {
				if (!Config::q)
					printf("Input the %s: ", what);
				try { re	= read_wvec(stdin); }
		    	catch (const format_error& e)
		    	{ puts(e.what()); continue; }
				break;
			}
			return re;
		}
		void Vec::write(fstream& f)
		{
			f.wrt("Vec");
			wrt_vec(wvec, f);
			wrt_vec(spread, f);
			wrt_vec(freq, f);
		}
	}
}
