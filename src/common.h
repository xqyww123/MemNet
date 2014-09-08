#pragma once
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

class NoteTip;
string to_string(const NoteTip& tip);
template<typename T, typename V>
vector<V> vec_trans(vector<T> vec, V (*action)(const T&)) 
{
	vector<V> re; re.reserve(vec.size());
	for (auto a : vec) re.push_back(action(a));
	return re;
}

void list_strs(const vector<string>&);
template<typename T>
void prt_strs(const vector<T>& vec) { prt_strs(vec_trans<T,string>(Xero::MemNet::to_string)); }
template<>
void prt_strs(const vector<string>& ss) ;
int find_strs(vector<string> lst, string target);
int find_strs(const char* lst[], int cnt, const char* target);
int like_strs(const char* lst[], int cnt, const char* target);

string concact(const char* strs[], int n);
string concact(const char* strs[], int n, const char* speration);
string trim(string str, int maxlen);
string string_format(const string fmt_str, ...) ;
