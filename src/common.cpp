#include "common.h"
#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
using namespace std;

int to_i(string& str)
{
	int re;
	if (1 != sscanf(str.c_str(), "%d", &re))
		throw format_error("Bad Integer!");
	return re;
}
int to_i(string& str, int defaul)
{
	int re;
	if (1 != sscanf(str.c_str(), "%d", &re))
		return defaul;
	return re;
}
bool to_b(string str)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	if (like(str,"true") || like(str, "yes")) return true;
	else if (like(str, "false") || like(str, "no")) return false;
	throw format_error("Bad bool = =");
}
bool to_b(string str, bool defaul)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	if (str=="true" || str=="t") return true;
	else if (str == "false" || str=="f") return false;
	return defaul;
}
double to_d(string& str)
{
	double re;
	if (1 != sscanf(str.c_str(), "%lf", &re))
		throw format_error("Bad Double!");
	return re;
}
double to_d(string& str, double defaul)
{
	double re;
	if (1 != sscanf(str.c_str(), "%lf", &re))
		return defaul;
	return re;
}
void wrt_str(string& str, fstream& f)
{
	f.write(str.c_str(), sizeof(char[str.length()+1]));
}
string rd_str(fstream& f)
{
	char* buf = new char[255];
	int bi=0;
	do { f.read(buf+bi, sizeof(char)); }
	while (buf[bi++] != '\0' && bi != 255);
	if (bi == 255) throw runtime_error("The string is too long to read = =.");
	string re(buf);
	delete buf;
	return re;
}
void list_strs(const vector<string>& ss)
{
	if (ss.empty()) return;
	string sperate;
	if (ss[0].length() > 20) sperate = "\n";
	else sperate = "\t\t";
	for (auto a : ss)
	{
		cout << a << sperate;
	}
	cout << endl;
}
int find_strs(const vector<string>& lst, string target)
{
	for (int i=0;i<lst.size();++i)
		if (lst[i] == target) return i;
	return -1;
}
int like_strs(const char* lst[], int cnt, const char* target)
{
	int len = strlen(target);
	for (int i=0;i<cnt;++i)
		if (strlen(lst[i]) >= len && !strncmp(lst[i], target, len))
			return i;
	return -1;
}
int find_strs(const char* lst[], int cnt, const char* target)
{
	int len = strlen(target);
	for (int i=0;i<cnt;++i)
		if (strlen(lst[i]) == len && !strcmp(lst[i], target))
			return i;
	return -1;
}
string concact(const char* strs[], int n)
{
	if (!n) return string();
	string re(strs[0]);
	for (int i=1;i<n;++i)
		re += string(strs[i]);
	return re;
}
string concact(const char* strs[], int n, const char* speration)
{
	if (!n) return string();
	string re(strs[0]);
	for (int i=1;i<n;++i)
		re += string(speration) + strs[i];
	return re;
}
string trim(string str, int maxlen)
{
	if (str.length() <= maxlen) return str;
	return string(str.c_str(), maxlen);
}
// code from http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
string string_format(const string fmt_str, ...) 
{
    int final_n, n = ((int)fmt_str.size()) * 2; 
    string str;
    unique_ptr<char[]> formatted;
    va_list ap;
    while(1) {
        formatted.reset(new char[n]); 
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    return string(formatted.get());
}
// ref end

void prt_strs(const vector<string>& ss) { list_strs(ss); }
bool user_input_newline()
{
	while (true)
	{
		int c = getchar();
		switch(c)
		{
			case '\n':
				ungetc(c, stdin);
				return true;
			case ' ':
			case '\t':
			case '\r':
			case '\v':
			case '\f':
				continue;
			default:
				ungetc(c,stdin);
				return false;
		}
	}
}
string ui_getval(const string& tink)
{
	string re;
	if (user_input_newline()) cout << tink;
	cin >> re;
	return re;
}
bool ui_getbool(const string& tink)
{
	while (true)
	{
		try { return to_b(ui_getval(tink)); }
		catch (const format_error&) { puts("Bad format."); continue; }
		break;
	}
}
bool like(const string& a, const string& b)
{
	return !strncmp(a.c_str(), b.c_str(), min(a.length(), b.length()));
}
string extract_tilde(const string& str)
{
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	string re;
	for (auto a : str)
	{
		if (a == '~')
			re.append(homedir);
		else re.push_back(a);
	}
	return re;
}
