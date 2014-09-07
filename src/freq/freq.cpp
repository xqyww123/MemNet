#include "freq.h"
#include <algorithm>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <cstdio>
#include <fstream>
#include <utility> 
using namespace std;

namespace Xero
{
	namespace MemNet
	{
		float freq_most;
		typedef unordered_map<string, float> freq_table;
		freq_table data;
		
		char easytolower(char in){
			  if(in<='Z' && in>='A')
				      return in-('Z'-'z');
			    return in;
		} 
		void freq_init(const char* adress)
		{
			fstream f(adress, fstream::in);
			if (!f.good()) { fprintf(stderr, "Error = = : freq data read fail\n File : %d\n", adress); exit(1); }
			while (true)
			{
				string w; float freq;
				f>>w>>freq;
				transform(w.begin(), w.end(), w.begin(), easytolower);
				if (f.eof()) break;
				data.insert(make_pair<string, float>((string)w,(float)freq));
			}
			f.close();
			freq_most=freq_query("_most_");
		}
		double freq_query(string str)
		{
			try { return data.at(str); }
			catch (const out_of_range& e) { return 0.0; }
		}
		void freq_free()
		{
			;
		}
	}
}
