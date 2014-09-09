#pragma once
#include <iostream>
#include <stdarg.h>
#include <memory> 
#include <vector>
#include "config.h"
#include <cassert>
#include <string>
#include <fstream>
#include <stdexcept>
#include <exception>
using namespace std;
#define xssert(EX) ((void)((EX) || (__assert (#EX, __FILE__, __LINE__),0)))
#define ce_setopt(a,b,c) assert(! curl_easy_setopt(a,b,c))
#define _TO_STR(x) #x
#define TO_STR(x) _TO_STR(x)

class format_error : public runtime_error
{
   	public :  
		format_error() : runtime_error("Bad format.") {}
		format_error(const string& str) : runtime_error(str) {}
};
int to_i(string& str);
int to_i(string& str, int defaul);
bool to_b(string str);
bool to_b(string str, bool defaul);
double to_d(string& str);
double to_d(string& str, double defaul);

void wrt_str(string& str, fstream& f);
string rd_str(fstream& f);

template<typename T, typename V, typename Func>
vector<V> vec_trans(const vector<T>& vec, Func action) 
{
	vector<V> re; re.reserve(vec.size());
	for (auto a : vec) re.push_back(action(a));
	return re;
}

void list_strs(const vector<string>&);
void prt_strs(const vector<string>& ss) ;
template<typename T, typename Func>
void prt_strs(const vector<T>& vec, Func to_string) { prt_strs(vec_trans<T,string>(vec, to_string)); }
template<typename T>
void prt_strs(const vector<T>& vec, function<string(T)> to_string) { prt_strs(vec_trans<T,string>(vec, to_string)); }
int find_strs(vector<string> lst, string target);
int find_strs(const char* lst[], int cnt, const char* target);
int like_strs(const char* lst[], int cnt, const char* target);

string concact(const char* strs[], int n);
string concact(const char* strs[], int n, const char* speration);
string trim(string str, int maxlen);
string string_format(const string fmt_str, ...) ;

bool user_input_newline();
string ui_getval(const string& tink);
bool ui_getbool(const string& tink);
bool like(const string& a, const string& b);

string extract_tilde(const string& str);
#define may_null(may,mark,exp) auto mark=(may) (mark?NULL:(exp)) 

template<typename T, typename Func>
T ui_get(string name, Func convert)
{
	T re;
	while (true)
	{
		printf("%s : ", name.c_str());
		string str; getline(cin, str);
		try { re = convert(str); }
		catch (...) { continue; }
		break;
	}
	return re;
}
