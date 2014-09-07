#pragma once
#include "config.h"
#include <cassert>
#include <string>
using namespace std;
#define xssert(EX) ((void)((EX) || (__assert (#EX, __FILE__, __LINE__),0)))
#define ce_setopt(a,b,c) assert(! curl_easy_setopt(a,b,c))
#define _TO_STR(x) #x
#define TO_STR(x) _TO_STR(x)

int to_i(string& str);
int to_i(string& str, int defaul);
bool to_b(string str);
bool to_b(string str, bool defaul);
double to_d(string& str);
double to_d(string& str, double defaul);
