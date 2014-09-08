#include "common.h"
#include "act.h"
#include "vecdir/dic.h"

using namespace std;

namespace Xero {
	namespace MemNet {

		class NoteTip
		{
			public:
				enum Type { note, dir, link };
				string name;
				string val;
				string comment;
				Type type;

				string active();
		};
		string to_string(const NoteTip& tip);

		class Note
		{
			public:
				vector<NoteTip> note;
				vector<NoteTip> dirs;
				vector<NoteTip> links;
				string name;
				WVec* tags;
				void show_tags();
				void show_oneline();
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
				void set_current(Note* note) { current = note; name = string("Note - ") + current->name; }
				
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
		};

		class MainSta : public Status
		{
				Dic<Note*> dic;
				void sta_setup();
			public:
				MainSta() { note_sta.main_sta = this; sta_setup(); }
				NoteSta note_sta;

				void list_sta();
				Dic<Note*>::Result* possible(WVec& tags);
				Note* ui_select(WVec& tags);
				Note* ui_select();

				void open(Note& note);
				void ui_open();

				void create_new(WVec& tags);
				void ui_create_new();

				void drop(Note* nt);
				void tag_move(Note* nt, WVec& tags);
		};
	}
}
