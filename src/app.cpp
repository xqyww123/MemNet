#include "app.h"
#include "common.h"
#include <iostream>
#include <algorithm>
#define min(a,b) (((a)<(b))?(a):(b))

using namespace std;

namespace Xero {
	namespace MemNet {

		const char* note_lst_cmds[] = {"notes", "dirs", "links", "all", "*"};
		const int note_lst_cmds_len = 5;
		void show_list_title(int i) { printf("Showing list of %s\n",note_lst_cmds[i] ); }
		void NoteSta::list_note() { show_list_title(0); prt_strs(current->note); }
		void NoteSta::list_dirs() { show_list_title(1); prt_strs(current->dirs); }
		void NoteSta::list_links(){ show_list_title(2); prt_strs(current->links); }
		void NoteSta::list_all() { show_list_title(3); list_note(); list_dirs(); list_links(); }
		void NoteSta::list(string name)
		{
			switch (like_strs(note_lst_cmds, note_lst_cmds_len, name.c_str()))
			{
				case 0:
					list_note(); break;
				case 1:
					list_dirs(); break;
				case 2:
					list_links(); break;
				case 3:
				case 4:
					list_all(); break;
				default:
					throw CommandError(string("Bad list target, may one of : ") + concact(note_lst_cmds, note_lst_cmds_len, ", "));
			}
		}
		void NoteSta::ui_list() 
		{ 
			string str; cin >> str; 
			try { list(str); }
			catch (const CommandError&) 
			{ printf("Bad list target, one of : %s.", concact(note_lst_cmds, note_lst_cmds_len, ",").c_str()); }
		}
		string to_string(const NoteTip& tip)
		{
			return tip.name + "\t:\t" + trim(tip.val, 30) + (tip.val.length()>30?"...":"");
		}
		string NoteTip::active()
		{
			char *buf;
			switch(type)
			{
				case Type::note:
					buf = new char[val.length() + Config::text_editor.length()+1];
					sprintf(buf, Config::text_editor.c_str(), val.c_str());
				case Type::dir:
					buf = new char[val.length() + Config::dir_browser.length() + 1];
					sprintf(buf, Config::dir_browser.c_str(), val.c_str());
				case Type::link:
					buf = new char[val.length() + Config::web_browser.length() + 1];
					sprintf(buf, Config::web_browser.c_str(), val.c_str());
				default:
					throw logic_error("Bad Type");
			}
			system(buf);
		}
		void NoteSta::active(vector<NoteTip>& tips, string name)
		{
			auto x = find_if(tips.begin(), tips.end(), [&](const NoteTip& tip)->bool{ return tip.name==name; });
			if (x == tips.end()) throw out_of_range(string_format("Not found %s in the tips", name.c_str()));
			x->active();
		}
		void NoteSta::active(string name)
		{
			auto x = find_if(current->note.begin(), current->note.end(), [&](const NoteTip& tip){ return tip.name==name; });
			if (x == current->note.end())  x = find_if(current->links.begin(), current->links.end(), [&](const NoteTip& tip){ return tip.name==name; });
			if (x == current->links.end())  x = find_if(current->dirs.begin(), current->dirs.end(), [&](const NoteTip& tip){ return tip.name==name; });
			if (x == current->dirs.end())	throw out_of_range(string_format("Not found %s in the tips", name.c_str()));
			x->active();
		}
		void NoteSta::ui_active() 
		{ 
			string name; cin >> name; 
			try { active(name); }
			catch (const out_of_range& ) { printf("Sorry, not found tip named %s.\n", name.c_str());  }
		}
		void Note::show_tags()
		{
			printf("Note %s 's tags : \n", name.c_str());
			list_strs(*tags);
		}
		void NoteSta::show_info()
		{
			printf("Note %s : \n", current->name.c_str());
			list_all();
			current->show_tags();
		}
		void MainSta::open(Note& note)
		{
			note_sta.set_current(&note);
			StatusMng::trans(note_sta);
		}
		void NoteSta::close()
		{
			StatusMng::trans(*main_sta);
		}
		void Note::show_oneline()
		{
			printf("%-16s:\t", name.c_str());
			for (int i=0;i<min(tags->size(), 5);++i)
				printf(" %-8s", tags->at(i).c_str());
			if (tags->size() > 5)
				printf("...");
			putchar('\n');
		}
		void MainSta::list_sta()
		{
			puts("Listing all note : ");
			printf("%-16s:\tTags\n", "Note Name");
			for (auto now : dic.data)
			{
				auto a = now->data;
				a->show_oneline();
			}
			putchar('\n');
		}
		Dic<Note*>::Result* MainSta::possible(WVec& tags)
		{
			return dic.query(tags);
		}
		Note* MainSta::ui_select(WVec& tags)
		{
			auto re = possible(tags);
			
			Note* nts[10];
			printf("%-16s:\tTags\n", "Possible note");
			for (int i=0;i<10;++i)
			{
				auto a = re->top(); re->pop();
				nts[i] = a.val()->data;
				printf("%d : ", i); nts[i]->show_oneline();
			}

			int nl; printf("- which note you prefer ? (the line number) : ");
			while (1) {
				scanf("%d", &nl);
				if (nl< 0 || nl > 9) { puts("Wrong line number = =."); }
				else break;
			}
			Note* tar = nts[nl];
			open(*tar);

			delete re;
			return tar;
		}
		Note* MainSta::ui_select()
		{
			WVec* tags = Vec::read_wvec(stdin);
			auto re = ui_select(*tags);
			delete tags;
			return re;
		}
		void MainSta::sta_setup()
		{
			add_action(Action("open", [](Status& a){((MainSta&)a).ui_open();}, "open a note."));
			add_action(Action("create", [](Status& a){((MainSta&)a).ui_create_new();}));
		}
		void MainSta::create_new(WVec& tags)
		{
			Note* nt = new Note();
			dic.insert(tags, nt);
		}
		void MainSta::ui_create_new()
		{
			WVec* wv = Vec::read_wvec(stdin);
			create_new(*wv);
			delete wv;
		}
		void MainSta::drop(Note* nt)
		{
			auto to = find_if(dic.data.begin(), dic.data.end(), [=](Pair<Note*>* a){return a->data==nt;});
			if (to == dic.data.end()) throw out_of_range("Note not exist");
			dic.data.erase(to);
		}
		void MainSta::tag_move(Note* nt, WVec& wvec)
		{
			drop(nt);
			dic.insert(wvec, nt);
		}
		void NoteSta::ui_drop()
		{
			int c;
			if (Config::q)
			{
				do { printf("Are you sure to drop the note? [Y/n]"); c = getchar(); }
				while (c == 'Y' || c=='n');
				if (c == 'n') return;
			}
			main_sta -> drop(current);
		}
		void NoteSta::ui_move()
		{
			puts("Input new tags.");
			WVec* wv;
			try { wv = Vec::read_wvec(stdin); }
			catch(const runtime_error& e) 
			{ printf(e.what()); return; }
			main_sta->tag_move(current, *wv);
			delete wv;
		}
		void NoteSta::setup_sta()
		{
			add_action(Action("list", [](Status& a){ ((NoteSta&)a).ui_list();}, string("list tips. Usage : list ["+concact(note_lst_cmds, note_lst_cmds_len, "|")+"].")));
			add_action(Action("active", [](Status& a){((NoteSta&)a).ui_active();}, "active a tip. Usage : active <the tip name>"));
			add_action(Action("show", [](Status& a){((NoteSta&)a).show_info();}, "show current note's info."));
			add_action(Action("close", [](Status& a){((NoteSta&)a).close();}, "close current note and return back to main status."));
			add_action(Action("drop", [](Status& a){((NoteSta&)a).ui_drop();}, "delete current note forever."));
			add_action(Action("move", [](Status& a){((NoteSta&)a).ui_move();}, "re-assign current note's tags."));
		}
	}
}
