#include "act.h"
#include <iostream>

using namespace std;

namespace Xero{
	namespace MemNet{


		Status* StatusMng::current;
		vector<Action> StatusMng::specials;
		void StatusMng::init(Status& _default)
		{
			trans(_default);
			specials.push_back(Action("exit", [](Status& a){StatusMng::end_interactive();}));
			specials.push_back(Action("?", [](Status& a){StatusMng::show_help();}));
			specials.push_back(Action("help", [](Status& a){StatusMng::show_help();}));
		}
		void Action::invoke(Status& who) { func(who); }
		void Status::invoke(string name)
		{
			for (auto a : actions)
			{
				if (a.name == name) { a.invoke(*this); return; }
			}
			throw out_of_range((string("Action ") + name + " not found = =!"));
		}
		void StatusMng::trans(Status& status)
		{
			current = &status;
		}
		void StatusMng::invoke(string name)
		{
			try { current->invoke(name); }
			catch (const out_of_range& e) {
				for (auto a : specials) 
				{ if (a.name==name) {a.invoke(*current); return; }}
				throw e;
			}
		}
		void StatusMng::read_act()
		{
			printf("memnet : %s >", current->name.c_str());
			string name;
			cin >> name;
			invoke(name);
		}
		bool interactive_break;
		void StatusMng::end_interactive() { interactive_break = true; }
		void StatusMng::run_interactive()
		{
			interactive_break = false;
			while (!interactive_break)
			{
				read_act();
			}
		}
		void StatusMng::show_help()
		{
			puts("Usable method:");
			for (auto a : current->actions)
				printf("%s\t\t%s\n", a.name.c_str(), a.comment.c_str());
			puts("");
		}
		void Status::add_action(const Action& act)
		{
			for (int i=0;i<actions.size();++i)
				if (actions[i].name == act.name)
				{ actions[i] = act; return; }
			actions.push_back(act);
		}

	}
}
