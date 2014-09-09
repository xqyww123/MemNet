#include "app.h"
#include <libgen.h>
#include <unistd.h>
#include "tools/file.h"
#include <algorithm>
#define min(a,b) ((a)<(b)?(a):(b))

using namespace std;

namespace Xero{
	namespace MemNet{

		void Record::excu()
		{
			string excu = excu_method;
			if (excu_method.empty()) excu = default_excu();
			if ( system(string_format(excu, adress.c_str()).c_str()))
			{
				puts ("It seems that the excution you indicated failed. using default_excu [Y/n] ?");
				bool to = ui_getbool("Excute using default config? [Y/n] ");
				if (to) { system(string_format(Config::default_default_excu, adress.c_str()).c_str()); }
			}
		}
		void Core::add(Record* rec)
		{
			dic.insert(*rec->tags, rec);
		}
		bool Core::remove(Record* rec)
		{
			dic.remove(rec);
		}
		void Core::ui_add()
		{
			Record* to = new Record();
			to->tags = Vec::read_wvec(Config::action_must(2, "tags"));
			to->adress = extract_tilde(Config::action_must(3, "adress"));

			to->excu_method = Config::get("excu", "");
			add(to);
		}
		Record* Core::ui_find()
		{
			WVec* wv = Vec::read_wvec(Config::action_must(2, "tags"));
			auto pos = dic.query(*wv);
			int sz = min(10, pos->size());
			Record* pr[10];
			puts("Which record you prefer ?");
			for (int i=0;i<sz;++i)
			{
				auto tp = pos->top();
				pr[i] = tp.val()->data; pos->pop();
				cout << i << " : "<< pr[i]->to_string() << endl;
			}
			int id = ui_get<int>("Line number", [=](string s)->int{ int x = to_i(s); if (x<0 || x>=sz) throw x; return x; });	
			return pr[id];
		}
		void Core::ui_remove()
		{
			Record* rec = ui_find();
			remove(rec);
			delete rec;
		}
		void Core::ui_excu()
		{
			ui_find()->excu();
		}
		string Record::to_string() 
		{
		   	string re =  adress + "\t:\t";
			for (auto a : *tags)
			{ re += " "; re += a; }
			return re;
		}

		void Record::write(fstream& f)
		{
			write_string(adress, f);
			wrt_vec(*tags, f);
			write_string(excu_method, f);
		}
		Record* Record::read(fstream& f)
		{
			Record* re = new Record();
			re->adress = read_string(f);
			auto* t = new WVec(); 
			*t = rd_vec<string>(f);
			re->tags = t;
			re->excu_method = read_string(f);
			return re;
		}
		void write_rec(Record* rec, fstream& f) { rec->write(f); }
		void Core::write(fstream& f)
		{
			dic.write(f, (function<void(Record*, fstream&)>)&write_rec);
		}
		void Core::read(fstream& f)
		{
			dic = *Dic<Record*>::read(f, (function<Record*(fstream&)>)Record::read);
		}
		void Core::write_file()
		{
			fstream f(file_adress().c_str(), fstream::out | fstream::binary);
			if (!f.good()) { fprintf(stderr, "Error : fail to write file : %s", file_adress().c_str()); return; }
			write(f);
			f.close();
		}
		void Core::read_file()
		{
			fstream f(file_adress().c_str(), fstream::in | fstream::binary);
			if (!f.good()) return;
			read(f);
			f.close();
		}
		void Core::setup_sta()
		{
			add_action(Action("add", [](Status& a){((Core&)a).ui_add();}, "add a record. usage : add <tags> <adress> "));
			add_action(Action("remove", [](Status& a){((Core&)a).ui_remove();}, "remove a record. usage : remove <tags>"));
			add_action(Action("excu", [](Status& a){((Core&)a).ui_excu();}, "excu a record. usage : excu <tags>"));
			add_action(Action("cd", [](Status& a){((Core&)a).ui_cd();}, "change current directory to record. usage : cd <tags>"));
			

		}
		void Core::cd(Record* rec)
		{
			char* buf = new char[rec->adress.length()+1];
			strcpy(buf, rec->adress.c_str());
			char *dir = dirname(buf);
			chdir(dir);
			delete dir;
			//system(string_format("cd $(dirname \"%s\")", rec->adress.c_str()).c_str());
		}
		void Core::ui_cd()
		{
			cd(ui_find());
		}
	}
}
