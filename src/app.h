#include "common.h"
#include "act.h"
#include "vecdir/dic.h"

using namespace std;

namespace Xero {
	namespace MemNet {

		class NoteTip
		{
			public:
				enum Type { note, dir, link, badtype};
				string name;
				string val;
				string comment;
				Type type;

				void active();
				static NoteTip read(fstream& f);
				void write(fstream& f) const;
				static void write(const NoteTip& a, fstream& f) { a.write(f); }
		};
		string to_string(const NoteTip& tip);

		class Note
		{
			public:
				vector<NoteTip> note;
				vector<NoteTip> dirs;
				vector<NoteTip> links;
				string name;
				string workdir() { return Config::notedir + "/" + name; }
				WVec* tags;
				void show_tags();
				void show_oneline();
				// return new note's reference in vectors
				NoteTip& add_tip(const NoteTip& tip); 
				NoteTip& add_tip(const string& name, const string& val, NoteTip::Type type);
				bool contains_tip(string name);

				static void save_Note(Note* nt, fstream& f);
				//void save_Note(fstream& f) { save_Note(this, f); }
				static Note* read_Note(fstream& f);
		};

		class MainSta;
		class NoteSta : public Status
		{
				Note* current;
				void setup_sta();
			public:
				NoteSta() { setup_sta(); }
				MainSta* main_sta;
				Note* get_current() { return current;  }
				void set_current(Note* note) ;
				
				void list_note();
				void list_dirs();
				void list_links();
				void list_all();
				void list(string name);
				void ui_list();

				static void active(vector<NoteTip>& tips, string name);
				void active(string name);
				void active_note(string name) { active(current->note, name); }
				void active_dirs(string name) { active(current->dirs, name); }
				void active_links(string name) { active(current->links, name); }
				void ui_active();

				void show_info();
				void close();
				void ui_drop();
				void ui_move();
				void ui_addtip();
		};

		class MainSta : public Status
		{
				Dic<Note*> dic;
				void sta_setup();
			public:
				MainSta() ;
				~MainSta() { save_file(); }
				string save_adress() { return Config::notedir + "/info"; }
				NoteSta note_sta;

				void list_sta();
				Dic<Note*>::Result* possible(WVec& tags);
				Note* ui_select(WVec& tags);
				Note* ui_select();

				void open(Note& note);
				void ui_open();
				Note* find(string name);
				void ui_open_by_name();

				Note* create_new(WVec& tags);
				void ui_create_new();
				void ui_list();

				void drop(Note* nt);
				void tag_move(Note* nt, WVec& tags);
				void read_file();
				void save_file();
		};
	}
}
