#include "common.h"
#include "act.h"
#include "vecdir/vec.h"
#include "vecdir/dic.h"

using namespace std;

namespace Xero{
	namespace MemNet{

		class Record 
		{
			public:
				string adress;
				WVec* tags;
				string excu_method;

				Record() {}
				Record(string _ad, WVec* _t, string _excu) : adress(_ad), tags(_t), excu_method(_excu) {}

				void excu();
				static string default_excu() { return Config::default_excu; }
				string to_string() ;
				void write(fstream& f);
				static Record* read(fstream& f);
		};
		class Core : public  Status
		{
				void setup_sta();
			public:
				Core() { setup_sta(); read_file(); }
				~Core() { write_file(); }
				Dic<Record*> dic;
				
				void add(Record* rec);
				bool remove(Record* rec);
				void ui_add();
				Record* ui_find();
				void ui_remove();
				void ui_excu();
				void cd(Record* rec);
				void ui_cd();

				void write(fstream& f);
				void read(fstream& f);
				static string file_adress() { return Config::save_adress; }
				void write_file();
				void read_file();
		};
	}
}
