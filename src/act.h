#pragma once
#include "common.h"
#include <vector>
#include <string>
#include <functional>

using namespace std;

namespace Xero {
	namespace MemNet {

		class ActionNotFound : public runtime_error
		{
			public :
				ActionNotFound() : runtime_error("Action not found") {}
				ActionNotFound(const string& str) : runtime_error(str) {}
		};
		class CommandError : public runtime_error
		{
			public:
				CommandError() : runtime_error("Bad Command.") {}
				CommandError(const string& msg) : runtime_error(msg) {}
		};

		class Status;
		class Action
		{
			public:
				Action() {}
				typedef function<void(Status&)> ActFunc;
				Action(string _name, ActFunc _f) : name(_name), func(_f) {}
				Action(string _name, ActFunc _f, string _comment) :comment(_comment), name(_name), func(_f) {}

				string name;
				string comment;
				ActFunc func;
				void invoke(Status& who);
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
				void add_action(const Action& act);
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
				static void run_cmd();
				static void show_help();
		};
	}
}
