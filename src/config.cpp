#include "config.h"
#include <cstring>
#include <string>
#include <unordered_map>
#include <fstream>

namespace Xero
{
	namespace MemNet
	{

		ConDic Config::dic;
		string Config::search_engine_url;

		void add_config(const char* str, ConDic& dic, int& default_cnt)
		{
			char * name, *val;
			int len = strlen(str);
			name = new char[len]; val = new char[len];
			if (2!=sscanf(str, "%s%*[ \t]=%*[ \t]%[^\r\n]", name, val));
			{ strcpy(name, Config::DEFAULT_HEAD); sprintf(name+strlen(Config::DEFAULT_HEAD), "%d", default_cnt++); sscanf("%*[ \t]%[^\r\n]", val); }
			int i; for (i=strlen(val)-1;i>=0&&(val[i]==' '||val[i]=='\t');--i); val[i+1] = '\0';
			dic.insert(make_pair<string,string>(string(name), string(val)));
			delete name, val;
		}
		void read_config_file(const char *file_ad, ConDic& dic, int& default_cnt)
		{
			fstream f(file_ad, fstream::in);
			while (true)
			{
				string str;
				getline(f, str);
				if (f.eof()) break;
				add_config(str.c_str(), dic, default_cnt);
			}
			f.close();
		}
		void read_arg(int argc, char* argv[], ConDic& dic, int& def_cnt)
		{
			for (int i=1;i<argc;++i)
			{
				add_config(argv[i], dic, def_cnt);
			}
		}


		const char* default_url = "http://www.google.com/search?q=%s&lr=lang_en&num=50&gws_rd=ssl&ie=UTF-8";
		void Config::init(int argc, char* argv[])
		{
			int def_cnt = 0;
			read_config_file(TO_STR(CONFIG_FILE), dic, def_cnt);
			read_arg(argc, argv, dic, def_cnt);
			if (dic.find("seach_engine_url") == dic.end()) 
				{ 
					fputs("Warning : search_engine_url not indicated. using : ", stderr);
					fputs(default_url, stderr);
					fputs("However, the url may be BLOCKED in chinese area, please use a proxy or indicate another \
ENGLISH search engin url. (as far as I know, there isn'a stable ENGLISH engine in BLOCKED area. = =)", stderr);
			   	}
			Config::search_engine_url = get("seach_engine_url",default_url);
		}
		string Config::get(string name, string defual)
		{
			try { return dic.at(name); }
			catch(const out_of_range& ) { return defual; }
		}
		int Config::geti(string name, int defual)
		{
			try { return to_i(dic.at(name), defual); }
			catch(const out_of_range& ) { return defual; }
		}
		bool Config::getb(string name, bool defual)
		{
			try { return to_b(dic.at(name), defual); }
			catch(const out_of_range& ) { return defual; }
		}
		double Config::getd(string name, double defual)
		{
			try { return to_d(dic.at(name), defual); }
			catch(const out_of_range& ) { return defual; }
		}
	}
}
		
