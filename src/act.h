#pragma once
#include "common.h"
#include <vector>
#include <string>

using namespace std;

namespace Xero {
	namespace MemNet {

		class Action
		{
			public:
				typedef void (*ActFunc) ();
				Action(string _name, ActFunc _f) : name(_name), func(_f) {}
				Action(string _name, ActFunc _f, string _comment) :comment(_comment), name(_name), func(_f) {}

				string name;
				string comment;
				ActFunc func;
				void invoke();
		};
		class Status
		{
			public:
				Status() {}
				Status(string _name) : name(_name) {}
				Status(string _name, vector<Action> acts) : name(_name), actions(acts) {}
				vector<Action> actions;
				string name;

				void invoke(string name);
				void add_action(Action& act);
		};
		class StatusMng
		{
			public:
				static Status* current;
				static vector<Action> specials;

				static void init(Status& _default);
				static void trans(Status& status);
				static void invoke(string name);
				static void read_act();
				static void run_interactive();
				static void end_interactive();
				static void show_help();
		};
	}
}
