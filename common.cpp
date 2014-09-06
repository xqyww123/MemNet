#include "common.h"
#include <algorithm>

int to_i(string& str)
{
	int re;
	if (1 != sscanf(str.c_str(), "%d", &re))
		throw "Bad Integer!";
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
	if (str=="true" || str=="t") return true;
	else if (str == "false" || str=="f") return false;
	throw "Bad bool = =";
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
		throw "Bad Double!";
	return re;
}
double to_d(string& str, double defaul)
{
	double re;
	if (1 != sscanf(str.c_str(), "%lf", &re))
		return defaul;
	return re;
}
