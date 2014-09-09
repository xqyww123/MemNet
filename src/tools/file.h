#pragma once
#include "../common.h"
#include <fstream>

using namespace std;

namespace Xero {
	namespace MemNet{
#define wrt(x) write((char*)&x,sizeof(x))
#define wr2(a,b) write((char*)a,sizeof(b))
#define rd(x) read((char*)&x, (streamsize)sizeof(x))
#define rd2(x,b) read((char*)x, sizeof(b))
		string read_string(fstream& f);
		void write_string(const string& str, fstream& f);
		template<typename T>
		void wrt_vec (vector<T>& vec, fstream& f) 
		{
			auto tmpsz = vec.size();
			f.wrt(tmpsz);
			for (auto a : vec)
				f.wrt(a);
		}
		template<typename T, typename Write>
		void wrt_vec (vector<T>& vec, fstream& f, Write func) 
		{
			auto tmpsz = vec.size();
			f.wrt(tmpsz);
			for (auto a : vec)
				func(a, f);
		}
		template<>
		void wrt_vec (vector<string>& vec, fstream& f);
		template<typename T>
		vector<T> rd_vec (fstream& f)
		{
			typename vector<T>::size_type sz; f.rd(sz);
			vector<T> re; re.reserve(sz);
			for (typename vector<T>::size_type i=0;i<sz;++i)
			{
				T some; f.rd(some);
				re.push_back(some);
			}
			return re;
		}
		template<typename T, typename Read>
		vector<T> rd_vec (fstream& f, Read func)
		{
			typename vector<T>::size_type sz; f.rd(sz);
			vector<T> re; re.reserve(sz);
			for (typename vector<T>::size_type i=0;i<sz;++i)
			{
				T some = func(f);
				re.push_back(some);
			}
			return re;
		}
		template<>
		vector<string> rd_vec(fstream &f);
	}
}
#undef wrt
#undef wr2
#undef rd
#undef rd2
