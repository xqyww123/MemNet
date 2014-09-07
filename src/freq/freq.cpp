#include "freq.h"
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
		
		void freq_init(const char* adress)
		{
			fstream f(adress, fstream::in);
			while (true)
			{
				string w; float freq;
				f>>w>>freq;
				if (f.eof()) break;
				data.insert(make_pair<string, float>((string)w,(float)freq));
			}
			f.close();
			freq_most=freq_query("_MOST_");
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
