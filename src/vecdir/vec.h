#pragma once
#include "../common.h"
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

namespace Xero
{
	namespace MemNet
	{
		typedef vector<string> WVec; // Words Vector
		typedef vector<float> FreqVec; // Frequence Vector
		typedef unordered_map<string, int> Freqs;
		typedef unordered_map<string, float> Frates;
		class Vec 
		{
			public:
				WVec wvec;
				WVec spread;
				FreqVec freq;

				static Freqs* cal_freq(WVec* wvec);
				static float cal_rate(int num, string word);
				static Frates* cal_rate(Freqs*);
				static Vec* cal_spread(Frates*, WVec*);
				static Vec* cal_spread(WVec*);
				static WVec* read_wvec(FILE* f);
				static void init();
		};

		char* prt_WVec(WVec* wvec);
	}
}
