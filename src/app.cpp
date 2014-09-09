#include "app.h"
#include "tools/file.h"
#include "common.h"
#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#define min(a,b) (((a)<(b))?(a):(b))

using namespace std;

namespace Xero {
	namespace MemNet {

		const char* note_lst_cmds[] = {"notes", "dirs", "links", "all", "*"};
		const int note_lst_cmds_len = 5;
		void show_list_title(int i) { printf("Showing list of %s\n",note_lst_cmds[i] ); }
		void NoteSta::list_note() { show_list_title(0); prt_strs(current->note, (function<string(const NoteTip&)>)to_string); }
		void NoteSta::list_dirs() { show_list_title(1); prt_strs(current->dirs, (function<string(const NoteTip&)>)to_string); }
		void NoteSta::list_links(){ show_list_title(2); prt_strs(current->links, (function<string(const NoteTip&)>)to_string); }
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
			string str;
			if (user_input_newline())
				str = "all";
			else cin >> str; 
			try { list(str); }
			catch (const CommandError&) 
			{ printf("Bad list target, one of : %s.", concact(note_lst_cmds, note_lst_cmds_len, ",").c_str()); }
		}
		string to_string(const NoteTip& tip)
		{
			return tip.name + "\t:\t" + trim(tip.val, 30) + (tip.val.length()>30?"...":"");
		}
		void NoteTip::active()
		{
			char *buf;
			switch(type)
			{
				case Type::note:
					buf = new char[val.length() + Config::text_editor.length()+1];
					sprintf(buf, Config::text_editor.c_str(), val.c_str());
					break;
				case Type::dir:
					buf = new char[val.length() + Config::dir_browser.length() + 1];
					sprintf(buf, Config::dir_browser.c_str(), val.c_str());
					break;
				case Type::link:
					buf = new char[val.length() + Config::web_browser.length() + 1];
					sprintf(buf, Config::web_browser.c_str(), val.c_str());
					break;
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
			else goto find_end;
			if (x == current->links.end())  x = find_if(current->dirs.begin(), current->dirs.end(), [&](const NoteTip& tip){ return tip.name==name; });
			else goto find_end;
			if (x == current->dirs.end())	throw out_of_range(string_format("Not found %s in the tips", name.c_str()));
find_end:
			x->active();
		}
		void NoteSta::ui_active() 
		{ 
			string name; 
			if (user_input_newline()) printf("Which one (name)? ");
			cin >> name; 
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
			
			int sz = min(10, re->size());
			if (!sz)
			{
				puts("Sorry.. no tip found.");
				return NULL;
			}
			Note* nts[10];
			printf("%-16s:\tTags\n", "Possible note");
			for (int i=0;i<10 && !re->empty();++i)
			{
				auto a = re->top(); re->pop();
				nts[i] = a.val()->data;
				printf("%d : ", i); nts[i]->show_oneline();
			}

			int nl; 
			while (1) {
				printf("- which note you prefer ? (the line number) : ");
				if (scanf("%d", &nl) != 1 ||	nl< 0 || nl >= sz) { puts("Wrong line number = =."); string tmp; getline(cin, tmp); }
				else break;
			}
			Note* tar = nts[nl];
			open(*tar);

			delete re;
			return tar;
		}
		Note* MainSta::ui_select()
		{
			WVec* tags = Vec::ui_read_wvec("tags");
			auto re = ui_select(*tags);
			delete tags;
			return re;
		}
		void MainSta::ui_open() { ui_select(); }
		void MainSta::sta_setup()
		{
			add_action(Action("open", [](Status& a){((MainSta&)a).ui_open();}, "open a note."));
			add_action(Action("create", [](Status& a){((MainSta&)a).ui_create_new();}));
			add_action(Action("list", [](Status& a){((MainSta&)a).ui_list();}, "list all note"));
			add_action(Action("look", [](Status& a){((MainSta&)a).ui_open_by_name();}, "open a note by name"));
		}
		Note* MainSta::create_new(WVec& tags)
		{
			Note* nt = new Note();
			nt->tags = &tags;
			dic.insert(tags, nt);
			return nt;
		}
		void MainSta::ui_create_new()
		{
			WVec* wv = Vec::ui_read_wvec("tags");
			Note* nt = create_new(*wv);
			printf("Then the name ? : ");
			string str; cin >> str;
			nt -> name = str;
			open(*nt);
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
			WVec* wv = Vec::ui_read_wvec("new tags");
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
			add_action(Action("add", [](Status& a){((NoteSta&)a).ui_addtip();}, "add new note tip"));

		}

		NoteTip& Note::add_tip(const NoteTip& tip)
		{
			switch(tip.type)
			{
				case NoteTip::Type::note:
					note.push_back(tip); 
					return note[note.size()-1];
				case NoteTip::Type::dir:
					dirs.push_back(tip);
					return dirs[dirs.size()-1];
				case NoteTip::Type::link:
					links.push_back(tip); 
					return links[links.size()-1];
				default:
					throw logic_error("Strange...");
			}
		}
		NoteTip& Note::add_tip(const string& name, const string& val,NoteTip:: Type type)
		{
			NoteTip tip;
			tip.name = name;
			tip.val = val;
			tip.type = type;
			return add_tip(tip);
		}
		NoteTip::Type check_type_str(const string& str)
		{
			if(like(str, "note")) return NoteTip::Type::note;
			if (like(str, "link")) return NoteTip::Type::link;
			if (like(str, "director")) return NoteTip::Type::dir;
			return NoteTip::Type::badtype;
		}
		void NoteSta::ui_addtip()
		{
			cout << "Creating tip..." << endl;
			string name ; 
			do
			{ 
				name = ui_getval("\tname : ");  
				if (current->contains_tip(name))
					printf("Sorry, there has been a tip called %s", name.c_str());
				else break;
			}
			while (true);
			NoteTip::Type tp;
			do
			{ 
				string type = ui_getval("\ttype : "); 
				tp = check_type_str(type);
				if (tp == NoteTip::Type::badtype)
					puts("Type must be one of { note, link, dir }");
			} while (tp == NoteTip::Type::badtype);
			string val;
			switch (tp)
			{
				case NoteTip::Type::note:
					val = current->workdir()+"/"+name;
					break;
				case NoteTip::Type::link:
				case NoteTip::Type::dir:
					printf("value : ");
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					getline(cin, val);
					break;
				default: exit(1);
			}
			NoteTip& nt = current->add_tip(name, val, tp);
			cout << "comment (optional) (Ctrl-D to terminate) : "<<endl;
			string comment;
			if (!user_input_newline()) 
			{ 
				getline(cin, comment, (char)4);  
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				cin.clear();
			}
			nt.comment = comment;

			if (tp == NoteTip::Type::note)
				nt.active();
		}
		bool Note::contains_tip(string name)
		{
			for (auto a : note) if (a.name == name) return true;
			for (auto a : dirs) if (a.name == name) return true;
			for (auto a : links) if (a.name == name) return true;
			return false;
		}
		void NoteSta::set_current(Note* note) 
		{ 
			current = note; 
			name = string("Note - ") + current->name; 
			struct stat stabuf;
			if (stat(current->workdir().c_str(), &stabuf) && mkdir(current->workdir().c_str(), S_IRWXU | S_IRWXG)) 
				throw runtime_error(string_format("Error : can not access notedir : %s", current->workdir().c_str()));
		}

	/******* File level ********/
		void MainSta::read_file()
		{
			fstream f(save_adress().c_str(), fstream::in | fstream::binary);
			if (!f.good()) return;
			dic = *Dic<Note*>::read(f, (function<Note*(fstream&)>)Note::read_Note);
			f.close();
		}
		void MainSta::save_file()
		{
			fstream f(save_adress().c_str(), fstream::out | fstream::binary);
			dic.write(f, (function<void(Note*&,fstream&)>)Note::save_Note);
			f.close();
		}
		void Note::save_Note(Note* nt, fstream& f)
		{
			write_string(nt->name, f);
			wrt_vec(*nt->tags, f);
			wrt_vec(nt->note, f, (void(*)(const NoteTip&, fstream&))NoteTip::write);
			wrt_vec(nt->dirs, f, (void(*)(const NoteTip&, fstream&))NoteTip::write);
			wrt_vec(nt->links,f, (void(*)(const NoteTip&, fstream&))NoteTip::write);
		}
		Note* Note::read_Note(fstream& f)
		{
			Note* re = new Note();
			re->name = read_string(f);
			WVec* wv = new WVec();
			*wv = rd_vec<string>(f);
			re->tags = wv;
			re->note = rd_vec<NoteTip>(f, NoteTip::read);
			re->dirs = rd_vec<NoteTip>(f, NoteTip::read);
			re->links = rd_vec<NoteTip>(f, NoteTip::read);
			return re;
		}
		MainSta::MainSta() 
		{
		   	note_sta.main_sta = this;
		   	sta_setup(); 
			read_file();
		}
		void MainSta::ui_list()
		{
			puts("All notes : ");
			prt_strs(dic.data, [](Pair<Note*>* a)->string{return a->data->name;});
		}
		NoteTip NoteTip::read(fstream& f)
		{
			NoteTip re;
			re.name = read_string(f);
			re.val = read_string(f);
			re.comment = read_string(f);
			f.read((char*)&re.type, sizeof(NoteTip::Type));
			return re;
		}
		void NoteTip::write(fstream& f) const
		{
			write_string(name, f);
			write_string(val, f);
			write_string(comment, f);
			f.write((const char*)&type, sizeof(NoteTip::Type));
		}

		Note* MainSta::find(string name)
		{
			for (auto a : dic.data)
				printf("%s\n", a->data->name.c_str());
			for (auto a : dic.data)
				if (a->data->name == name)
					return a->data;
			return NULL;
		}
		void MainSta::ui_open_by_name()
		{
			string name = ui_getval("The name : ");
			Note * re = find(name);
			if (re == NULL) { printf("No note named %s.\n", name.c_str()); return; }
			open(*re);
		}
	}
}
