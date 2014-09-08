#include "config.h"
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>

namespace Xero
{
	namespace MemNet
	{

		bool Config::q;
		string Config::text_editor;
		string Config::web_browser;
		string Config::dir_browser;
		ConDic Config::dic;
		string Config::search_engine_url;
		string Config::DEFAULT_HEAD_STR;

		void add_config(const char* str, ConDic& dic, int& default_cnt)
		{
			char * name, *val;
			int len = strlen(str);
			name = new char[len]; val = new char[len];
			if (2!=sscanf(str, "%s%*[ \t]=%*[ \t]%s", name, val))
			{ 
				dic.insert(make_pair<string,string>((string)Config::DEFAULT_HEAD_STR+to_string(default_cnt++), string(str)));
				delete name, val;
				return;
			}
			int i; for (i=strlen(val)-1;i>=0&&(val[i]==' '||val[i]=='\t');--i); val[i+1] = '\0';
			dic.insert(make_pair<string,string>(string(name), string(val)));
			delete name, val;
		}
		void read_config_file(const char *file_ad, ConDic& dic, int& default_cnt)
		{
			fstream f(file_ad, fstream::in);
			if (!f.good()) return;
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
			for (int i=0;i<argc;++i)
			{
				add_config(argv[i], dic, def_cnt);
			}
		}


		const char* default_url = "http://www.google.com/search?q=%s&lr=lang_en&num=50&gws_rd=ssl&ie=UTF-8";
		const char* default_text = "vim %s";
		const char* default_web = "xdg-open %s";
		const char* default_dir = "xdg-open %s";
		void Config::init(int argc, char* argv[])
		{
			DEFAULT_HEAD_STR = DEFAULT_HEAD;
			int def_cnt = 0;
			read_config_file(TO_STR(CONFIG_FILE), dic, def_cnt);
			read_arg(argc, argv, dic, def_cnt);
			if (dic.find("seach_engine_url") == dic.end()) 
				{ 
					fputs("Warning : search_engine_url not indicated. using : ", stderr);
					fputs(default_url, stderr);
					fputs("However, the url may be BLOCKED in chinese area, please use a proxy or indicate another \
ENGLISH search engin url. (as far as I know, there isn'a stable ENGLISH engine in BLOCKED area. = =)\n", stderr);
			   	}
			if (dic.find("text_editor") == dic.end())
			{
				fputs("Warning : text_editor not set, using default text_editor : ", stderr);
				fputs(default_text, stderr);
			}

			Config::search_engine_url = get("seach_engine_url",default_url);
			Config::text_editor = get("text_editor", default_text);
			Config::web_browser = get("web_browser", default_web);
			Config::dir_browser = get("dir_browser", default_dir);
			Config::q = getb("q", false);
		}
		string Config::get(string name, string defual)
		{
			try { return dic.at(name); }
			catch(const out_of_range& ) { return defual; }
		}
		int Config::geti(string name, int defual)
		{
			int re;
			try { re = to_i(dic.at(name)); }
			catch(const out_of_range& ) { return defual; }
			catch(const format_error&) 
				{ fprintf(stderr, "You indicated %s which only receive number. We can not understand your indication and the defualt value was adopted\n", name.c_str()); return defual; }
			return re;
		}
		bool Config::getb(string name, bool defual)
		{
			bool re;
			try { re = to_b(dic.at(name)); }
			catch(const out_of_range& ) { return defual; }
			catch(const format_error&) 
				{ fprintf(stderr, "You indicated %s which only receive Y/N value. We can not understand your indication and the defualt value was adopted\n", name.c_str()); return defual; }
			return re;
		}
		double Config::getd(string name, double defual)
		{
			double re;
			try { re = to_d(dic.at(name)); }
			catch(const out_of_range& ) { return defual; }
			catch(const format_error&) 
				{ fprintf(stderr, "You indicated %s which only receive float. We can not understand your indication and the defualt value was adopted\n", name.c_str()); return defual; }
			return re;
		}
		string Config::action(int i, string defual)
		{
			return get(Config::DEFAULT_HEAD_STR+to_string(i), defual);
		}
	}
}
		
